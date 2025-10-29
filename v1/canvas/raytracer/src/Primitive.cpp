#include "Primitive.hpp"

bool Triangle::intersect(const vec3f_t& v0, const vec3f_t& v1, const vec3f_t& v2,
                         const vec3f_t& origin, const vec3f_t& direction,
                         float& tnear, float& u, float& v)
{
	vec3f_t edge1 = v1 - v0;
	vec3f_t edge2 = v2 - v0;
	vec3f_t pvec = direction.cross(edge2);
	float   det = edge1.dot(pvec);
	if (std::fabs(det) < 1e-8f)
		return false;

	vec3f_t tvec = origin - v0;
	u = tvec.dot(pvec);
	if (u < 0.f || u > det)
		return false;

	vec3f_t qvec = tvec.cross(edge1);
	v = direction.dot(qvec);
	if (v < 0.f || u + v > det)
		return false;

	float inv_det = 1.0f / det;
	tnear = edge2.dot(qvec) * inv_det;
	u *= inv_det;
	v *= inv_det;

	return true;
}

Bound Triangle::bound() const
{
	return Bound{
	    v0.cwiseMin(v1).cwiseMin(v2),
	    v0.cwiseMax(v1).cwiseMax(v2)};
}

float Triangle::area() const
{
	return 0.5f * (v1 - v0).cross(v2 - v0).norm();
}

void Triangle::sample(Intersection& pos, float& pdf)
{
	float r1 = Geometry::randomFloat();
	float r2 = Geometry::randomFloat();
	if (r1 + r2 > 1.0f) {
		r1 = 1.0f - r1;
		r2 = 1.0f - r2;
	}
	float r3 = 1.0f - r1 - r2;

	pos.hit = true;
	pos.position = r3 * v0 + r1 * v1 + r2 * v2;
	pos.normal = (v1 - v0).cross(v2 - v0).normalized();
	pos.texcoord = vec2f_t(r1, r2);
	pos.material = material;
	pos.primitive = this;
	pos.emit = material ? material->emission : vec3f_t(0, 0, 0);
	pdf = 1.0f / area();
}

bool Triangle::intersect(const Ray& ray) const
{
	return true;
}

bool Triangle::intersect(const Ray& ray, float& tnear, uint32_t& index) const
{
	return false;
}

Intersection Triangle::getIntersection(const Ray& ray)
{
	Intersection intersection;

	vec3f_t e1 = v1 - v0, e2 = v2 - v0;
	vec3f_t normal = e1.cross(e2).normalized();
	if (ray.direction.dot(normal) > 0.f)
		return intersection;

	float   u, v, tnear{};
	vec3f_t pvec = ray.direction.cross(e2);
	float   det = e1.dot(pvec);
	if (std::fabs(det) < 1e-8f)
		return intersection;

	float   inv_det = 1.0f / det;
	vec3f_t tvec = ray.origin - v0;
	u = tvec.dot(pvec) * inv_det;
	if (u < 0 || u > 1)
		return intersection;

	vec3f_t qvec = tvec.cross(e1);
	v = ray.direction.dot(qvec) * inv_det;
	if (v < 0 || u + v > 1)
		return intersection;

	tnear = e2.dot(qvec) * inv_det;
	if (tnear < 0)
		return intersection;

	intersection.hit = true;
	intersection.position = ray.at(tnear);
	intersection.distance = tnear;
	intersection.texcoord = vec2f_t(u, v);
	intersection.normal = normal;
	intersection.material = this->material;
	intersection.primitive = this;

	return intersection;
}

bool Triangle::hasEmission() const
{
	return material && material->hasEmission();
}

vec3f_t Triangle::evalDiffuse(const vec2f_t& texcoords) const
{
	if (material)
		return material->kd;

	return vec3f_t(1.0f, 1.0f, 1.0f);
}

void Triangle::getSurfaceProps(const vec3f_t& point, const vec3f_t& direction, uint32_t index, const vec2f_t& uv, vec3f_t& normal, vec2f_t& texcoords) const
{
	float w = 1.f - uv.x() - uv.y();
	normal = (w * n0 + uv.x() * n1 + uv.y() * n2).normalized();
	texcoords = w * t0 + uv.x() * t1 + uv.y() * t2;
}

Bound Sphere::bound() const
{
	return Bound{
	    center - vec3f_t(radius, radius, radius),
	    center + vec3f_t(radius, radius, radius)};
}

float Sphere::area() const
{
	return 4.0f * PI * radius * radius;
}

void Sphere::sample(Intersection& pos, float& pdf)
{
	float u1 = Geometry::randomFloat() * 2.0f * PI;
	float u2 = Geometry::randomFloat() * PI;
	float z = 1.f - 2.f * u1;
	float r = std::sqrt(std::max(0.f, 1.f - z * z));
	float phi = 2.f * PI * u2;
	float theta = std::acos(std::clamp(z, -1.f, 1.f));
	float u = (phi + PI) / (2.f * PI);
	float v = theta / PI;

	vec3f_t dir(r * std::cos(phi), r * std::sin(phi), z);

	pos.hit = true;
	pos.position = center + dir * radius;
	pos.normal = dir;
	pos.material = material;
	pos.primitive = this;
	pos.texcoord = vec2f_t(u, v);
	pos.emit = vec3f_t(0, 0, 0);
	pdf = 1.0f / area();
}

bool Sphere::intersect(const Ray& ray) const
{
	vec3f_t l = ray.origin - center;
	float   a = ray.direction.dot(ray.direction);
	float   b = ray.direction.dot(l) * 2;
	float   c = l.dot(l) - radius * radius;
	float   t0, t1;
	float   area = 4 * PI * radius * radius;

	if (!Geometry::solveQuadratic(a, b, c, t0, t1))
		return false;
	if (t0 < 0)
		t0 = t1;
	if (t0 < 0)
		return false;

	return true;
}

bool Sphere::intersect(const Ray& ray, float& tnear, uint32_t& index) const
{
	vec3f_t l = ray.origin - center;
	float   a = ray.direction.dot(ray.direction);
	float   b = ray.direction.dot(l) * 2.f;
	float   c = l.dot(l) - radius * radius;
	float   t0, t1;

	if (!Geometry::solveQuadratic(a, b, c, t0, t1))
		return false;
	if (t0 < 0.f)
		t0 = t1;
	if (t0 < 0.f)
		return false;
	tnear = t0;

	return true;
}

Intersection Sphere::getIntersection(const Ray& ray)
{
	Intersection intersection;
	float        tnear;
	uint32_t     index;
	vec3f_t      local_point = (intersection.position - center) / radius;
	float        phi = std::atan2(local_point.z(), local_point.x());
	float        theta = std::acos(std::clamp(local_point.y(), -1.f, 1.f));
	float        u = (phi + PI) / (2.f * PI);
	float        v = theta / PI;

	intersection.hit = intersect(ray, tnear, index);
	if (!intersection.hit)
		return intersection;

	intersection.position = ray.origin + ray.direction * tnear;
	intersection.normal = (intersection.position - center).normalized();
	intersection.material = this->material;
	intersection.primitive = this;
	intersection.distance = tnear;
	intersection.texcoord = vec2f_t(u, v);
	intersection.emit = vec3f_t(0, 0, 0);

	return intersection;
}

bool Sphere::hasEmission() const
{
	return material && material->hasEmission();
}

vec3f_t Sphere::evalDiffuse(const vec2f_t& texcoords) const
{
	if (material)
		return material->kd;

	return vec3f_t(1.f, 1.f, 1.f);
}

void Sphere::getSurfaceProps(const vec3f_t& point, const vec3f_t& direction, uint32_t index, const vec2f_t& uv, vec3f_t& normal, vec2f_t& texcoords) const
{
	normal = (point - center).normalized();
	vec3f_t local_point = (point - center) / radius;
	float   phi = std::atan2(normal.z(), normal.x());
	float   theta = std::acos(std::clamp(normal.y(), -1.f, 1.f));
	texcoords.x() = (phi + PI) / (2.f * PI);
	texcoords.y() = theta / PI;
}
