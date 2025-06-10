#include "Ray.hpp"

vec3f_t Ray::at(double t) const
{
	return origin + t * direction;
}
