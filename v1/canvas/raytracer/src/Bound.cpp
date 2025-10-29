#include "Bound.hpp"

Bound::Bound(const vec3f_t& p1, const vec3f_t& p2)
{
	pmin = p1.cwiseMin(p2);
	pmax = p1.cwiseMax(p2);
}

double Bound::area() const
{
	vec3f_t d = diagonal();
	return 2.0 * (d[0] * d[1] + d[1] * d[2] + d[2] * d[0]);
}

vec3f_t Bound::diagonal() const
{
	return pmax - pmin;
}

vec3f_t Bound::centroid() const
{
	return (pmin + pmax) * 0.5f;
}

vec3f_t Bound::offset(const vec3f_t& point) const
{
	vec3f_t o = point - pmin;
	if (pmax.x() > pmin.x())
		o[0] /= (pmax.x() - pmin.x());
	if (pmax.y() > pmin.y())
		o[1] /= (pmax.y() - pmin.y());
	if (pmax.z() > pmin.z())
		o[2] /= (pmax.z() - pmin.z());

	return o;
}

int Bound::maxextent() const
{
	vec3f_t d = diagonal();
	if (d.x() > d.y() && d.x() > d.z())
		return 0;
	else if (d.y() > d.z())
		return 1;
	else
		return 2;
}

Bound Bound::intersect(const Bound& b) const
{
	return Bound{
	    pmin.cwiseMax(b.pmin),
	    pmax.cwiseMin(b.pmax)};
}

bool Bound::intersectp(const Ray& ray, const vec3f_t inv_dir, const std::array<int, 3>& dir_is_neg) const
{
	vec3f_t v1 = (pmin - ray.origin).cwiseProduct(inv_dir);
	vec3f_t v2 = (pmax - ray.origin).cwiseProduct(inv_dir);
	vec3f_t tmin = v1.cwiseMin(v2);
	vec3f_t tmax = v1.cwiseMax(v2);
	float   tenter = std::max({tmin.x(), tmin.y(), tmin.z()});
	float   texit = std::min({tmax.x(), tmax.y(), tmax.z()});

	return (tenter <= texit && texit >= 0);
}

bool Bound::overlaps(const Bound& b1, const Bound& b2)
{
	return (b1.pmin.x() <= b2.pmax.x() && b1.pmax.x() >= b2.pmin.x()) &&
	       (b1.pmin.y() <= b2.pmax.y() && b1.pmax.y() >= b2.pmin.y()) &&
	       (b1.pmin.z() <= b2.pmax.z() && b1.pmax.z() >= b2.pmin.z());
}

bool Bound::inside(const vec3f_t& p, const Bound& b)
{
	return (p.x() >= b.pmin.x() && p.x() <= b.pmax.x()) &&
	       (p.y() >= b.pmin.y() && p.y() <= b.pmax.y()) &&
	       (p.z() >= b.pmin.z() && p.z() <= b.pmax.z());
}

Bound Bound::merge(const Bound& b1, const Bound& b2)
{
	return Bound{
	    b1.pmin.cwiseMin(b2.pmin),
	    b1.pmax.cwiseMax(b2.pmax)};
}

Bound Bound::merge(const Bound& b, const vec3f_t& p)
{
	return Bound{
	    b.pmin.cwiseMin(p),
	    b.pmax.cwiseMax(p)};
}
