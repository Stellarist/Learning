#pragma once

#include "global.hpp"

struct Light {
	vec3f_t position;
	vec3f_t intensity;

	Light(vec3f_t position, vec3f_t intensity);
	virtual ~Light() = default;
};

struct AreaLight : public Light {
	float   length;
	vec3f_t normal;
	vec3f_t u, v;

	AreaLight(vec3f_t position, vec3f_t intensity);
	~AreaLight() override = default;

	vec3f_t samplePoint() const;
};
