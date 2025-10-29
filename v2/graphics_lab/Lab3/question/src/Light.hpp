#pragma once

#include <glm/glm.hpp>

#include "Shader.hpp"

/**
 * @brief Enum for different types of lights.
 * @note This enum is used to specify the type of light in the Light struct.
 */
enum class LightType : uint8_t {
	DIRECTIONAL = 0x1,
	POINT = 0x2,
	SPOT = 0x4
};

/**
 * @brief Struct representing a light source in the scene.
 * @note This struct contains properties of the light such as position, direction, color, and attenuation factors.
 */
struct Light {
	/**
	 * @brief The type of the light source.
	 * @note This value determines how the light behaves in the scene (e.g., point, directional, or spot light).
	 */
	LightType type;

	/**
	 * @brief The position of the light in world coordinates.
	 * @note This vector represents the position of the light source in the 3D space.
	 */
	glm::vec3 position;

	/**
	 * @brief The direction of the light.
	 * @note This vector represents the direction in which the light is shining.
	 *       It is used for directional and spot lights.
	 */
	glm::vec3 direction;

	/**
	 * @brief The ambient color of the light.
	 * @note This vector represents the ambient light contribution from the light source.
	 */
	glm::vec3 ambient;

	/**
	 * @brief The diffuse color of the light.
	 * @note This vector represents the diffuse light contribution from the light source.
	 */
	glm::vec3 diffuse;

	/**
	 * @brief The specular color of the light.
	 * @note This vector represents the specular light contribution from the light source.
	 */
	glm::vec3 specular;

	/**
	 * @brief The attenuation factors for the light.
	 * @note These values are used to calculate the attenuation of the light over distance.
	 */
	float constant;

	/**
	 * @brief The linear attenuation factor.
	 * @note This value is used to calculate the linear attenuation of the light over distance.
	 */
	float linear;

	/**
	 * @brief The quadratic attenuation factor.
	 * @note This value is used to calculate the quadratic attenuation of the light over distance.
	 */
	float quadratic;

	/**
	 * @brief The cutoff angle for spot lights.
	 * @note This value is used to determine the angle at which the spot light starts to fade.
	 */
	float cutoff;

	/**
	 * @brief The outer cutoff angle for spot lights.
	 * @note This value is used to determine the angle at which the spot light completely fades out.
	 */
	float outer_cutoff;

	/**
	 * @brief Default constructor for the Light struct.
	 * @note Initializes the light with default values.
	 */
	Light(LightType type = LightType::POINT,
	      glm::vec3 position = glm::vec3(0.0f),
	      glm::vec3 direction = glm::vec3(0.0f, 0.0f, -1.0f),
	      glm::vec3 ambient = glm::vec3(0.1f),
	      glm::vec3 diffuse = glm::vec3(0.8f),
	      glm::vec3 specular = glm::vec3(1.0f),
	      float     constant = 1.0f,
	      float     linear = 0.09f,
	      float     quadratic = 0.032f,
	      float     cutoff = glm::cos(glm::radians(12.5f)),
	      float     outer_cutoff = glm::cos(glm::radians(15.0f)));

	void applyToShader(Shader& shader, const std::string& name) const;
};
