#include "GLShader.hpp"

#include <print>
#include <array>
#include <fstream>
#include <sstream>

GLShader::GLShader(const std::string& vertex_path, const std::string& fragment_path, const std::string& geometry_path) :
    id(0)
{
	std::string   vertex_source, fragment_source, geometry_source;
	std::ifstream vertex_file, fragment_file, geometry_file;
	vertex_file.exceptions(std::ios_base::failbit | std::ios_base::badbit);
	fragment_file.exceptions(std::ios_base::failbit | std::ios_base::badbit);
	geometry_file.exceptions(std::ios_base::failbit | std::ios_base::badbit);

	try {
		vertex_file.open(vertex_path);
		fragment_file.open(fragment_path);
		std::stringstream vertex_stream, fragment_stream;
		vertex_stream << vertex_file.rdbuf();
		fragment_stream << fragment_file.rdbuf();
		vertex_file.close();
		fragment_file.close();
		vertex_source = vertex_stream.str();
		fragment_source = fragment_stream.str();
		if (!geometry_path.empty()) {
			geometry_file.open(geometry_path);
			std::stringstream geometry_stream;
			geometry_stream << geometry_file.rdbuf();
			geometry_file.close();
			geometry_source = geometry_stream.str();
		}
	} catch (const std::ifstream::failure& e) {
		std::println("ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: {}", e.what());
	}

	unsigned int vertex = 0;
	const char*  vertex_code = vertex_source.c_str();
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vertex_code, nullptr);
	glCompileShader(vertex);
	checkCompileErrors(vertex, "VERTEX");

	unsigned int fragment = 0;
	const char*  fragment_code = fragment_source.c_str();
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fragment_code, nullptr);
	glCompileShader(fragment);
	checkCompileErrors(fragment, "FRAGMENT");

	unsigned int geometry = 0;
	if (!geometry_path.empty()) {
		const char* geometry_code = geometry_source.c_str();
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &geometry_code, nullptr);
		glCompileShader(geometry);
		checkCompileErrors(geometry, "GEOMETRY");
	}

	id = glCreateProgram();
	glAttachShader(id, vertex);
	glAttachShader(id, fragment);
	if (!geometry_path.empty())
		glAttachShader(id, geometry);
	glLinkProgram(id);
	checkCompileErrors(id, "PROGRAM");

	glDeleteShader(vertex);
	glDeleteShader(fragment);
	if (!geometry_path.empty())
		glDeleteShader(geometry);
}

GLShader::GLShader(GLShader&& other) noexcept :
    id(other.id)
{
	other.id = 0;
}

GLShader& GLShader::operator=(GLShader&& other) noexcept
{
	if (this != &other) {
		if (id)
			glDeleteProgram(id);

		id = other.id;
		other.id = 0;
	}

	return *this;
}

GLShader::~GLShader()
{
	glDeleteProgram(id);
}

unsigned int GLShader::getId() const
{
	return id;
}

void GLShader::use() const
{
	glUseProgram(id);
}

void GLShader::setBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void GLShader::setInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void GLShader::setFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}

void GLShader::setVec2(const std::string& name, const glm::vec2& value) const
{
	glUniform2fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
}

void GLShader::setVec2(const std::string& name, float x, float y) const
{
	glUniform2f(glGetUniformLocation(id, name.c_str()), x, y);
}

void GLShader::setVec3(const std::string& name, const glm::vec3& value) const
{
	glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
}

void GLShader::setVec3(const std::string& name, float x, float y, float z) const
{
	glUniform3f(glGetUniformLocation(id, name.c_str()), x, y, z);
}

void GLShader::setVec4(const std::string& name, const glm::vec4& value) const
{
	glUniform4fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
}

void GLShader::setVec4(const std::string& name, float x, float y, float z, float w) const
{
	glUniform4f(glGetUniformLocation(id, name.c_str()), x, y, z, w);
}

void GLShader::setMat2(const std::string& name, const glm::mat2& mat) const
{
	glUniformMatrix2fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void GLShader::setMat3(const std::string& name, const glm::mat3& mat) const
{
	glUniformMatrix3fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void GLShader::setMat4(const std::string& name, const glm::mat4& mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void GLShader::checkCompileErrors(unsigned int shader, std::string type)
{
	int           success = 0;
	constexpr int ARRAY_SIZE = 1024;

	std::array<char, ARRAY_SIZE> info_log{};
	if (type != "PROGRAM") {
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(shader, ARRAY_SIZE, nullptr, info_log.data());
			std::println("ERROR::SHADER_COMPILATION_ERROR of type: {}\n \t{}\n", type, info_log.data());
		}
	} else {
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shader, ARRAY_SIZE, nullptr, info_log.data());
			std::println("ERROR::PROGRAM_LINKING_ERROR of type: {}\n \t{}\n", type, info_log.data());
		}
	}
}
