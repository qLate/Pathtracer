#include "ShaderProgram.h"

ComputeShaderProgram::ComputeShaderProgram(const char* path) : BaseShaderMethods(glCreateProgram())
{
	_computeShader = new Shader(path, id, GL_COMPUTE_SHADER);
	glLinkProgram(id);
	checkCompileErrors(id, "PROGRAM");
}
ComputeShaderProgram::~ComputeShaderProgram()
{
	delete _computeShader;
	glDeleteProgram(id);
}

void ComputeShaderProgram::use() const
{
	glUseProgram(id);
}
void ComputeShaderProgram::dispatch(glm::ivec3 numGroups, GLenum sync)
{
	glDispatchCompute(numGroups.x, numGroups.y, numGroups.z);
	if (sync != -1)
		glMemoryBarrier(sync);
}
