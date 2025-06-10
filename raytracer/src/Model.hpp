#pragma once

#include <string>
#include <vector>
#include <stb_image.h>
#include <tiny_obj_loader.h>

#include "Bound.hpp"
#include "Primitive.hpp"
#include "BVH.hpp"

enum class TextureType {
	DIFFUSE,
	SPECULAR,
	BUMP
};

struct Texture {
	int width;
	int height;
	int nrChannels;

	std::string          file_path;
	std::vector<vec3f_t> data;
	TextureType          type;

	Texture(const std::string& file_path, TextureType type);
	vec3f_t sample(float u, float v) const;
};

struct Model : public Primitive {
	std::vector<Triangle> triangles;
	std::vector<Material> materials;

	std::unordered_map<std::string, Texture> textures;

	BVHAccel* bvh{};
	Material* default_material{nullptr};

	bool  has_emission{};
	float total_area{};
	Bound bounding_box{};

	Model(const std::string& filepath, Material* material = nullptr);
	~Model() override;

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
