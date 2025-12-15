#pragma once

#include <string>
#include <glm/glm.hpp>
#include <glad/glad.h>

struct GLShader {
	unsigned int id;

	GLShader(const std::string& vertex_path, const std::string& fragment_path, const std::string& geometry_path = {});

	GLShader(const GLShader& other) = delete;
	GLShader& operator=(const GLShader& other) = delete;

	GLShader(GLShader&& other) noexcept;
	GLShader& operator=(GLShader&& other) noexcept;

	~GLShader();

	unsigned int getId() const;

	void use() const;
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setVec2(const std::string& name, const glm::vec2& value) const;
	void setVec2(const std::string& name, float x, float y) const;
	void setVec3(const std::string& name, const glm::vec3& value) const;
	void setVec3(const std::string& name, float x, float y, float z) const;
	void setVec4(const std::string& name, const glm::vec4& value) const;
	void setVec4(const std::string& name, float x, float y, float z, float w) const;
	void setMat2(const std::string& name, const glm::mat2& mat) const;
	void setMat3(const std::string& name, const glm::mat3& mat) const;
	void setMat4(const std::string& name, const glm::mat4& mat) const;

	static void checkCompileErrors(unsigned int shader, std::string type);
};
