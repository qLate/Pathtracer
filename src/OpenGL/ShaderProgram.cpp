#include "ShaderProgram.h"

ComputeShaderProgram::ComputeShaderProgram(const char* path) : BaseShaderMethods(glCreateProgram())
{
	_computeShader = new Shader(path, _id, GL_COMPUTE_SHADER);
	glLinkProgram(_id);
	checkCompileErrors(_id, "PROGRAM");
}
ComputeShaderProgram::~ComputeShaderProgram()
{
	delete _computeShader;
	glDeleteProgram(_id);
}

void ComputeShaderProgram::use() const
{
	glUseProgram(_id);
}
void ComputeShaderProgram::dispatch(glm::ivec3 numGroups, GLenum sync)
{
	glDispatchCompute(numGroups.x, numGroups.y, numGroups.z);
	if (sync != -1)
		glMemoryBarrier(sync);
}
