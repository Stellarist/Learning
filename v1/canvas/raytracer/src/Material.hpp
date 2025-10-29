#pragma once

#include "global.hpp"

struct Material {
	vec3f_t kd;
	vec3f_t ks;
	float   ior;
	vec3f_t emission;
	float   specular_exponent;

	bool hasEmission() const;

	vec3f_t reflect(const vec3f_t& normal, const vec3f_t& incident);
	vec3f_t refract(const vec3f_t& normal, const vec3f_t& incident, float ior);
	float   fresnel(const vec3f_t& normal, const vec3f_t& incident, float ior);
	vec3f_t toWorld(const vec3f_t& local, const vec3f_t& normal) const;

	vec3f_t sample(const vec3f_t& wi, const vec3f_t& normal);
	vec3f_t eval(const vec3f_t& wi, const vec3f_t& wo, const vec3f_t& normal) const;
	float   pdf(const vec3f_t& wi, const vec3f_t& wo, const vec3f_t& normal) const;
};
