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
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/matrix_decompose.hpp"

void SceneLoaderPbrt::loadScene(const std::string& path)
{
	Debug::log("Loading PBRT scene...");
	TimeMeasurer tm;

	minipbrt::Loader loader;
	if (!loader.load(path.c_str()) || !loader.borrow_scene()->load_all_ply_meshes())
	{
		auto err = loader.error();
		fprintf(stderr, "[%s, line %lld, column %lld] %s\n", err->filename(), err->line(), err->column(), err->message());
		return;
	}
	tm.printElapsedFromLast("   Scene loaded in ");

	auto scene = loader.take_scene();

	loadScene_camera(scene);
	tm.printElapsedFromLast("   Camera loaded in ");

	std::vector<Texture*> parsedTextures;
	loadScene_textures(scene, parsedTextures);
	tm.printElapsedFromLast("   Textures loaded in ");

	std::vector<Material*> materials;
	loadScene_materials(scene, parsedTextures, materials);
	tm.printElapsedFromLast("   Materials loaded in ");

	loadScene_lights(scene);
	tm.printElapsedFromLast("   Lights loaded in ");

	loadScene_objects(scene, materials);
	tm.printElapsedFromLast("   Shapes loaded in ");

	delete scene;
	tm.printElapsed("PBRT scene loaded in ");
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

		Texture* parsedTex = nullptr;
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
			case minipbrt::TextureType::Windy:
			{
				auto t = dynamic_cast<const minipbrt::WindyTexture*>(tex);
				parsedTex = new WindyTexture(Color::white(), t->scale * 6.0f, 60.0f);

				break;
			}
			default:
				Debug::logError("!!! Unsupported texture type: ", static_cast<int>(tex->type()));
				break;
		}

		if (parsedTex)
			parsedTex->setName(tex->name);
		parsedTextures.push_back(parsedTex);
	}
}
void SceneLoaderPbrt::loadScene_materials(const minipbrt::Scene* scene, const std::vector<Texture*>& parsedTextures, std::vector<Material*>& materials)
{
	materials.resize(scene->materials.size());
	for (int i = 0; i < scene->materials.size(); i++)
	{
		auto mat = scene->materials[i];

		auto baseColor = Color::white();
		auto specColor = Color::clear();
		auto lit = true;
		auto roughness = 1.0f;
		auto metallic = 0.0f;
		auto emission = Color::clear();
		auto tex = Texture::defaultTex();
		auto opacity = 1.0f;
		Texture* opacityTex = nullptr;

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
				metallic = 1.0f;

				if (m->bumpmap != minipbrt::kInvalidIndex)
					tex = parsedTextures[m->bumpmap];
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
				baseColor = Color(m->Kr.value[0], m->Kr.value[1], m->Kr.value[2]);
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
				specColor = Color(m->Ks.value[0], m->Ks.value[1], m->Ks.value[2]);
				roughness = m->roughness.value;
				break;
			}
			case minipbrt::MaterialType::Uber:
			{
				auto m = dynamic_cast<const minipbrt::UberMaterial*>(mat);
				if (m->Kd.texture != minipbrt::kInvalidIndex)
					tex = parsedTextures[m->Kd.texture];
				else
					baseColor = Color(m->Kd.value[0], m->Kd.value[1], m->Kd.value[2]);
				specColor = Color(m->Ks.value[0], m->Ks.value[1], m->Ks.value[2]);

				if (m->opacity.texture != minipbrt::kInvalidIndex)
					opacityTex = parsedTextures[m->opacity.texture];
				else
					opacity = (m->opacity.value[0] + m->opacity.value[1] + m->opacity.value[2]) / 3.0f;
				roughness = (m->uroughness.value + m->vroughness.value) * 0.5f;
				break;
			}
			default:
				Debug::logError("!!! Unsupported material type: ", static_cast<int>(mat->type()));
				break;
		}

		if (mat->bumpmap != minipbrt::kInvalidIndex)
			int x = 1;

		materials[i] = new Material(baseColor, lit, tex, roughness, metallic, emission, opacity, opacityTex, specColor);
	}
}

void SceneLoaderPbrt::loadScene_objects(const minipbrt::Scene* scene, const std::vector<Material*>& materials)
{
	auto models = loadScene_objects_loadModels(scene);

	//#pragma omp parallel for
	for (int shapeInd = 0; shapeInd < scene->shapes.size(); ++shapeInd)
	{
		auto shape = scene->shapes[shapeInd];

		bool isInstanced = false;
		for (auto obj : scene->objects)
		{
			if (obj->firstShape <= shapeInd && obj->firstShape + obj->numShapes > shapeInd)
				isInstanced = true;
		}
		if (isInstanced) continue;

		auto spawned = spawnObjectFromShape(shape, materials, models[shapeInd], scene);
		if (!spawned) continue;

		auto transform = transpose(glm::make_mat4x4(&shape->shapeToWorld.start[0][0]));
		spawned->setTransform(transform);

		if (shape->object != minipbrt::kInvalidIndex)
			spawned->setName(scene->objects[shape->object]->name);
	}

	int count = 0;
	for (const auto* inst : scene->instances)
	{
		count++;
		if (count > 100) continue;
		auto obj = scene->objects[inst->object];

		auto objToInst = transpose(glm::make_mat4x4(&obj->objectToInstance.start[0][0]));
		auto instToWorld = transpose(glm::make_mat4x4(&inst->instanceToWorld.start[0][0]));

		for (uint32_t i = 0; i < obj->numShapes; ++i)
		{
			auto shape = scene->shapes[obj->firstShape + i];
			auto spawned = spawnObjectFromShape(shape, materials, models[obj->firstShape + i], scene);
			if (!spawned) continue;

			auto shapeToWorld = transpose(glm::make_mat4x4(&shape->shapeToWorld.start[0][0]));
			auto finalTransform = instToWorld * objToInst * shapeToWorld;

			spawned->setTransform(finalTransform);
			spawned->setName(obj->name);
		}
	}
}

std::vector<Model*> SceneLoaderPbrt::loadScene_objects_loadModels(const minipbrt::Scene* scene)
{
	std::vector<Model*> models(scene->shapes.size());
	for (int shapeInd = 0; shapeInd < scene->shapes.size(); shapeInd++)
	{
		auto shape = scene->shapes[shapeInd];
		if (shape->type() != minipbrt::ShapeType::TriangleMesh) continue;

		auto mesh = dynamic_cast<const minipbrt::TriangleMesh*>(shape);
		auto tris = loadModelTriangles(mesh);

		models[shapeInd] = new Model(tris);
	}
	return models;
}
std::vector<BaseTriangle*> SceneLoaderPbrt::loadModelTriangles(const minipbrt::TriangleMesh* mesh)
{
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
	return tris;
}

Graphical* SceneLoaderPbrt::spawnObjectFromShape(const minipbrt::Shape* shape, const std::vector<Material*>& materials, Model* model, const minipbrt::Scene* scene)
{
	Graphical* obj = nullptr;
	switch (shape->type())
	{
		case minipbrt::ShapeType::TriangleMesh:
		{
			obj = new Mesh(model);
			break;
		}
		case minipbrt::ShapeType::Sphere:
		{
			auto sphere = dynamic_cast<const minipbrt::Sphere*>(shape);
			obj = new Sphere({0, 0, 0}, sphere->radius);
			break;
		}
		case minipbrt::ShapeType::Disk:
		{
			auto disk = dynamic_cast<const minipbrt::Disk*>(shape);
			obj = new Disk({0, 0, 0}, disk->radius);
			break;
		}
		default:
		{
			Debug::logError("!!! Unsupported shape type: ", static_cast<int>(shape->type()));
			return nullptr;
		}
	}

	auto transform = transpose(glm::make_mat4x4(&shape->shapeToWorld.start[0][0]));
	obj->setTransform(transform);

	if (shape->material != minipbrt::kInvalidIndex)
		obj->setSharedMaterial(materials[shape->material]);

	if (shape->areaLight != minipbrt::kInvalidIndex)
	{
		auto areaLight = dynamic_cast<const minipbrt::AreaLight*>(scene->areaLights[shape->areaLight]);
		switch (areaLight->type())
		{
			case minipbrt::AreaLightType::Diffuse:
			{
				auto l = dynamic_cast<const minipbrt::DiffuseAreaLight*>(areaLight);
				obj->material()->setEmission({l->L[0], l->L[1], l->L[2]});
				break;
			}
		}
	}

	return obj;
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

				auto envMapToWorld = transpose(glm::make_mat4x4(&il->lightToWorld.start[0][0]));
				glm::mat4 toYUp = rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1, 0, 0));
				glm::mat4 rotateY = rotate(glm::mat4(1.0f), glm::radians(-0.0f), glm::vec3(0, 1, 0));
				Renderer::renderProgram()->setMatrix4X4("envMapToWorld", rotateY * toYUp * envMapToWorld);
			}
		}
	}
}
