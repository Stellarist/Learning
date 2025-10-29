#pragma once

#include "GLBuffer.hpp"
#include "GLTexture.hpp"
#include "GLShader.hpp"

class GLMesh {
private:
	SubMesh* submesh{};

	VertexArray  vao;
	VertexBuffer vbo;
	IndexBuffer  ibo;

	std::unordered_map<std::string, GLTexture*> textures{};

public:
	GLMesh() = default;
	GLMesh(SubMesh* mesh);

	GLMesh(GLMesh& other) = delete;
	GLMesh& operator=(GLMesh& other) = delete;

	GLMesh(GLMesh&& other) noexcept;
	GLMesh& operator=(GLMesh&& other) noexcept;

	~GLMesh() = default;

	void bind() const;
	void unbind() const;

	auto getSubMesh() const -> const SubMesh*;

	void setShader(GLShader& shader);
	auto getShader() -> GLShader*;

	void setTexture(const std::string& name, GLTexture& texture);
	auto getTexture(const std::string& name) -> GLTexture*;
	auto getTextures() const -> const std::unordered_map<std::string, GLTexture*>&;

	void draw(GLShader& shader);
};
