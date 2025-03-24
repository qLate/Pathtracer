#pragma once

#include <string>

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

class GLTexture2D;
class Texture;

class BaseShaderMethods
{
protected:
	static void checkCompileErrors(unsigned int shader, const std::string& type, const std::string& path = "");

public:
	unsigned int _id = -1;

	BaseShaderMethods(int id);

	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setFloat2(const std::string& name, glm::vec2 value) const;
	void setFloat3(const std::string& name, glm::vec3 value) const;
	void setFloat4(const std::string& name, glm::vec4 value) const;
	void setMatrix4X4(const std::string& name, glm::mat<4, 4, float> mat) const;

	bool getBool(const std::string& name) const;
	int getInt(const std::string& name) const;
	float getFloat(const std::string& name) const;
	glm::vec2 getFloat2(const std::string& name) const;
	glm::vec3 getFloat3(const std::string& name) const;
	glm::vec4 getFloat4(const std::string& name) const;
	glm::mat<4, 4, float> getMatrix4X4(const std::string& name) const;
};


class Shader : public BaseShaderMethods
{
	static std::string parseShader(const std::string& pathStr);
	static std::string readShaderFile(const std::string& path);
	static void writeOutShader(const std::string& code, const std::string& path);

public:
	Shader(const std::string& path, int id, int type);

	static void addInclude(const std::string& path);
};
