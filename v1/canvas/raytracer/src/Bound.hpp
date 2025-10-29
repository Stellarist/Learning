#pragma once

#include <limits>

#include "global.hpp"
#include "Ray.hpp"

struct Bound {
	vec3f_t pmin{std::numeric_limits<float>::max(),
	             std::numeric_limits<float>::max(),
	             std::numeric_limits<float>::max()};
	vec3f_t pmax{std::numeric_limits<float>::min(),
	             std::numeric_limits<float>::min(),
	             std::numeric_limits<float>::min()};

	Bound() = default;
	Bound(const vec3f_t& p1, const vec3f_t& p2);

	double  area() const;
	vec3f_t diagonal() const;
	vec3f_t centroid() const;
	vec3f_t offset(const vec3f_t& point) const;
	int     maxextent() const;

	Bound intersect(const Bound& b) const;
	bool  intersectp(const Ray& ray, const vec3f_t inv_dir,
	                 const std::array<int, 3>& dir_is_neg) const;

	static bool  overlaps(const Bound& b1, const Bound& b2);
	static bool  inside(const vec3f_t& p, const Bound& b);
	static Bound merge(const Bound& b1, const Bound& b2);
	static Bound merge(const Bound& b, const vec3f_t& p);
};
