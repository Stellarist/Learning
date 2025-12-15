#include "Material.hpp"

bool Material::hasEmission() const
{
	return emission.norm() > 1e-6f;
}

vec3f_t Material::reflect(const vec3f_t& normal, const vec3f_t& incident)
{
	return incident - 2 * normal.dot(incident) * normal;
}

vec3f_t Material::refract(const vec3f_t& normal, const vec3f_t& incident, float ior)
{
	float   cosi = std::clamp(-1.f, 1.f, -normal.dot(incident));
	float   etai = 1.0f, etat = ior;
	vec3f_t n = normal;
	if (cosi < 0.0f)
		cosi = -cosi;
	else {
		std::swap(etai, etat);
		n = -normal;
	}
	float eta = etai / etat;
	float k = 1.0f - eta * eta * (1.0f - cosi * cosi);

	if (k < 0.f)
		return vec3f_t(0.0f, 0.0f, 0.0f);
	else
		return eta * incident + (eta * cosi - std::sqrt(k)) * n;
}

float Material::fresnel(const vec3f_t& normal, const vec3f_t& incident, float ior)
{
	float cosi = std::clamp(-1.f, 1.f, -normal.dot(incident));
	float etai = 1.0f, etat = ior;
	if (cosi > 0.0f)
		std::swap(etai, etat);
	float sint = etai / etat * std::sqrt(std::max(0.f, 1.f - cosi * cosi));
	if (sint >= 1.0f)
		return 1.0f;

	float cost = std::sqrt(std::max(0.f, 1.f - sint * sint));
	cosi = std::abs(cosi);
	float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
	float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
	return (Rs * Rs + Rp * Rp) / 2.0f;
}

vec3f_t Material::toWorld(const vec3f_t& local, const vec3f_t& normal) const
{
	vec3f_t a, b;
	if (std::fabs(normal.x()) > std::fabs(normal.y())) {
		float inv_len = 1.f / std::sqrt(normal.x() * normal.x() + normal.z() * normal.z());
		b = vec3f_t(normal.z() * inv_len, 0.f, -normal.x() * inv_len);
	} else {
		float inv_len = 1.f / std::sqrt(normal.y() * normal.y() + normal.z() * normal.z());
		b = vec3f_t(0.f, normal.z() * inv_len, -normal.y() * inv_len);
	}
	a = b.cross(normal);

	return local.x() * a + local.y() * b + local.z() * normal;
}

vec3f_t Material::sample(const vec3f_t& wi, const vec3f_t& normal)
{
	float x1 = Geometry::randomFloat();
	float x2 = Geometry::randomFloat();
	float z = std::fabs(1.f - 2.f * x1);
	float r = std::sqrt(1.f - z * z);
	float phi = 2.f * PI * x2;

	vec3f_t local(r * std::cos(phi), r * std::sin(phi), z);

	return toWorld(local, normal);
}

vec3f_t Material::eval(const vec3f_t& wi, const vec3f_t& wo, const vec3f_t& normal) const
{
	return normal.dot(wo) > .0f ? kd / PI : vec3f_t{};
}

float Material::pdf(const vec3f_t& wi, const vec3f_t& wo, const vec3f_t& normal) const
{
	return normal.dot(wo) > 0.f ? .5f / PI : 0.f;
}
