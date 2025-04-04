#include "SceneLoaderPbrt.h"

#include "Camera.h"
#include "Graphical.h"
#include "Light.h"
#include "Material.h"
#include "minipbrt.h"
#include "Model.h"
#include "MyMath.h"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/matrix_decompose.hpp"

void SceneLoaderPbrt::loadScene(const std::string& path)
{
	minipbrt::Loader loader;
	if (!loader.load(path.c_str()) || !loader.borrow_scene()->load_all_ply_meshes())
	{
		auto err = loader.error();
		fprintf(stderr, "[%s, line %lld, column %lld] %s\n", err->filename(), err->line(), err->column(), err->message());
		return;
	}

	minipbrt::Scene* scene = loader.take_scene();

	// === Load camera ===
	if (scene->camera)
	{
		switch (scene->camera->type())
		{
			case minipbrt::CameraType::Perspective:
			{
				auto c = dynamic_cast<const minipbrt::PerspectiveCamera*>(scene->camera);

				auto transform = transpose(glm::make_mat4x4(&c->cameraToWorld.start[0][0]));

				glm::mat4 yUpToZUp = rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1, 0, 0));

				// Apply rotation only to the upper-left 3x3 rotation part
				glm::mat3 rotOnly = glm::mat3(transform);
				rotOnly = glm::mat3(yUpToZUp) * rotOnly;

				// Rebuild the final transform with adjusted rotation and original translation
				transform = glm::mat4(1.0f);
				transform = glm::mat4(rotOnly);                      // set new rotation
				transform[3] = glm::vec4(glm::vec3(transpose(glm::make_mat4x4(&c->cameraToWorld.start[0][0]))[3]), 1.0f); // restore original translation

				glm::vec3 scale;
				glm::quat rot;
				glm::vec3 pos;
				glm::vec3 skew;
				glm::vec4 perspective;

				decompose(transform, scale, rot, pos, skew, perspective);

				auto cam = new Camera(pos, c->fov, c->lensradius);
				cam->setRot(pitch(rot), yaw(rot));
			}
			break;
		}
	}
	else
		Debug::logError("No camera found in the scene.");

	// === Load materials ===
	std::vector<Material*> materials;
	materials.reserve(scene->materials.size());
	for (auto mat : scene->materials)
	{
		auto baseColor = Color::white();
		auto lit = true;
		auto roughness = 1.0f;
		auto metallic = 0.0f;
		auto emission = Color::clear();
		auto tex = Texture::defaultTex();

		switch (mat->type())
		{
			case minipbrt::MaterialType::Matte:
			{
				auto m = dynamic_cast<const minipbrt::MatteMaterial*>(mat);
				baseColor = Color(m->Kd.value[0], m->Kd.value[1], m->Kd.value[2]);
				break;
			}
			case minipbrt::MaterialType::Mirror:
			{
				auto m = dynamic_cast<const minipbrt::MirrorMaterial*>(mat);
				baseColor = Color(m->Kr.value[0], m->Kr.value[1], m->Kr.value[2]);
				roughness = 0.0f;
				break;
			}
			case minipbrt::MaterialType::Metal:
			{
				auto m = dynamic_cast<const minipbrt::MetalMaterial*>(mat);
				baseColor = Color(m->eta.value[0], m->eta.value[1], m->eta.value[2]);
				metallic = 1.0f;
				roughness = (m->uroughness.value + m->vroughness.value) * 0.5f;
				break;
			}
			case minipbrt::MaterialType::Glass:
			{
				auto m = dynamic_cast<const minipbrt::GlassMaterial*>(mat);
				baseColor = Color(m->Kt.value[0], m->Kt.value[1], m->Kt.value[2]);
				roughness = 0.0f;
				break;
			}
			default:
				break;
		}

		materials.push_back(new Material(baseColor, lit, tex, roughness, metallic, emission));
	}

	// === Load triangle meshes ===
	for (auto shape : scene->shapes)
	{
		if (shape->type() != minipbrt::ShapeType::TriangleMesh) continue;

		auto mesh = dynamic_cast<const minipbrt::TriangleMesh*>(shape);
		if (!mesh) continue;

		auto points = mesh->P;
		auto normals = mesh->N;
		auto uvs = mesh->uv;
		auto indices = mesh->indices;

		auto numTris = mesh->num_indices / 3;
		std::vector<BaseTriangle*> tris;
		tris.reserve(numTris);
		for (int i = 0; i < numTris; ++i)
		{
			Vertex v[3];
			for (int j = 0; j < 3; ++j)
			{
				auto idx = indices[i * 3 + j];

				v[j].pos = {points[3 * idx + 0], points[3 * idx + 1], points[3 * idx + 2]};
				if (normals) v[j].normal = {normals[3 * idx + 0], normals[3 * idx + 1], normals[3 * idx + 2]};
				if (uvs) v[j].uvPos = {Utils::mod(uvs[2 * idx + 0], 1.0f), Utils::mod(uvs[2 * idx + 1], 1.0f)};
			}

			tris.push_back(new BaseTriangle(v[0], v[1], v[2]));
		}

		auto meshObj = new Mesh(new Model(tris));
		meshObj->setSharedMaterial(materials[mesh->material]);
	}

	// === Load lights ===
	for (auto light : scene->lights)
	{
		auto transform = transpose(glm::make_mat4x4(&light->lightToWorld.start[0][0]));
		auto pos = glm::vec3(transform[3][0], transform[3][1], transform[3][2]);

		if (light->type() == minipbrt::LightType::Point)
		{
			auto pt = dynamic_cast<const minipbrt::PointLight*>(light);
			new PointLight(pos + glm::vec3(pt->from[0], pt->from[1], pt->from[2]), {pt->I[0], pt->I[1], pt->I[2]}, 1.0f, 1);
		}
		else if (light->type() == minipbrt::LightType::Distant)
		{
			auto dl = dynamic_cast<const minipbrt::DistantLight*>(light);

			auto dir = glm::vec3(transform[2][0], transform[2][1], transform[2][2]);
			new DirectionalLight(dir, {dl->L[0], dl->L[1], dl->L[2]}, 1.0f);
		}
		else if (light->type() == minipbrt::LightType::Infinite)
		{
			auto il = dynamic_cast<const minipbrt::InfiniteLight*>(light);
			Camera::instance->setBgColor(Color(il->L[0], il->L[1], il->L[2]));
		}
	}

	for (auto areaLight : scene->areaLights)
	{
		auto dl = dynamic_cast<const minipbrt::DiffuseAreaLight*>(areaLight);
	}

	delete scene;
}
