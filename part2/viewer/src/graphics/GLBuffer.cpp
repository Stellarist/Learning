#include "GLBuffer.hpp"

VertexArray::VertexArray() :
    id(0), stride(0)
{
	glGenVertexArrays(1, &id);
}

VertexArray::VertexArray(VertexArray&& other) noexcept :
    id(other.id), stride(other.stride), attributes(std::move(other.attributes))
{
	other.id = 0;
	other.stride = 0;
}

VertexArray& VertexArray::operator=(VertexArray&& other) noexcept
{
	if (this != &other) {
		if (id)
			glDeleteVertexArrays(1, &id);

		id = other.id;
		stride = other.stride;
		attributes = std::move(other.attributes);

		other.id = 0;
		other.stride = 0;
	}

	return *this;
}

VertexArray::~VertexArray()
{
	if (id) {
		glDeleteVertexArrays(1, &id);
		id = 0;
	}
}

void VertexArray::bind() const
{
	glBindVertexArray(id);
}

void VertexArray::unbind()
{
	glBindVertexArray(0);
}

void VertexArray::pushAttribute(unsigned int count)
{
	unsigned int offset = stride;
	attributes.push_back({GL_FLOAT, count, stride, offset});
	stride += count * sizeof(float);
}

void VertexArray::pushAttribute(VertexAttribute&& attribute)
{
	attributes.emplace_back(attribute);
	stride += attribute.count * sizeof(float);
}

void VertexArray::addBuffer(const VertexBuffer& vbo)
{
	bind();
	vbo.bind();

	unsigned int attribute_index = 0;
	for (const auto& attribute : attributes) {
		glEnableVertexAttribArray(attribute_index);
		glVertexAttribPointer(attribute_index++,
		                      static_cast<GLint>(attribute.count),
		                      attribute.format,
		                      GL_FALSE,
		                      static_cast<GLsizei>(stride),
		                      reinterpret_cast<const void*>(static_cast<std::uintptr_t>(attribute.offset)));
	}
}
