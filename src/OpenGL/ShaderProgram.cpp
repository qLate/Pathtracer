#include "ShaderProgram.h"

ComputeShaderProgram::ComputeShaderProgram(const char* path) : BaseShaderMethods(glCreateProgram())
{
	computeShader = new Shader(path, id, GL_COMPUTE_SHADER);
	glLinkProgram(id);
	checkCompileErrors(id, "PROGRAM");
}
ComputeShaderProgram::~ComputeShaderProgram()
{
	delete computeShader;
	glDeleteProgram(id);
}

void ComputeShaderProgram::use() const
{
	glUseProgram(id);
}
void ComputeShaderProgram::dispatch(glm::ivec3 numGroups)
{
	glDispatchCompute(numGroups.x, numGroups.y, numGroups.z);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
}
