#include "GLObject.h"

#include "Material.h"
#include "Utils.h"

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
void GLBuffer::setDefaultBind(int index)
{
	currBase = index;
}
void GLBuffer::bindDefault()
{
	bind(currBase);
}

UBO::UBO(int align, int baseIndex) : align(align)
{
	glBindBuffer(GL_UNIFORM_BUFFER, id);
	if (baseIndex != -1)
	{
		glBindBufferBase(GL_UNIFORM_BUFFER, baseIndex, id);
		setDefaultBind(baseIndex);
	}
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UBO::bind(int index)
{
	glBindBufferBase(GL_UNIFORM_BUFFER, index, id);
}
void UBO::setData(const float* data, int count, GLenum type) const
{
	glBindBuffer(GL_UNIFORM_BUFFER, id);
	glBufferData(GL_UNIFORM_BUFFER, count * align * sizeof(float), data, type);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

SSBO::SSBO(int align, int baseIndex) : align(align)
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
	if (baseIndex != -1)
	{
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, baseIndex, id);
		setDefaultBind(baseIndex);
	}
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void SSBO::bind(int index)
{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, id);
}
void SSBO::setData(const float* data, int count, GLenum type) const
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
	glBufferData(GL_SHADER_STORAGE_BUFFER, count * align * sizeof(float), data, type);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}
void SSBO::setStorage(int count) const
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, count * align * sizeof(float), NULL, GL_MAP_READ_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}
void SSBO::clear(const void* data) const
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
	glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_R32I, GL_RED, GL_INT, data);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void* SSBO::mapBuffer() const
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
	auto ptr = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	return ptr;
}
void SSBO::unmapBuffer() const
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}
AtomicCounterBuffer::AtomicCounterBuffer(int baseIndex, int initValue)
{
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, id);
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, id);

	glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &initValue);

	setDefaultBind(baseIndex);
}
void AtomicCounterBuffer::bind(int index)
{
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, index, id);
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

void GLCubeMap::setFaceTexture(const unsigned char* data, int faceInd, int width, int height, GLenum typeInternal, GLenum type, GLenum dataType) const
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);

	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + faceInd, 0, typeInternal, width, height, 0, type, dataType, data);

	glBindTexture(GL_TEXTURE_2D, 0);
}

GLTexture2D::GLTexture2D(int width, int height, const unsigned char* data, GLenum type) : width(width), height(height)
{
	glBindTexture(GL_TEXTURE_2D, id);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, type, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);
}

GLTexture2DArray::GLTexture2DArray(int width, int height, int layers, GLenum type) : width(width), height(height), layers(layers)
{
	glBindTexture(GL_TEXTURE_2D_ARRAY, id);

	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, type, width, height, layers);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

int GLTexture2DArray::addTexture(const Texture* tex, GLenum type)
{
	glBindTexture(GL_TEXTURE_2D_ARRAY, id);
	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, currentFreeSpotIndex, tex->width, tex->height, 1, type, GL_UNSIGNED_BYTE, tex->data);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

	return currentFreeSpotIndex++;
}

GLFrameBuffer::GLFrameBuffer(int width, int height)
{
	glGenFramebuffers(1, &id);
	glBindFramebuffer(GL_FRAMEBUFFER, id);

	renderTexture = new GLTexture2D(width, height, nullptr, GL_RGB);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTexture->id, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		Debug::logError("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLFrameBuffer::~GLFrameBuffer()
{
	delete renderTexture;
	glDeleteFramebuffers(1, &id);
}
