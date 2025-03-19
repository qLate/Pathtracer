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


class UBO : public GLBuffer
{
	int _align = -1;

public:
	UBO(int align, int baseIndex = -1);

	void bind(int index) override;
	void setData(const float* data, int count, GLenum type = GL_STATIC_DRAW) const;
};


class SSBO : public GLBuffer
{
	int _align = -1;

public:
	SSBO(int align, int baseIndex = -1);

	void bind(int index) override;
	void setData(const float* data, int count, GLenum type = GL_STATIC_DRAW) const;
	void setStorage(int count) const;

	void clear(const void* data = nullptr) const;

	void* mapBuffer() const;
	void unmapBuffer() const;

	template <typename T>
	std::vector<T> readData(int count) const;
};

template <typename T>
std::vector<T> SSBO::readData(int count) const
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, _id);

	auto ptr = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
	auto data = std::vector<T>(count);
	memcpy(data.data(), ptr, count * _align * sizeof(float));
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	return data;
}

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
public:
	int width, height;

	GLTexture2D(int width, int height, const unsigned char* data = nullptr, GLenum type = GL_RGBA);
};


class GLTexture2DArray : public GLTexture
{
	int currentFreeSpotIndex = 0;

public:
	int width, height, layers;

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
