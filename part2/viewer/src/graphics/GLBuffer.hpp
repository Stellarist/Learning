#pragma once

#include <glad/glad.h>

#include "components/SubMesh.hpp"

template <typename T,
          unsigned int Target = GL_ARRAY_BUFFER,
          unsigned int Usage = GL_STATIC_DRAW>
struct GLBuffer {
	unsigned int id;
	unsigned int count;

	GLBuffer();
	GLBuffer(const T* data, unsigned int count);

	GLBuffer(const GLBuffer& other) = delete;
	GLBuffer& operator=(const GLBuffer& other) = delete;

	GLBuffer(GLBuffer&& other) noexcept;
	GLBuffer& operator=(GLBuffer&& other) noexcept;

	~GLBuffer();

	unsigned int getId();

	void        bind() const;
	static void unbind();

	void update(const T* data, unsigned int new_count);
	void clear();
};

using VertexBuffer = GLBuffer<float, GL_ARRAY_BUFFER, GL_STATIC_DRAW>;
using IndexBuffer = GLBuffer<unsigned int, GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW>;

class VertexArray {
public:
	unsigned int id;
	unsigned int stride;

	std::vector<VertexAttribute> attributes;

	VertexArray();

	VertexArray(const VertexArray& other) = delete;
	VertexArray& operator=(const VertexArray& other) = delete;

	VertexArray(VertexArray&& other) noexcept;
	VertexArray& operator=(VertexArray&& other) noexcept;

	~VertexArray();

	void        bind() const;
	static void unbind();

	void pushAttribute(unsigned int count);
	void pushAttribute(VertexAttribute&& attribute);

	void addBuffer(const VertexBuffer& vbo);
};

template <typename T, unsigned int Target, unsigned int Usage>
GLBuffer<T, Target, Usage>::GLBuffer() :
    id(0), count(0)
{
	glGenBuffers(1, &id);
}

template <typename T, unsigned int Target, unsigned int Usage>
GLBuffer<T, Target, Usage>::GLBuffer(const T* data, unsigned int count) :
    id(0), count(count)
{
	glGenBuffers(1, &id);
	glBindBuffer(Target, id);
	glBufferData(Target, count * sizeof(T), data, Usage);
}

template <typename T, unsigned int Target, unsigned int Usage>
GLBuffer<T, Target, Usage>::GLBuffer(GLBuffer&& other) noexcept :
    id(other.id), count(other.count)
{
	other.id = 0;
	other.count = 0;
}

template <typename T, unsigned int Target, unsigned int Usage>
auto GLBuffer<T, Target, Usage>::operator=(GLBuffer&& other) noexcept -> GLBuffer&
{
	if (this != &other) {
		if (id)
			glDeleteBuffers(1, &id);

		id = other.id;
		count = other.count;
		other.id = 0;
		other.count = 0;
	}

	return *this;
}

template <typename T, unsigned int Target, unsigned int Usage>
GLBuffer<T, Target, Usage>::~GLBuffer()
{
	if (id) {
		glDeleteBuffers(1, &id);
		id = 0;
	}
}

template <typename T, unsigned int Target, unsigned int Usage>
unsigned int GLBuffer<T, Target, Usage>::getId()
{
	return id;
}

template <typename T, unsigned int Target, unsigned int Usage>
void GLBuffer<T, Target, Usage>::bind() const
{
	glBindBuffer(Target, id);
}

template <typename T, unsigned int Target, unsigned int Usage>
void GLBuffer<T, Target, Usage>::unbind()
{
	glBindBuffer(Target, 0);
}

template <typename T, unsigned int Target, unsigned int Usage>
void GLBuffer<T, Target, Usage>::update(const T* data, unsigned int new_count)
{
	bind();
	glBufferData(Target, new_count * sizeof(T), data, Usage);
	count = new_count;
}

template <typename T, unsigned int Target, unsigned int Usage>
void GLBuffer<T, Target, Usage>::clear()
{
	bind();
	glBufferData(Target, 0, nullptr, Usage);
	count = 0;
}
