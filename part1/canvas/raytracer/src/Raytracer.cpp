#include "Raytracer.hpp"

#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>

void Raytracer::render(Scene& new_scene)
{
	this->scene = &new_scene;
	framebuffer.resize(scene->width * scene->height, vec3f_t::Zero());
	fov = 40.0f;
	scale = std::tan(Geometry::radians(fov) / 2.0f);
	aspect_ratio = static_cast<float>(scene->width) / static_cast<float>(scene->height);
	camera_position = vec3f_t(278, 273, -800);

	const int num_threads = std::thread::hardware_concurrency();

	std::vector<std::thread> threads;
	std::atomic<int>         completed_pixels{0};
	std::mutex               progress_mutex;

	auto render_rows = [&](int start_row, int end_row, int thread_id) {
		std::random_device rd;

		std::mt19937 gen(rd() + thread_id);

		std::uniform_real_distribution<float> dis(0.0f, 1.0f);

		for (int j = start_row; j < end_row; j++) {
			for (int i = 0; i < scene->width; i++) {
				float   x = (2.f * ((i + 0.5f) / scene->width) - 1.f) * scale * aspect_ratio;
				float   y = (1.f - 2.f * ((j + 0.5f) / scene->height)) * scale;
				vec3f_t ray_direction = vec3f_t(-x, y, 1).normalized();
				vec3f_t pixel_color = vec3f_t::Zero();

				for (int k = 0; k < samples_per_pixel; k++) {
					pixel_color += scene->castRay(Ray(camera_position, ray_direction), 0);
				}

				int pixel_index = j * scene->width + i;
				framebuffer[pixel_index] = pixel_color / samples_per_pixel;

				int current_completed = completed_pixels.fetch_add(1);
				if (current_completed % 1000 == 0) {
					std::lock_guard<std::mutex> lock(progress_mutex);
					std::cout << "\rRendering: " << current_completed / 1000 << "k / " << (scene->width * scene->height) / 1000 << "k pixels" << std::flush;
				}
			}
		}
	};

	int rows_per_thread = scene->height / num_threads;
	for (int t = 0; t < num_threads; t++) {
		int start_row = t * rows_per_thread;
		int end_row = (t == num_threads - 1) ? scene->height : (t + 1) * rows_per_thread;
		threads.emplace_back(render_rows, start_row, end_row, t);
	}

	for (auto& thread : threads)
		thread.join();

	std::cout << std::endl;
}

void Raytracer::save(const std::string& filename)
{
	constexpr float GAMMA = .6f;

	std::ofstream file(filename, std::ios::binary);
	if (!file.is_open())
		throw std::runtime_error("Failed to open file for saving: " + filename);

	file << "P6\n"
	     << scene->width << " " << scene->height << "\n255\n";
	for (const auto& color : framebuffer) {
		unsigned char r = static_cast<unsigned char>(255.f * std::pow(std::clamp(color.x(), 0.f, 1.f), GAMMA));
		unsigned char g = static_cast<unsigned char>(255.f * std::pow(std::clamp(color.y(), 0.f, 1.f), GAMMA));
		unsigned char b = static_cast<unsigned char>(255.f * std::pow(std::clamp(color.z(), 0.f, 1.f), GAMMA));
		file.write(reinterpret_cast<const char*>(&r), sizeof(r));
		file.write(reinterpret_cast<const char*>(&g), sizeof(g));
		file.write(reinterpret_cast<const char*>(&b), sizeof(b));
	}

	file.close();
}
