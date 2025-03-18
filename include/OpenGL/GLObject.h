#pragma once

#include "glad.h"

class Texture;

class GLObject
{
protected:
	virtual ~GLObject() = default;

public:
	GLuint id = -1;
};


class VAO : public GLObject
{
public:
	VAO();

	void setVertices(const float* data, int size) const;
};


class GLBuffer : public GLObject
{
	int currBase = -1;

protected:
	GLBuffer();

	virtual void bind(int index) = 0;
	void setDefaultBind(int index);

	virtual void setData(const float* data, int count, GLenum type = GL_STATIC_DRAW) = 0;

public:
	void bindDefault();
};


class UBO : public GLBuffer
{
public:
	int align = -1;

	UBO(int align, int baseIndex = -1);

	void bind(int index) override;
	void setData(const float* data, int count, GLenum type = GL_STATIC_DRAW) override;
};


class SSBO : public GLBuffer
{
public:
	int align = -1;

	SSBO(int align, int baseIndex = -1);

	void bind(int index) override;
	void setData(const float* data, int count, GLenum type = GL_STATIC_DRAW) override;
	void setStorage(int count) const;

	void* mapBuffer() const;
	void unmapBuffer() const;
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

	GLFrameBuffer(int width, int height);
	~GLFrameBuffer() override;
};
