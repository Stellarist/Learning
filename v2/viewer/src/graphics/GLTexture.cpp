#include "GLTexture.hpp"

GLTexture::GLTexture() :
    id(0),
    format(GL_RGBA),
    data_type(GL_UNSIGNED_BYTE)
{
	glGenTextures(1, &id);
}

GLTexture::GLTexture(unsigned int format, unsigned int data_type) :
    id(0),
    format(format),
    data_type(data_type)
{
	glGenTextures(1, &id);
}

GLTexture::GLTexture(
    const void* data, int width, int height,
    unsigned int format, unsigned int data_type) :
    id(0),
    format(format),
    data_type(data_type)
{
	glGenTextures(1, &id);
	upload(data, width, height);
}

GLTexture::GLTexture(GLTexture&& other) noexcept :
    id(other.id),
    format(other.format),
    data_type(other.data_type)
{
	other.id = 0;
}

GLTexture& GLTexture::operator=(GLTexture&& other) noexcept
{
	if (this != &other) {
		if (id)
			glDeleteTextures(1, &id);

		id = other.id;
		format = other.format;
		data_type = other.data_type;

		other.id = 0;
	}

	return *this;
}

GLTexture::~GLTexture()
{
	if (id) {
		glDeleteTextures(1, &id);
		id = 0;
	}
}

void GLTexture::upload(const void* data, int width, int height) const
{
	bind();
	glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(format), width, height,
	             0, format, data_type, data);
	unbind();
}

unsigned int GLTexture::getId() const
{
	return id;
}

void GLTexture::bind() const
{
	glBindTexture(GL_TEXTURE_2D, id);
}

void GLTexture::unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void GLTexture::activate(unsigned int unit) const
{
	glActiveTexture(GL_TEXTURE0 + unit);
	bind();
}

void GLTexture::deactivate(unsigned int unit)
{
	glActiveTexture(GL_TEXTURE0 + unit);
	unbind();
}

void GLTexture::setParameteri(std::initializer_list<std::pair<int, int>> param) const
{
	bind();
	for (const auto& p : param)
		glTexParameteri(GL_TEXTURE_2D, p.first, p.second);
	unbind();
}

void GLTexture::generateMipmap() const
{
	bind();
	glGenerateMipmap(GL_TEXTURE_2D);
	unbind();
}
