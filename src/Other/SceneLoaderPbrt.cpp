#include "SceneLoaderPbrt.h"

#include "Assets.h"
#include "Camera.h"
#include "Graphical.h"
#include "Light.h"
#include "Material.h"
#include "minipbrt.h"
#include "Model.h"
#include "MyMath.h"
#include "Renderer.h"
#include "Scene.h"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/matrix_decompose.hpp"

void SceneLoaderPbrt::loadScene(const std::string& path)
{
	TimeMeasurer tm;

	minipbrt::Loader loader;
	if (!loader.load(path.c_str()) || !loader.borrow_scene()->load_all_ply_meshes())
	{
		auto err = loader.error();
		fprintf(stderr, "[%s, line %lld, column %lld] %s\n", err->filename(), err->line(), err->column(), err->message());
		return;
	}
	tm.printElapsedFromLast("Scene loaded in ");

	minipbrt::Scene* scene = loader.take_scene();

	loadScene_camera(scene);
	tm.printElapsedFromLast("Camera loaded in ");

	std::vector<Texture*> parsedTextures;
	loadScene_textures(scene, parsedTextures);
	tm.printElapsedFromLast("Textures loaded in ");

	std::vector<Material*> materials;
	loadScene_materials(scene, parsedTextures, materials);
	tm.printElapsedFromLast("Materials loaded in ");

	loadScene_shapes(scene, materials);
	tm.printElapsedFromLast("Shapes loaded in ");

	loadScene_lights(scene);
	tm.printElapsedFromLast("Lights loaded in ");

	delete scene;

	if (Scene::baseTriangles.empty())
		new Cube({}, 0);
}

void SceneLoaderPbrt::loadScene_camera(minipbrt::Scene* scene)
{
	switch (scene->camera->type())
	{
		case minipbrt::CameraType::Perspective:
		{
			auto c = dynamic_cast<const minipbrt::PerspectiveCamera*>(scene->camera);

			auto transform = transpose(glm::make_mat4x4(&c->cameraToWorld.start[0][0]));

			glm::vec3 scale;
			glm::quat rot;
			glm::vec3 pos;
			glm::vec3 skew;
			glm::vec4 perspective;

			decompose(transform, scale, rot, pos, skew, perspective);

			float pitch = glm::pitch(rot) * RAD_TO_DEG;
			float yaw = glm::yaw(rot) * RAD_TO_DEG;
			float roll = glm::roll(rot) * RAD_TO_DEG;

			auto cam = new Camera(pos, c->fov, c->lensradius);
			cam->setRot(pitch, yaw, roll);
			cam->setScale(scale);
		}
		break;
	}
}
void SceneLoaderPbrt::loadScene_textures(const minipbrt::Scene* scene, std::vector<Texture*>& parsedTextures)
{
	parsedTextures.reserve(scene->textures.size());
	for (int i = 0; i < scene->textures.size(); i++)
	{
		auto tex = scene->textures[i];

		Texture* parsedTex = Texture::defaultTex();
		switch (tex->type())
		{
			case minipbrt::TextureType::ImageMap:
			{
				auto t = dynamic_cast<const minipbrt::ImageMapTexture*>(tex);
				parsedTex = Assets::load<Texture>(t->filename);
				break;
			}
			case minipbrt::TextureType::Constant:
			{
				auto t = dynamic_cast<const minipbrt::ConstantTexture*>(tex);
				parsedTex = new Texture(Color(t->value[0], t->value[1], t->value[2]));
				break;
			}
			case minipbrt::TextureType::Scale:
			{
				auto t = dynamic_cast<const minipbrt::ScaleTexture*>(tex);
				parsedTex = parsedTextures[t->tex1.texture];
				break;
			}
			default:
				Debug::logError("!!! Unsupported texture type: ", static_cast<int>(tex->type()));
				break;
		}

		parsedTex->setName(tex->name);
		parsedTextures.push_back(parsedTex);
	}
}
void SceneLoaderPbrt::loadScene_materials(const minipbrt::Scene* scene, const std::vector<Texture*>& parsedTextures, std::vector<Material*>& materials)
{
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
				if (m->Kd.texture != minipbrt::kInvalidIndex)
					tex = parsedTextures[m->Kd.texture];
				else
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
			case minipbrt::MaterialType::Plastic:
			{
				auto m = dynamic_cast<const minipbrt::PlasticMaterial*>(mat);
				if (m->Kd.texture != minipbrt::kInvalidIndex)
					tex = parsedTextures[m->Kd.texture];
				else
					baseColor = Color(m->Kd.value[0], m->Kd.value[1], m->Kd.value[2]);
				roughness = m->roughness.value;
				break;
			}
			default:
				break;
		}
		materials.push_back(new Material(baseColor, lit, tex, roughness, metallic, emission));
	}
}

void SceneLoaderPbrt::loadScene_shapes(const minipbrt::Scene* scene, const std::vector<Material*>& materials)
{
	#pragma omp parallel for
	for (int shapeInd = 0; shapeInd < scene->shapes.size(); ++shapeInd)
	{
		auto shape = scene->shapes[shapeInd];

		bool isPartOfPrefab = false;
		for (auto obj : scene->objects)
		{
			if (obj->firstShape <= shapeInd && obj->firstShape + obj->numShapes > shapeInd)
				isPartOfPrefab = true;
		}
		if (isPartOfPrefab) continue;

		auto obj = spawnObjectFromShape(shape, materials);
		if (obj != nullptr)
		{
			auto transform = transpose(glm::make_mat4x4(&shape->shapeToWorld.start[0][0]));
			obj->setTransform(transform);
		}
	}
	int count = 0;
	for (const auto* inst : scene->instances)
	{
		count++;
		if (count > 100) continue;
		auto obj = scene->objects[inst->object];

		glm::mat4 objToInst = transpose(glm::make_mat4x4(&obj->objectToInstance.start[0][0]));
		glm::mat4 instToWorld = transpose(glm::make_mat4x4(&inst->instanceToWorld.start[0][0]));

		for (uint32_t i = 0; i < obj->numShapes; ++i)
		{
			auto shape = scene->shapes[obj->firstShape + i];
			auto spawned = spawnObjectFromShape(shape, materials);
			if (!spawned) continue;

			glm::mat4 shapeToWorld = transpose(glm::make_mat4x4(&shape->shapeToWorld.start[0][0]));
			glm::mat4 finalTransform = instToWorld * objToInst * shapeToWorld;

			spawned->setTransform(finalTransform);
		}
	}
}
Object* SceneLoaderPbrt::spawnObjectFromShape(minipbrt::Shape* shape, const std::vector<Material*>& materials)
{
	static std::mutex mutex;

	Object* obj = nullptr;
	switch (shape->type())
	{
		case minipbrt::ShapeType::TriangleMesh:
		{
			obj = spawnObjectFromShape_triMesh(materials, shape, mutex);
			break;
		}
		case minipbrt::ShapeType::Sphere:
		{
			auto sphere = dynamic_cast<const minipbrt::Sphere*>(shape);
			auto center = glm::vec3(shape->shapeToWorld.start[3][0], shape->shapeToWorld.start[3][1], shape->shapeToWorld.start[3][2]);

			auto sphereObj = new Sphere(center, sphere->radius);
			sphereObj->setSharedMaterial(materials[shape->material]);

			obj = sphereObj;
			break;
		}
		default:
		{
			Debug::logError("!!! Unsupported shape type: ", static_cast<int>(shape->type()));
			break;
		}
	}

	return obj;
}
Mesh* SceneLoaderPbrt::spawnObjectFromShape_triMesh(const std::vector<Material*>& materials, const minipbrt::Shape* shape, std::mutex& mutex)
{
	auto mesh = dynamic_cast<const minipbrt::TriangleMesh*>(shape);

	auto points = mesh->P;
	auto normals = mesh->N;
	auto uvs = mesh->uv;
	auto indices = mesh->indices;

	auto numTris = mesh->num_indices / 3;
	std::vector<BaseTriangle*> tris(numTris);

	#pragma omp parallel for
	for (int i = 0; i < numTris; ++i)
	{
		Vertex v[3];
		for (int j = 0; j < 3; ++j)
		{
			auto idx = indices[i * 3 + j];

			v[j].pos = {points[3 * idx + 0], points[3 * idx + 1], points[3 * idx + 2]};
			if (normals) v[j].normal = {normals[3 * idx + 0], normals[3 * idx + 1], normals[3 * idx + 2]};
			if (uvs) v[j].uvPos = {Math::mod(uvs[2 * idx + 0], 1.0f), Math::mod(uvs[2 * idx + 1], 1.0f)};
		}

		tris[i] = new BaseTriangle(v[0], v[1], v[2]);
	}

	mutex.lock();
	auto meshObj = new Mesh(new Model(tris));
	mutex.unlock();

	if (mesh->material != minipbrt::kInvalidIndex)
		meshObj->setSharedMaterial(materials[mesh->material]);

	auto transform = transpose(glm::make_mat4x4(&shape->shapeToWorld.start[0][0]));
	meshObj->setTransform(transform);

	return meshObj;
}

void SceneLoaderPbrt::loadScene_lights(minipbrt::Scene* scene)
{
	for (auto light : scene->lights)
	{
		auto transform = glm::make_mat4x4(&light->lightToWorld.start[0][0]);
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

			if (il->mapname != nullptr)
			{
				auto tex = Assets::load<Texture>(il->mapname);
				Renderer::renderProgram()->setBool("useEnvMap", true);
				Renderer::renderProgram()->setHandle("envMap", tex->glTex()->getHandle());
			}
		}
	}

	for (auto areaLight : scene->areaLights)
	{
		auto dl = dynamic_cast<const minipbrt::DiffuseAreaLight*>(areaLight);
	}
}
