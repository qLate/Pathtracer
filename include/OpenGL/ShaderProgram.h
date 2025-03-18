#pragma once

#include "glad.h"
#include "Shader.h"

class ComputeShaderProgram : public BaseShaderMethods
{
public:
	Shader* computeShader;

	ComputeShaderProgram(const char* path);
	~ComputeShaderProgram();

	void use() const;
	static void dispatch(glm::ivec3 numGroups, GLenum sync = -1);
};

template <typename FragType = Shader>
class DefaultShaderProgram : public BaseShaderMethods
{
public:
	Shader* vertShader;
	FragType* fragShader;

	DefaultShaderProgram(const char* vertPath, const char* fragPath);
	~DefaultShaderProgram();

	void addShader(const char* path, int type) const;
	void use() const;
};

template <typename FragType>
DefaultShaderProgram<FragType>::DefaultShaderProgram(const char* vertPath, const char* fragPath) : BaseShaderMethods(glCreateProgram())
{
	vertShader = new Shader(vertPath, id, GL_VERTEX_SHADER);
	fragShader = new FragType(fragPath, id, GL_FRAGMENT_SHADER);

	glLinkProgram(id);
	checkCompileErrors(id, "PROGRAM");
}

template <typename FragType>
DefaultShaderProgram<FragType>::~DefaultShaderProgram()
{
	delete vertShader;
	delete fragShader;
	glDeleteProgram(id);
}

template <typename FragType>
void DefaultShaderProgram<FragType>::addShader(const char* path, int type) const
{
	Shader(path, id, type);
}

template <typename FragType>
void DefaultShaderProgram<FragType>::use() const
{
	glUseProgram(id);
}
