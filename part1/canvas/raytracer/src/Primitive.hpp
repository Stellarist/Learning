#pragma once

#include "Ray.hpp"
#include "Bound.hpp"
#include "Material.hpp"

struct Primitive {
	virtual ~Primitive() = default;

	virtual Bound bound() const = 0;
	virtual float area() const = 0;
	virtual void  sample(Intersection& pos, float& pdf) = 0;

	virtual bool intersect(const Ray& ray) const = 0;
	virtual bool intersect(const Ray& ray, float& tnear, uint32_t& index) const = 0;
	virtual auto getIntersection(const Ray& ray) -> Intersection = 0;

	virtual bool hasEmission() const = 0;
	virtual auto evalDiffuse(const vec2f_t& texcoords) const -> vec3f_t = 0;
	virtual void getSurfaceProps(const vec3f_t& point, const vec3f_t& direction, uint32_t index, const vec2f_t& uv, vec3f_t& normal, vec2f_t& texcoords) const = 0;
};

struct Triangle : public Primitive {
	vec3f_t   v0, v1, v2;
	vec3f_t   n0, n1, n2;
	vec2f_t   t0, t1, t2;
	float     sarea;
	Material* material;

	Bound bound() const override;
	float area() const override;
	void  sample(Intersection& pos, float& pdf) override;

	bool intersect(const Ray& ray) const override;
	bool intersect(const Ray& ray, float& tnear, uint32_t& index) const override;
	auto getIntersection(const Ray& ray) -> Intersection override;

	bool hasEmission() const override;
	auto evalDiffuse(const vec2f_t& texcoords) const -> vec3f_t override;
	void getSurfaceProps(const vec3f_t& point, const vec3f_t& direction, uint32_t index, const vec2f_t& uv, vec3f_t& normal, vec2f_t& texcoords) const override;

	static bool intersect(const vec3f_t& v0, const vec3f_t& v1, const vec3f_t& v2,
	                      const vec3f_t& origin, const vec3f_t& direction,
	                      float& tnear, float& u, float& v);
};

struct Sphere : public Primitive {
	vec3f_t center;
	float   radius;

	Material* material;

	Bound bound() const override;
	float area() const override;
	void  sample(Intersection& pos, float& pdf) override;

	bool intersect(const Ray& ray) const override;
	bool intersect(const Ray& ray, float& tnear, uint32_t& index) const override;
	auto getIntersection(const Ray& ray) -> Intersection override;

	bool hasEmission() const override;
	auto evalDiffuse(const vec2f_t& texcoords) const -> vec3f_t override;
	void getSurfaceProps(const vec3f_t& point, const vec3f_t& direction, uint32_t index, const vec2f_t& uv, vec3f_t& normal, vec2f_t& texcoords) const override;
};
