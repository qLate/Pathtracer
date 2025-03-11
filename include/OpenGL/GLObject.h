#pragma once

#include <vector>

#include "Color.h"
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
protected:
	GLBuffer();

	virtual void bindBase(int index) = 0;
	virtual void setData(const float* data, int count) = 0;
};


class UBO : public GLBuffer
{
public:
	int align = -1;

	UBO(int align, int baseIndex = -1);

	void bindBase(int index) override;
	void setData(const float* data, int count) override;
};


class SSBO : public GLBuffer
{
public:
	int align = -1;

	SSBO(int align, int baseIndex = -1);

	void bindBase(int index) override;
	void setData(const float* data, int count) override;
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

	void setFaceTexture(const unsigned char* data, int faceInd, int width, int height) const;
};


class GLTexture2D : public GLTexture
{
public:
	int width, height;

	GLTexture2D(int width, int height, const unsigned char* data = nullptr);
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
