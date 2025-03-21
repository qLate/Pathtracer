#include "Object.h"

#include <glm/gtx/matrix_decompose.hpp>

#include "BufferController.h"
#include "MyMath.h"
#include "Scene.h"

Object::Object(const glm::vec3 pos, glm::quat rot, glm::vec3 scale) : _pos(pos), _rot(rot), _scale(scale)
{
	Scene::objects.emplace_back(this);
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

	if (_pos != pos || _rot != rot || _scale != scale)
	{
		_pos = pos;
		_rot = rot;
		_scale = scale;

		BufferController::markBufferForUpdate(BufferType::Objects);
	}
}

void Object::translate(const glm::vec3& v)
{
	setPos(_pos + v);
}
void Object::rotate(const glm::vec3& degrees)
{
	setRot(glm::quat(eulerAngles(_rot) + degrees * DEG_TO_RAD));
}

glm::vec3 Object::forward() const { return _rot * glm::vec3(0, 1, 0); }
glm::vec3 Object::backward() const { return _rot * glm::vec3(0, -1, 0); }
glm::vec3 Object::up() const { return _rot * glm::vec3(0, 0, 1); }
glm::vec3 Object::down() const { return _rot * glm::vec3(0, 0, -1); }
glm::vec3 Object::left() const { return _rot * glm::vec3(-1, 0, 0); }
glm::vec3 Object::right() const { return _rot * glm::vec3(1, 0, 0); }

glm::vec3 Object::localToGlobalPos(const glm::vec3& localPos) const
{
	return _pos + _rot * (localPos * _scale);
}
glm::vec3 Object::globalToLocalPos(const glm::vec3& globalPos) const
{
	return inverse(_rot) * (globalPos - _pos) / _scale;
}
