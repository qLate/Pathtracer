#include "Shader.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

#include "GLObject.h"
#include "glm/mat3x3.hpp"
#include "glm/gtc/type_ptr.hpp"

BaseShaderMethods::BaseShaderMethods(int id): id(id) {}

void BaseShaderMethods::checkCompileErrors(unsigned shader, const std::string& type)
{
	int success;
	char infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog <<
				"\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog <<
				"\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
}

void BaseShaderMethods::setBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
}
void BaseShaderMethods::setInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}
void BaseShaderMethods::setFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}
void BaseShaderMethods::setFloat2(const std::string& name, glm::vec2 value) const
{
	glUniform2f(glGetUniformLocation(id, name.c_str()), value.x, value.y);
}
void BaseShaderMethods::setFloat3(const std::string& name, glm::vec3 value) const
{
	glUniform3f(glGetUniformLocation(id, name.c_str()), value.x, value.y, value.z);
}
void BaseShaderMethods::setFloat4(const std::string& name, glm::vec4 value) const
{
	glUniform4f(glGetUniformLocation(id, name.c_str()), value.x, value.y, value.z, value.w);
}
void BaseShaderMethods::setMatrix4X4(const std::string& name, glm::mat<4, 4, float> mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, false, value_ptr(mat));
}

void Shader::addTexture2D(const Texture* texture)
{
	textures.push_back(new GLTexture2D(texture));
}

Shader::Shader(const std::string& path, int id, int type) : BaseShaderMethods(id)
{
	std::string code = parseShader(path);
	const char* shaderCode = code.c_str();

	auto shader = glCreateShader(type);
	glShaderSource(shader, 1, &shaderCode, nullptr);
	glCompileShader(shader);
	checkCompileErrors(shader, "FRAGMENT");

	glAttachShader(id, shader);

	glDeleteShader(shader);
}

constexpr char SHADERS_DIR[] = "shaders/";
std::string Shader::parseShader(const std::string& pathStr)
{
	auto code = readShaderFile(pathStr);

	// Find and replace #include directives
	for (size_t pos = code.find("/// #include"); pos != std::string::npos; pos = code.find("/// #include", pos))
	{
		auto start = code.find('"', pos) + 1;
		auto end = code.find('"', start);

		auto relIncludePath = code.substr(start, end - start);
		auto includeCode = readShaderFile(SHADERS_DIR + relIncludePath) + "\n";
		code.replace(pos, end - pos + 1, includeCode);
	}

	return code;
}
std::string Shader::readShaderFile(const std::string& path)
{
	std::ifstream file;
	std::string text;

	file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		file.open(path);

		std::stringstream stream;
		stream << file.rdbuf();
		file.close();

		text = stream.str();
	}
	catch (std::ifstream::failure& e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
	}

	return text;
}
