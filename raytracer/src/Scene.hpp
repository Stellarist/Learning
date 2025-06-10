#pragma once

#include "Light.hpp"
#include "BVH.hpp"

struct Scene {
	BVHAccel* bvh;

	int width{48};
	int height{64};

	int   max_depth{3};
	float russian_roulette{0.8f};

	std::vector<Light*>     lights;
	std::vector<Primitive*> primitives;

	~Scene();

	void add(Primitive* primitive);
	void add(Light* light);

	auto getLights() const -> const std::vector<Light*>&;
	auto getPrimitives() const -> const std::vector<Primitive*>&;

	void buildBVH();
	auto intersect(const Ray& ray) const -> Intersection;
	void sampleLight(Intersection& pos, float& pdf) const;
	auto castRay(const Ray& ray, int depth) const -> vec3f_t;
	bool trace(const Ray& ray, const std::vector<Primitive*>& objects, float& tnear, uint32_t& index, Primitive** hit_object);
};
