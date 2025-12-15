#include "Light.hpp"

Light::Light(LightType type, glm::vec3 position, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular,
             float constant, float linear, float quadratic, float cutoff, float outer_cutoff) :
    type(type), position(position), direction(direction), ambient(ambient), diffuse(diffuse), specular(specular), constant(constant), linear(linear), quadratic(quadratic), cutoff(cutoff), outer_cutoff(outer_cutoff) {}

void Light::applyToShader(Shader& shader, const std::string& name) const
{
	shader.setVec3(name + ".ambient", ambient);
	shader.setVec3(name + ".diffuse", diffuse);
	shader.setVec3(name + ".specular", specular);

	if (type == LightType::DIRECTIONAL) {
		shader.setVec3(name + ".direction", direction);
	} else if (type == LightType::POINT) {
		shader.setVec3(name + ".position", position);
		shader.setFloat(name + ".constant", constant);
		shader.setFloat(name + ".linear", linear);
		shader.setFloat(name + ".quadratic", quadratic);
	} else if (type == LightType::SPOT) {
		shader.setVec3(name + ".position", position);
		shader.setVec3(name + ".direction", direction);
		shader.setFloat(name + ".cutOff", cutoff);
		shader.setFloat(name + ".outerCutOff", outer_cutoff);
		shader.setFloat(name + ".constant", constant);
		shader.setFloat(name + ".linear", linear);
		shader.setFloat(name + ".quadratic", quadratic);
	}
}
