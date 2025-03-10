#pragma once

#include <glm/gtc/quaternion.hpp>
#include <glm/vec3.hpp>

class Object
{
protected:
	glm::vec3 pos;
	glm::quat rot;

	virtual ~Object() = default;
	Object(glm::vec3 pos, glm::quat rot = {1, 0, 0, 0});

public:
	glm::vec3 getPos() const { return pos; }
	glm::quat getRot() const { return rot; }
	glm::vec4 getRotVec4() const { return {rot.x, rot.y, rot.z, rot.w}; }

	virtual void setPos(glm::vec3 pos);
	virtual void setRot(glm::quat rot);

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
};
