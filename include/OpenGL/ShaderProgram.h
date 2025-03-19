#pragma once

#include "glad.h"
#include "Shader.h"

class ComputeShaderProgram : public BaseShaderMethods
{
	Shader* _computeShader;

public:
	ComputeShaderProgram(const char* path);
	~ComputeShaderProgram();

	Shader* computeShader() const { return _computeShader; }

	void use() const;
	static void dispatch(glm::ivec3 numGroups, GLenum sync = -1);
};

template <typename FragType = Shader>
class DefaultShaderProgram : public BaseShaderMethods
{
	Shader* _vertShader;
	FragType* _fragShader;

public:
	DefaultShaderProgram(const char* vertPath, const char* fragPath);
	~DefaultShaderProgram();

	Shader* vertShader() const { return _vertShader; }
	FragType* fragShader() const { return _fragShader; }

	void addShader(const char* path, int type) const;
	void use() const;
};

template <typename FragType>
DefaultShaderProgram<FragType>::DefaultShaderProgram(const char* vertPath, const char* fragPath) : BaseShaderMethods(glCreateProgram())
{
	_vertShader = new Shader(vertPath, id, GL_VERTEX_SHADER);
	_fragShader = new FragType(fragPath, id, GL_FRAGMENT_SHADER);

	glLinkProgram(id);
	checkCompileErrors(id, "PROGRAM");
}

template <typename FragType>
DefaultShaderProgram<FragType>::~DefaultShaderProgram()
{
	delete _vertShader;
	delete _fragShader;
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
