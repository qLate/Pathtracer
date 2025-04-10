#include "Object.h"

#include <glm/gtx/matrix_decompose.hpp>

#include "BufferController.h"
#include "MyMath.h"
#include "Scene.h"

Object::Object(const glm::vec3 pos, glm::quat rot, glm::vec3 scale) : _initialized(true), _id(_nextAvailableId++), _pos(pos), _rot(rot), _scale(scale)
{
	Scene::objects.emplace_back(this);

	BufferController::markBufferForUpdate(BufferType::Objects);
}
Object::Object(const Object& other) : Object(other._pos, other._rot, other._scale) {}

Object::~Object()
{
	if (!_initialized) return;

	auto ind = std::ranges::find(Scene::objects, this);
	Scene::objects[ind - Scene::objects.begin()] = nullptr;
}

void Object::destroy(const Object* object)
{
	delete object;
}

void Object::setName(const std::string& name)
{
	this->_name = name;
}
void Object::setPos(glm::vec3 pos, bool notify)
{
	this->_pos = pos;

	if (notify) BufferController::markBufferForUpdate(BufferType::Objects);
}
void Object::setRot(glm::quat rot, bool notify)
{
	this->_rot = rot;

	if (notify) BufferController::markBufferForUpdate(BufferType::Objects);
}
void Object::setScale(glm::vec3 scale, bool notify)
{
	this->_scale = scale;

	if (notify) BufferController::markBufferForUpdate(BufferType::Objects);
}

glm::mat4 Object::getTransform() const
{
	return glm::translate(glm::mat4(1), _pos) * mat4_cast(_rot) * glm::scale(glm::mat4(1), _scale);
}
void Object::setTransform(const glm::mat4& transform, bool notify)
{
	glm::vec3 pos, scale;
	glm::quat rot;
	glm::vec3 skew;
	glm::vec4 perspective;
	decompose(transform, scale, rot, pos, skew, perspective);

	if (_pos != pos)
		setPos(pos, notify);
	if (_rot != rot)
		setRot(rot, notify);
	if (_scale != scale)
		setScale(scale, notify);
}

void Object::translate(const glm::vec3& v)
{
	setPos(_pos + v);
}
void Object::rotate(const glm::vec3& degrees)
{
	setRot(glm::rotate(_rot, glm::radians(degrees.x), vec3::FORWARD));
}

glm::vec3 Object::forward() const { return _rot * (vec3::FORWARD * sign(_scale)); }
glm::vec3 Object::backward() const { return _rot * (vec3::BACKWARD * sign(_scale)); }
glm::vec3 Object::up() const { return _rot * (vec3::UP * sign(_scale)); }
glm::vec3 Object::down() const { return _rot * (vec3::DOWN * sign(_scale)); }
glm::vec3 Object::left() const { return _rot * (vec3::LEFT * sign(_scale)); }
glm::vec3 Object::right() const { return _rot * (vec3::RIGHT * sign(_scale)); }

glm::vec3 Object::localToGlobalPos(const glm::vec3& localPos) const
{
	return _pos + _rot * (localPos * _scale);
}
glm::vec3 Object::globalToLocalPos(const glm::vec3& globalPos) const
{
	return inverse(_rot) * (globalPos - _pos) / _scale;
}
