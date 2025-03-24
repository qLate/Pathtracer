#pragma once

#include <glm/gtc/quaternion.hpp>
#include <glm/vec3.hpp>

#include "InspectorDrawer.h"
#include "JsonUtility.h"

class Object
{
	inline static int _nextAvailableId = 0;

	bool _initialized = false;
	int _id;

protected:
	glm::vec3 _pos;
	glm::quat _rot;
	glm::vec3 _scale;

	Object() = default;

public:
	Object(glm::vec3 pos, glm::quat rot = {}, glm::vec3 scale = {1, 1, 1});
	Object(const Object& other);
	virtual ~Object();

	static void destroy(const Object* object);
	static Object* clone(const Object* object) { return object->clone_internal(); }

	bool initialized() const { return _initialized; }
	int id() const { return _id; }
	glm::vec3 pos() const { return _pos; }
	glm::quat rot() const { return _rot; }
	glm::vec3 scale() const { return _scale; }

	virtual void setPos(glm::vec3 pos, bool notify = true);
	virtual void setRot(glm::quat rot, bool notify = true);
	virtual void setScale(glm::vec3 scale, bool notify = true);

	glm::mat4 getTransform() const;
	void setTransform(const glm::mat4& transform, bool notify = true);

	void translate(const glm::vec3& v);
	void rotate(const glm::vec3& degrees);

	glm::vec3 forward() const;
	glm::vec3 backward() const;
	glm::vec3 up() const;
	glm::vec3 down() const;
	glm::vec3 left() const;
	glm::vec3 right() const;

	glm::vec3 localToGlobalPos(const glm::vec3& localPos) const;
	glm::vec3 globalToLocalPos(const glm::vec3& globalPos) const;

	constexpr static auto properties();

private:
	virtual Object* clone_internal() const { return new Object(*this); }
	virtual void drawInspector() { return ObjectInspectorDrawer::draw(this); }

	friend class WindowDrawer;
};

constexpr auto Object::properties()
{
	return std::make_tuple(
		JsonUtility::property(&Object::_pos, "pos"),
		JsonUtility::property(&Object::_rot, "rot"),
		JsonUtility::property(&Object::_scale, "scale")
	);
}
