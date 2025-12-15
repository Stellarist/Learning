#pragma once

#include <vector>
#include <string_view>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Shader.hpp"

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
};

struct Light {        // Directional light
	glm::vec3 direction{};
	glm::vec3 color{0.8f};

	float ambient{0.1f};
	float diffuse{0.8f};
	float specular{0.5f};

	void upload(Shader& shader);
};

struct Camera {
	glm::vec3 position{};
	glm::vec3 rotation{};

	bool  rotatable{true};
	float fov{45.0f};
	float aspect{1.77f};
	float znear{0.1f};
	float zfar{100.0f};

	glm::vec3 right() const;
	glm::vec3 up() const;
	glm::vec3 front() const;

	void upload(Shader& shader);
};

using vertex_t = Vertex;
using index_t = unsigned int;

struct Mesh {
	unsigned int vao{}, vbo{}, ibo{};
	std::string  name;

	glm::vec3 position{};
	glm::quat rotation{};
	glm::vec3 scale{1.0f};

	std::vector<vertex_t> vertices;
	std::vector<index_t>  indices;

	void upload(Shader& shader);
};

struct ShadowMap {
	unsigned int vao{}, vbo{}, ibo{};
	unsigned int fbo{}, texture{};

	float light_size{0.1f};
	int   blocker_search_radius{3};
	int   shadow_sampling_radius{2};

	void upload(Shader& shader);
};

class Scene {
private:
	Scene() = default;

public:
	Camera    camera;
	Light     light;
	ShadowMap shadow_map;

	bool show_shadow_map{false};        // true to show shadow map, false to render scene
	bool PCF_or_PCSS{true};             // true for PCF, false for PCSS

	std::vector<Mesh> meshes;

	static Scene& instance();

	void loadGltf(std::string_view file_path);

	void createMeshes();
	void deleteMeshes();
	void createShadowMap(unsigned int width, unsigned int height);
	void deleteShadowMap();

	void renderScene(Shader& shader);
	void renderShadow(Shader& shader);
	void renderShadowMap(Shader& shader);
	void render(Shader& shader);
};
