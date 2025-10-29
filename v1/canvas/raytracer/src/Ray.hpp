#pragma once

#include "global.hpp"
#include "Material.hpp"

class Primitive;

struct Ray {
	vec3f_t origin;
	vec3f_t direction;
	double  time;

	vec3f_t at(double t) const;
};

struct Intersection {
	vec3f_t position;
	vec3f_t normal;
	vec2f_t texcoord;
	vec3f_t emit;
	float   distance{std::numeric_limits<float>::max()};

	bool       hit{false};
	Material*  material{nullptr};
	Primitive* primitive{nullptr};
};
