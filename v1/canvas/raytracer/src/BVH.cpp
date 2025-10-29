#include "BVH.hpp"

BVHAccel::BVHAccel(std::vector<Primitive*> primitives,
                   int                     max_primitives_per_leaf,
                   BVHBuildMethod          build_method) :
    MAX_PRIMITIVES_PER_LEAF(max_primitives_per_leaf),
    BUILD_METHOD(build_method)
{
	if (primitives.empty())
		return;

	root = build(primitives);
}

BVHAccel::~BVHAccel()
{
	destroy(root);
}

BVHNode* BVHAccel::build(std::vector<Primitive*> primitives)
{
	auto* node = new BVHNode();

	Bound total_bound{};
	for (const auto* p : primitives)
		total_bound = Bound::merge(total_bound, p->bound());

	if (primitives.size() == 1) {
		node->bound = primitives.front()->bound();
		node->primitive = primitives.front();
		node->left = nullptr;
		node->right = nullptr;
		node->area = primitives.front()->area();
		return node;
	} else if (primitives.size() == 2) {
		node->left = build({primitives[0]});
		node->right = build({primitives[1]});
		node->bound = Bound::merge(node->left->bound, node->right->bound);
		node->area = node->left->area + node->right->area;
		return node;
	} else {
		Bound centroid_bound{};
		for (auto* p : primitives)
			centroid_bound = Bound::merge(centroid_bound, p->bound().centroid());

		switch (int dim = centroid_bound.maxextent(); dim) {
		case 0:
			std::sort(primitives.begin(), primitives.end(), [](auto* a, auto* b) {
				return a->bound().centroid().x() < b->bound().centroid().x();
			});
			break;
		case 1:
			std::sort(primitives.begin(), primitives.end(), [](auto* a, auto* b) {
				return a->bound().centroid().y() < b->bound().centroid().y();
			});
			break;
		case 2:
			std::sort(primitives.begin(), primitives.end(), [](auto* a, auto* b) {
				return a->bound().centroid().z() < b->bound().centroid().z();
			});
			break;
		}

		auto beg = primitives.begin();
		auto mid = primitives.begin() + primitives.size() / 2;
		auto end = primitives.end();
		auto left_primitives = std::vector<Primitive*>(beg, mid);
		auto right_primitives = std::vector<Primitive*>(mid, end);

		assert(primitives.size() == (left_primitives.size() + right_primitives.size()));

		node->left = build(left_primitives);
		node->right = build(right_primitives);

		node->bound = Bound::merge(node->left->bound, node->right->bound);
		node->area = node->left->area + node->right->area;
	}

	return node;
}

void BVHAccel::destroy(BVHNode* node)
{
	if (!node)
		return;

	if (node->left)
		destroy(node->left);
	if (node->right)
		destroy(node->right);

	delete node;
}

Intersection BVHAccel::intersect(const Ray& ray) const
{
	if (!root)
		return Intersection{};

	return getIntersection(root, ray);
}

Intersection BVHAccel::getIntersection(BVHNode* node, const Ray& ray) const
{
	Intersection intersection;

	vec3f_t inv_dir = ray.direction.cwiseInverse();
	if (!node || !node->bound.intersectp(ray, inv_dir, {0, 0, 0}))
		return intersection;

	if (!node->left && !node->right)
		return node->primitive->getIntersection(ray);

	Intersection hit1 = getIntersection(node->left, ray);
	Intersection hit2 = getIntersection(node->right, ray);

	return hit1.distance < hit2.distance ? hit1 : hit2;
}

void BVHAccel::sample(Intersection& pos, float& pdf) const
{
	float p = Geometry::randomFloat() * root->area;
	getSample(root, p, pos, pdf);
	pdf /= root->area;
}

void BVHAccel::getSample(BVHNode* node, float p, Intersection& pos, float& pdf) const
{
	if (!node->left && !node->right) {
		node->primitive->sample(pos, pdf);
		pdf *= node->area;
		return;
	}

	if (p < node->left->area)
		getSample(node->left, p, pos, pdf);
	else
		getSample(node->right, p, pos, pdf);
}
