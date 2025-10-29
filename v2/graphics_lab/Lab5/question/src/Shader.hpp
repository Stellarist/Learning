#pragma once

#include <string>
#include <glm/glm.hpp>

/**
 * @brief Shader class for managing OpenGL shaders.
 * @note This class handles the compilation and linking of vertex and fragment shaders.
 *       It also provides methods for setting uniform variables in the shader program.
 */
class Shader {
private:
	/**
	 * @brief The ID of the shader program.
	 * @note This ID is used to reference the shader program in OpenGL.
	 * 	 It is created when the shader program is compiled and linked.
	 */
	unsigned int id;

	/**
	 * @brief Check for compilation errors in the shader.
	 * @param shader The shader ID to check.
	 * @param type The type of shader (vertex, fragment, etc.).
	 * @note This function prints error messages to the console if there are any compilation errors.
	 */
	void checkCompileErrors(unsigned int shader, std::string type);

public:
	Shader(const std::string& vertex_path, const std::string& fragment_path, const std::string& geometry_path = "");
	~Shader();

	/**
	 * @brief Get the ID of the shader program.
	 * @return The ID of the shader program.
	 * @note This ID is used to reference the shader program in OpenGL.
	 */
	unsigned int getId();

	/**
	 * @brief Use the shader program.
	 * @note This function activates the shader program for rendering.
	 *       It should be called before drawing any objects that use this shader.
	 */
	void use();

	/**
	 * @brief Set uniform variables in the shader program.
	 * @param name The name of the uniform variable in the shader.
	 * @param value The value to set for the uniform variable.
	 * @note The type of the value should match the type of the uniform variable in the shader.
	 *       For example, if the uniform variable is a float, use setFloat.
	 */
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
};
