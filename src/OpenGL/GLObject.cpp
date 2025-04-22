#include "GLObject.h"

#include "Material.h"
#include "Utils.h"

VAO::VAO()
{
	glGenVertexArrays(1, &_id);
}

void VAO::setVertices(const float* data, int size) const
{
	glBindVertexArray(_id);

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
	glGenBuffers(1, &_id);
}
void GLBuffer::setDefaultBind(int index)
{
	_currBase = index;
}
void GLBuffer::bindDefault()
{
	bind(_currBase);
}

GLBufferObject::GLBufferObject(GLenum type, int align, int baseIndex) : _type(type), _align(align)
{
	glBindBuffer(_type, _id);
	if (baseIndex != -1)
	{
		glBindBufferBase(_type, baseIndex, _id);
		setDefaultBind(baseIndex);
	}
	glBindBuffer(_type, 0);
}
void GLBufferObject::bind(int index)
{
	glBindBufferBase(_type, index, _id);
}
void GLBufferObject::setData(const float* data, int count, GLenum type)
{
	_capacity = count;

	glBindBuffer(_type, _id);
	glBufferData(_type, count * _align * sizeof(float), data, type);
	glBindBuffer(_type, 0);
}
void GLBufferObject::setSubData(const float* data, int count, int offset) const
{
	glBindBuffer(_type, _id);
	glBufferSubData(_type, offset * _align * sizeof(float), count * _align * sizeof(float), data);
	glBindBuffer(_type, 0);
}
void GLBufferObject::setStorage(int count, GLenum flags, const void* data)
{
	if (_capacity != -1) throw std::runtime_error("Cannot set storage again on a fixed size storage buffer.");
	_capacity = count;

	glBindBuffer(_type, _id);
	glBufferStorage(_type, count * _align * sizeof(float), data, flags);
	glBindBuffer(_type, 0);
}
void GLBufferObject::setDataCapacity(int capacity, GLenum type)
{
	_capacity = capacity;

	glBindBuffer(_type, _id);
	glBufferData(_type, capacity * _align * sizeof(float), NULL, type);
	glBindBuffer(_type, 0);
}
void GLBufferObject::ensureDataCapacity(int capacity, GLenum type)
{
	static constexpr int CAPACITY_MULT = 2;
	if (_capacity >= capacity) return;
	setDataCapacity(capacity * CAPACITY_MULT, type);
}

void GLBufferObject::clear(const void* data) const
{
	glBindBuffer(_type, _id);
	glClearBufferData(_type, GL_R32I, GL_RED, GL_INT, data);
	glBindBuffer(_type, 0);
}

UBO::UBO(int align, int baseIndex) : GLBufferObject(GL_UNIFORM_BUFFER, align, baseIndex) {}

SSBO::SSBO(int align, int baseIndex) : GLBufferObject(GL_SHADER_STORAGE_BUFFER, align, baseIndex) {}

AtomicCounterBuffer::AtomicCounterBuffer(int baseIndex, int initValue)
{
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, _id);
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, _id);

	glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &initValue);

	setDefaultBind(baseIndex);
}
void AtomicCounterBuffer::bind(int index)
{
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, index, _id);
}

GLTexture::GLTexture()
{
	glGenTextures(1, &_id);
}
GLTexture::~GLTexture()
{
	glDeleteTextures(1, &_id);
}

GLTexture2D::GLTexture2D(int width, int height, const void* data, GLenum format, GLenum internalFormat, GLenum filter, GLenum type) : _width(width), _height(height),
	_format(format)
{
	glBindTexture(GL_TEXTURE_2D, _id);

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

	glBindTexture(GL_TEXTURE_2D, 0);
}
uint64_t GLTexture2D::getHandle() const
{
	GLuint64 handle = glGetTextureHandleARB(_id);
	if (!glIsTextureHandleResidentARB(handle))
		glMakeTextureHandleResidentARB(handle);
	return handle;
}
void GLTexture2D::setWrapMode(GLint wrapMode) const
{
	glBindTexture(GL_TEXTURE_2D, _id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
	glBindTexture(GL_TEXTURE_2D, 0);
}

GLFrameBuffer::GLFrameBuffer(glm::ivec2 size)
{
	glGenFramebuffers(1, &_id);
	glBindFramebuffer(GL_FRAMEBUFFER, _id);

	_renderTexture = new GLTexture2D(size.x, size.y, nullptr, GL_RGB, GL_RGB);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _renderTexture->id(), 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		Debug::logError("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLFrameBuffer::~GLFrameBuffer()
{
	delete _renderTexture;
	glDeleteFramebuffers(1, &_id);
}
