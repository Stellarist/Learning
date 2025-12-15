#include "Light.hpp"

Light::Light(vec3f_t position, vec3f_t intensity) :
    position(std::move(position)),
    intensity(std::move(intensity))
{}

AreaLight::AreaLight(vec3f_t position, vec3f_t intensity) :
    Light(std::move(position), std::move(intensity)),
    normal(0, -1, 0),
    u(1, 0, 0),
    v(0, 0, 1),
    length(100.f)
{}

vec3f_t AreaLight::samplePoint() const
{
	auto random_u = Geometry::randomFloat();
	auto random_v = Geometry::randomFloat();

	return position + random_u * u + random_v * v;
}
