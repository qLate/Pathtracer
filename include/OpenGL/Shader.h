#pragma once

#include <string>
#include <vector>

#include "glad.h"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

class GLTexture2D;
class Texture;

class BaseShaderMethods
{
protected:
	static void checkCompileErrors(unsigned int shader, const std::string& type);

public:
	unsigned int id = -1;

	BaseShaderMethods(int id);

	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setFloat2(const std::string& name, glm::vec2 value) const;
	void setFloat3(const std::string& name, glm::vec3 value) const;
	void setFloat4(const std::string& name, glm::vec4 value) const;
	void setMatrix4X4(const std::string& name, glm::mat<4, 4, float> mat) const;
};

class Shader : public BaseShaderMethods
{
	static std::string parseShader(const std::string& pathStr);
	static std::string readShaderFile(const std::string& path);

public:
	Shader(const std::string& path, int id, int type);
};

template <typename FragType = Shader>
class ShaderProgram : public BaseShaderMethods
{
public:
	Shader* vertShader;
	FragType* fragShader;

	ShaderProgram(const char* vertPath, const char* fragPath);
	~ShaderProgram();

	void addShader(const char* path, int type) const;
	void use() const;
};

template <typename FragType>
ShaderProgram<FragType>::ShaderProgram(const char* vertPath, const char* fragPath) : BaseShaderMethods(glCreateProgram())
{
	vertShader = new Shader(vertPath, id, GL_VERTEX_SHADER);
	fragShader = new FragType(fragPath, id, GL_FRAGMENT_SHADER);

	glLinkProgram(id);
	checkCompileErrors(id, "PROGRAM");
}

template <typename FragType>
ShaderProgram<FragType>::~ShaderProgram()
{
	delete vertShader;
	delete fragShader;
	glDeleteProgram(id);
}

template <typename FragType>
void ShaderProgram<FragType>::addShader(const char* path, int type) const
{
	Shader(path, id, type);
}

template <typename FragType>
void ShaderProgram<FragType>::use() const
{
	glUseProgram(id);
}
