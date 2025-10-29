#include <chrono>
#include <iostream>

#include "Raytracer.hpp"
#include "Model.hpp"

void init(Scene& scene);

int main(int argc, const char* argv[])
{
	Scene scene;
	init(scene);

	auto start = std::chrono::system_clock::now();

	Raytracer raytracer;
	raytracer.render(scene);
	raytracer.save(BUILD_PATH_2 "/cornellbox.ppm");

	auto stop = std::chrono::system_clock::now();

	std::cout << std::flush;
	std::cout << "Render takes: " << std::chrono::duration_cast<std::chrono::seconds>(stop - start).count() << " seconds." << std::endl;

	return 0;
}

void init(Scene& scene)
{
	Material* red = new Material();
	red->emission = vec3f_t::Zero();
	red->kd = vec3f_t(0.63f, 0.065f, 0.05f);
	Material* green = new Material();
	green->emission = vec3f_t::Zero();
	green->kd = vec3f_t(0.14f, 0.45f, 0.091f);
	Material* white = new Material();
	white->emission = vec3f_t::Zero();
	white->kd = vec3f_t(0.725f, 0.71f, 0.68f);
	Material* light = new Material();
	light->emission = vec3f_t(47.8348f, 38.5664f, 31.0808f);
	light->kd = vec3f_t(0.65f, 0.65f, 0.65f);

	auto* floor_mesh = new Model(PROJECT_PATH_2 "/assets/cornell/floor.obj", white);
	auto* shortbox_mesh = new Model(PROJECT_PATH_2 "/assets/cornell/shortbox.obj", white);
	auto* tallbox_mesh = new Model(PROJECT_PATH_2 "/assets/cornell/tallbox.obj", white);
	auto* left_mesh = new Model(PROJECT_PATH_2 "/assets/cornell/left.obj", red);
	auto* right_mesh = new Model(PROJECT_PATH_2 "/assets/cornell/right.obj", green);
	auto* light_mesh = new Model(PROJECT_PATH_2 "/assets/cornell/light.obj", light);

	scene.add(floor_mesh);
	scene.add(shortbox_mesh);
	scene.add(tallbox_mesh);
	scene.add(left_mesh);
	scene.add(right_mesh);
	scene.add(light_mesh);

	scene.buildBVH();
}
