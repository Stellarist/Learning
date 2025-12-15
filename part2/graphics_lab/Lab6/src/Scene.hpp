#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Shader.hpp"

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
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

	glm::vec3 position{};
	glm::quat rotation{};
	glm::vec3 scale{1.0f};

	glm::vec3 albedo{0.8f};
	float     metallic{0.0f};
	float     roughness{0.5f};

	std::vector<vertex_t> vertices;
	std::vector<index_t>  indices;

	void upload(Shader& shader);
};

class Scene {
private:
	unsigned int fbo{};
	unsigned int rbo{};

	unsigned int cube_vao{};
	unsigned int cube_vbo{};
	unsigned int cube_ibo{};

	unsigned int quad_vao{};
	unsigned int quad_vbo{};
	unsigned int quad_ibo{};

	unsigned int hdr_texture{};
	unsigned int skybox_texture{};
	unsigned int irradiance_map{};
	unsigned int prefilter_map{};
	unsigned int brdf_lut{};

	Scene() = default;

	void generateSkyboxCubemap(Shader& shader);
	void generateIrradianceMap(Shader& shader);
	void generatePrefilterMap(Shader& shader);

	void renderCube(Shader& shader);
	void renderMeshes(Shader& shader);

public:
	Camera camera;

	std::vector<Mesh> meshes;

	static Scene& instance();

	void loadSkybox(std::string_view file_path);
	void loadBrdfLut(std::string_view file_path);
	void loadScene(std::string_view file_path);

	void generate(Shader& cshader, Shader& ishader, Shader& pshader);
	void render(Shader& eshader, Shader& sshader);
	void create();
	void destroy();
};
