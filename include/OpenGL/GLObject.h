#pragma once

#include <vector>

#include "glad.h"
#include "glm/vec2.hpp"

class Texture;

class GLObject
{
protected:
	GLuint _id = -1;

	virtual ~GLObject() = default;

public:
	GLuint id() const { return _id; }
};


class VAO : public GLObject
{
public:
	VAO();

	void setVertices(const float* data, int size) const;
};


class GLBuffer : public GLObject
{
	int _currBase = -1;

protected:
	GLBuffer();

	virtual void bind(int index) = 0;
	void setDefaultBind(int index);

public:
	void bindDefault();
};


class GLBufferObject : public GLBuffer
{
	GLenum _type;
	int _align = -1;
	int _capacity = -1;

protected:
	GLBufferObject(GLenum type, int align, int baseIndex = -1);

public:
	void bind(int index) override;
	void setData(const float* data, int count, GLenum type = GL_STATIC_DRAW);
	void setSubData(const float* data, int count, int offset = 0) const;
	void setStorage(int count, GLenum flags = NULL, const void* data = nullptr);

	void setDataCapacity(int capacity, GLenum type = GL_STATIC_DRAW);
	void ensureDataCapacity(int capacity, GLenum type = GL_STATIC_DRAW);

	void clear(const void* data = nullptr) const;

	template <typename T>
	std::vector<T> readData(int count) const;
};


class UBO : public GLBufferObject
{
public:
	UBO(int align, int baseIndex = -1);
};


class SSBO : public GLBufferObject
{
public:
	SSBO(int align, int baseIndex = -1);
};


class AtomicCounterBuffer : public GLBuffer
{
public:
	AtomicCounterBuffer(int baseIndex = -1, int initValue = 0);

	void bind(int index) override;
};


class GLTexture : public GLObject
{
public:
	GLTexture();
	~GLTexture() override;
};


class GLCubeMap : public GLTexture
{
public:
	GLCubeMap();

	void setFaceTexture(const unsigned char* data, int faceInd, int width, int height, GLenum typeInternal = GL_RGB, GLenum type = GL_RGB, GLenum dataType = GL_RGB) const;
};


class GLTexture2D : public GLTexture
{
	int _width, _height;

public:
	GLTexture2D(int width, int height, const unsigned char* data = nullptr, GLenum type = GL_RGBA, GLint internalFormat = GL_RGBA);

	uint64_t getHandle() const;
};


class GLTexture2DArray : public GLTexture
{
	int _width, _height, _layers;
	int _currentFreeSpotIndex = 0;

public:
	GLTexture2DArray(int width, int height, int layers, GLenum type = GL_RGBA8);

	int addTexture(const Texture* tex, GLenum type = GL_RGBA);
};


class GLFrameBuffer : public GLObject
{
public:
	GLTexture2D* renderTexture = nullptr;

	GLFrameBuffer(glm::ivec2 size);
	~GLFrameBuffer() override;
};


template <typename T>
std::vector<T> GLBufferObject::readData(int count) const
{
	glBindBuffer(_type, _id);

	auto ptr = glMapBuffer(_type, GL_READ_ONLY);
	auto data = std::vector<T>(count);
	memcpy(data.data(), ptr, count * _align * sizeof(float));
	glUnmapBuffer(_type);

	glBindBuffer(_type, 0);
	return data;
}