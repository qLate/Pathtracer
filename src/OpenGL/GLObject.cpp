#include "GLObject.h"

#include <iostream>

#include "Material.h"

VAO::VAO()
{
	glGenVertexArrays(1, &id);
}

void VAO::setVertices(const float* data, int size) const
{
	glBindVertexArray(id);

	unsigned int vbo;
	glGenBuffers(1, &vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, size * sizeof(float), data, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)nullptr);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

GLBuffer::GLBuffer()
{
	glGenBuffers(1, &id);
}

UBO::UBO(int align, int baseIndex) : align(align)
{
	glBindBuffer(GL_UNIFORM_BUFFER, id);
	if (baseIndex != -1)
		glBindBufferBase(GL_UNIFORM_BUFFER, baseIndex, id);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UBO::bindBase(int index)
{
	glBindBufferBase(GL_UNIFORM_BUFFER, index, id);
}
void UBO::setData(const float* data, int count)
{
	glBindBuffer(GL_UNIFORM_BUFFER, id);
	glBufferData(GL_UNIFORM_BUFFER, count * align * sizeof(float), data, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

SSBO::SSBO(int align, int baseIndex) : align(align)
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
	if (baseIndex != -1)
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, baseIndex, id);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void SSBO::bindBase(int index)
{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, id);
}
void SSBO::setData(const float* data, int count)
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
	glBufferData(GL_SHADER_STORAGE_BUFFER, count * align * sizeof(float), data, GL_STATIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

GLTexture::GLTexture()
{
	glGenTextures(1, &id);
}
GLTexture::~GLTexture()
{
	glDeleteTextures(1, &id);
}

GLCubeMap::GLCubeMap()
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void GLCubeMap::setFaceTexture(const unsigned char* data, int faceInd, int width, int height) const
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);

	glTexImage2D(
		GL_TEXTURE_CUBE_MAP_POSITIVE_X + faceInd,
		0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
	);

	glBindTexture(GL_TEXTURE_2D, 0);
}

GLTexture2D::GLTexture2D(const Texture* texture) : width(texture->width), height(texture->height)
{
	glBindTexture(GL_TEXTURE_2D, id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	auto* data = new unsigned char[texture->width * texture->height * 4];
	for (int i = 0; i < texture->width * texture->height; ++i)
	{
		data[i * 4 + 0] = texture->pixelColors[i].x * 255.0f;
		data[i * 4 + 1] = texture->pixelColors[i].y * 255.0f;
		data[i * 4 + 2] = texture->pixelColors[i].z * 255.0f;
		data[i * 4 + 3] = texture->pixelColors[i].w * 255.0f;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->width, texture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
}
GLTexture2D::GLTexture2D(int width, int height) : width(width), height(height)
{
	glBindTexture(GL_TEXTURE_2D, id);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void GLTexture2D::resize(int width, int height)
{
	if (width == this->width && height == this->height) return;
	this->width = width;
	this->height = height;

	glBindTexture(GL_TEXTURE_2D, id);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);

}

GLFrameBuffer::GLFrameBuffer(int width, int height)
{
	glGenFramebuffers(1, &id);
	glBindFramebuffer(GL_FRAMEBUFFER, id);

	renderTexture = new GLTexture2D(width, height);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTexture->id, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << '\n';
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

GLFrameBuffer::~GLFrameBuffer()
{
	delete renderTexture;
	glDeleteFramebuffers(1, &id);
}

void GLFrameBuffer::resize(int width, int height) const
{
	glBindFramebuffer(GL_FRAMEBUFFER, id);

	renderTexture->resize(width, height);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTexture->id, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete after resize!" << '\n';

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
