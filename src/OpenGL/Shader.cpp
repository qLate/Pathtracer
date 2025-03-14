#include "Shader.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>

#include "GLObject.h"
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
	glUseProgram(id);
	glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
}
void BaseShaderMethods::setInt(const std::string& name, int value) const
{
	glUseProgram(id);
	glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}
void BaseShaderMethods::setFloat(const std::string& name, float value) const
{
	glUseProgram(id);
	glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}
void BaseShaderMethods::setFloat2(const std::string& name, glm::vec2 value) const
{
	glUseProgram(id);
	glUniform2f(glGetUniformLocation(id, name.c_str()), value.x, value.y);
}
void BaseShaderMethods::setFloat3(const std::string& name, glm::vec3 value) const
{
	glUseProgram(id);
	glUniform3f(glGetUniformLocation(id, name.c_str()), value.x, value.y, value.z);
}
void BaseShaderMethods::setFloat4(const std::string& name, glm::vec4 value) const
{
	glUseProgram(id);
	glUniform4f(glGetUniformLocation(id, name.c_str()), value.x, value.y, value.z, value.w);
}
void BaseShaderMethods::setMatrix4X4(const std::string& name, glm::mat<4, 4, float> mat) const
{
	glUseProgram(id);
	glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, false, value_ptr(mat));
}

bool BaseShaderMethods::getBool(const std::string& name) const
{
	glUseProgram(id);
	int val;
	glGetUniformiv(id, glGetUniformLocation(id, name.c_str()), &val);
	return val;
}
int BaseShaderMethods::getInt(const std::string& name) const
{
	glUseProgram(id);
	int value;
	glGetUniformiv(id, glGetUniformLocation(id, name.c_str()), &value);
	return value;
}
float BaseShaderMethods::getFloat(const std::string& name) const
{
	glUseProgram(id);
	float value;
	glGetUniformfv(id, glGetUniformLocation(id, name.c_str()), &value);
	return value;
}
glm::vec2 BaseShaderMethods::getFloat2(const std::string& name) const
{
	glUseProgram(id);
	glm::vec2 value;
	glGetUniformfv(id, glGetUniformLocation(id, name.c_str()), value_ptr(value));
	return value;
}
glm::vec3 BaseShaderMethods::getFloat3(const std::string& name) const
{
	glUseProgram(id);
	glm::vec3 value;
	glGetUniformfv(id, glGetUniformLocation(id, name.c_str()), value_ptr(value));
	return value;
}
glm::vec4 BaseShaderMethods::getFloat4(const std::string& name) const
{
	glUseProgram(id);
	glm::vec4 value;
	glGetUniformfv(id, glGetUniformLocation(id, name.c_str()), value_ptr(value));
	return value;
}
glm::mat<4, 4, float> BaseShaderMethods::getMatrix4X4(const std::string& name) const
{
	glUseProgram(id);
	glm::mat<4, 4, float> value;
	glGetUniformfv(id, glGetUniformLocation(id, name.c_str()), value_ptr(value));
	return value;
}

Shader::Shader(const std::string& path, int id, int type) : BaseShaderMethods(id)
{
	std::string code = parseShader(path);
	const char* shaderCode = code.c_str();

	auto shader = glCreateShader(type);
	glShaderSource(shader, 1, &shaderCode, nullptr);
	glCompileShader(shader);
	checkCompileErrors(shader, "SHADER");

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

	std::regex re = std::regex("/\\*buffer\\*/ uniform");
	while (std::regex_search(code, re))
		code = std::regex_replace(code, re, "buffer");

	writeOutShader(code, pathStr);

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

void Shader::writeOutShader(const std::string& code, const std::string& path)
{
	std::filesystem::path p(path);

	std::ofstream file;
	file.open("shaders/out/out_" + p.filename().string());
	file << code;
	file.close();
}
