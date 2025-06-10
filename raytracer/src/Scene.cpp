#include "Scene.hpp"

Scene::~Scene()
{
	delete bvh;
	bvh = nullptr;
	for (auto* light : lights)
		delete light;
	for (auto* primitive : primitives)
		delete primitive;
}

void Scene::add(Primitive* primitive)
{
	primitives.push_back(primitive);
}

void Scene::add(Light* light)
{
	lights.push_back(light);
}

const std::vector<Light*>& Scene::getLights() const
{
	return lights;
}

const std::vector<Primitive*>& Scene::getPrimitives() const
{
	return primitives;
}

void Scene::buildBVH()
{
	bvh = new BVHAccel(primitives, 1, BVHBuildMethod::NAIVE);
}

Intersection Scene::intersect(const Ray& ray) const
{
	return bvh->intersect(ray);
}

void Scene::sampleLight(Intersection& pos, float& pdf) const
{
	float emit_area_sum = 0;
	for (const auto& p : primitives)
		if (p->hasEmission())
			emit_area_sum += p->area();

	float a = Geometry::randomFloat() * emit_area_sum;
	emit_area_sum = 0;
	for (const auto& p : primitives) {
		if (p->hasEmission()) {
			emit_area_sum += p->area();
			if (a <= emit_area_sum) {
				p->sample(pos, pdf);
				break;
			}
		}
	}
}

vec3f_t Scene::castRay(const Ray& ray, int depth) const
{
	constexpr float EPSILON = 0.0001f;
	static int      cnt = 0;

	vec3f_t direct_lighting = vec3f_t::Zero();
	vec3f_t indirect_lighting = vec3f_t::Zero();

	// max depth check
	if (depth >= max_depth)
		return vec3f_t::Zero();

	// hit check
	Intersection hit_point = intersect(ray);
	if (!hit_point.hit)
		return vec3f_t::Zero();

	// material check
	if (!hit_point.material)
		return vec3f_t::Zero();

	// emission check
	if (hit_point.material->hasEmission())
		return hit_point.material->emission;

	// direct lighting
	Intersection light_sample{};
	float        light_pdf{};
	sampleLight(light_sample, light_pdf);

	vec3f_t hit_position = hit_point.position;
	vec3f_t light_position = light_sample.position;
	vec3f_t light_direction = (light_position - hit_position).normalized();
	float   light_distance = (light_position - hit_position).norm();
	vec3f_t surface_normal = hit_point.normal.normalized();
	vec3f_t light_normal = light_sample.normal.normalized();
	vec3f_t light_emission = light_sample.emit;

	Ray          direct_ray(hit_position, light_direction);
	Intersection direct_hit = intersect(direct_ray);
	if (direct_hit.distance - light_distance > -EPSILON) {
		vec3f_t direct_brdf = hit_point.material->eval(ray.direction, direct_ray.direction, surface_normal);
		direct_lighting = light_emission.cwiseProduct(direct_brdf) * direct_ray.direction.dot(surface_normal) * (-direct_ray.direction).dot(light_normal) / (std::pow(light_distance, 2)) / light_pdf;
	}

	if (Geometry::randomFloat() > russian_roulette)
		return direct_lighting;

	vec3f_t      indirect_direction = hit_point.material->sample(ray.direction, surface_normal).normalized();
	Ray          indirect_ray(hit_point.position, indirect_direction);
	Intersection indirect_hit = intersect(indirect_ray);
	if (indirect_hit.hit && (!indirect_hit.material->hasEmission())) {
		vec3f_t indirect_brdf = hit_point.material->eval(ray.direction, indirect_direction, surface_normal);
		float   pdf = hit_point.material->pdf(ray.direction, indirect_ray.direction, surface_normal);
		indirect_lighting = castRay(indirect_ray, depth + 1).cwiseProduct(indirect_brdf) * indirect_ray.direction.dot(surface_normal) / pdf / russian_roulette;
	}

	return direct_lighting + indirect_lighting;
}

bool Scene::trace(const Ray& ray, const std::vector<Primitive*>& primitives, float& tnear, uint32_t& index, Primitive** hit_object)
{
	*hit_object = nullptr;
	for (auto& pr : primitives) {
		float    tneark = std::numeric_limits<float>::max();
		uint32_t indexk = 0;
		vec2f_t  uvk;
		if (pr->intersect(ray, tneark, indexk) && tneark < tnear) {
			*hit_object = pr;
			tnear = tneark;
			index = indexk;
		}
	}

	return *hit_object;
}
