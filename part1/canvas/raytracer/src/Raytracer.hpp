#pragma once

#include "Scene.hpp"

class Raytracer {
public:
	Scene* scene;

	int samples_per_pixel{16};

	float fov;
	float scale;
	float aspect_ratio;

	vec3f_t background_color;
	vec3f_t camera_position;

	std::vector<vec3f_t> framebuffer;

	void render(Scene& new_scene);
	void save(const std::string& filename);
};
