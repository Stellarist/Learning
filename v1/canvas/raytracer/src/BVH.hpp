#pragma once

#include "Bound.hpp"
#include "Primitive.hpp"

enum class BVHBuildMethod {
	NAIVE,
	SAH
};

struct BVHNode {
	Bound      bound{};
	BVHNode*   left{};
	BVHNode*   right{};
	Primitive* primitive{};

	int   split_axis{};
	int   first_offset{};
	int   num_primitives{};
	float area{};
};

struct BVHAccel {
	BVHNode* root{};

	const int            MAX_PRIMITIVES_PER_LEAF;
	const BVHBuildMethod BUILD_METHOD;

	BVHAccel(std::vector<Primitive*> primitives,
	         int                     max_primitives_per_leaf = 1,
	         BVHBuildMethod          build_method = BVHBuildMethod::NAIVE);
	~BVHAccel();

	auto build(std::vector<Primitive*> primitives) -> BVHNode*;
	auto destroy(BVHNode* node) -> void;

	auto bound() const -> Bound;

	auto intersect(const Ray& ray) const -> Intersection;
	auto getIntersection(BVHNode* node, const Ray& ray) const -> Intersection;

	void sample(Intersection& pos, float& pdf) const;
	void getSample(BVHNode* node, float p, Intersection& pos, float& pdf) const;
};
