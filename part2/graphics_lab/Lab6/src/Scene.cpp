#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "Scene.hpp"

#include <array>
#include <iostream>

#include <glad/glad.h>
#include <tiny_gltf.h>

extern constexpr unsigned int SCR_WIDTH = 1920;
extern constexpr unsigned int SCR_HEIGHT = 1080;
static constexpr unsigned int SKYBOX_SIZE = 1024;
static constexpr unsigned int IRRADIANCE_MAP_SIZE = 64;
static constexpr unsigned int PREFILTER_MAP_SIZE = 128;

constexpr std::array<float, 24> cube_vertices = {
    -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, -1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f};

constexpr std::array<unsigned int, 36> cube_indices = {
    0, 1, 2, 2, 3, 0,
    4, 5, 6, 6, 7, 4,
    0, 1, 5, 5, 4, 0,
    2, 3, 7, 7, 6, 2,
    0, 3, 7, 7, 4, 0,
    1, 2, 6, 6, 5, 1};

glm::mat4 capture_projection = glm::perspective(
    glm::radians(90.0f), 1.0f, 0.1f, 10.0f);

glm::mat4 capture_views[] = {
    glm::lookAt(glm::vec3(0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),         // +X
    glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),        // -X
    glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),          // +Y
    glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),        // -Y
    glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),         // +Z
    glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))         // -Z
};

glm::vec3 Camera::right() const
{
	return glm::normalize(glm::cross(front(), glm::vec3(0.0f, 1.0f, 0.0f)));
}

glm::vec3 Camera::up() const
{
	return glm::normalize(glm::cross(right(), front()));
}

glm::vec3 Camera::front() const
{
	float pitch = glm::radians(rotation.x);
	float yaw = glm::radians(rotation.y);
	return glm::normalize(glm::vec3(cos(pitch) * cos(yaw), sin(pitch), cos(pitch) * sin(yaw)));
}

void Camera::upload(Shader& shader)
{
	shader.setMat4("view", glm::lookAt(position, position + front(), up()));
	shader.setMat4("projection", glm::perspective(glm::radians(fov), aspect, znear, zfar));
	shader.setVec3("camera_pos", position);
}

void Mesh::upload(Shader& shader)
{
	shader.setMat4("model", glm::translate(glm::mat4(1.0f), position) *
	                            glm::mat4_cast(rotation) *
	                            glm::scale(glm::mat4(1.0f), scale));
	shader.setVec3("albedo", albedo);
	shader.setFloat("metallic", metallic);
	shader.setFloat("roughness", roughness);
}

Scene& Scene::instance()
{
	static Scene instance;
	return instance;
}

void Scene::generateSkyboxCubemap(Shader& shader)
{
	shader.use();
	shader.setInt("environment_map", 0);
	shader.setMat4("projection", capture_projection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, hdr_texture);
	glViewport(0, 0, SKYBOX_SIZE, SKYBOX_SIZE);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	for (unsigned int i = 0; i < 6; ++i) {
		shader.setMat4("view", capture_views[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, skybox_texture, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		renderCube(shader);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_texture);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}

void Scene::generateIrradianceMap(Shader& shader)
{
	glGenTextures(1, &irradiance_map);
	glBindTexture(GL_TEXTURE_CUBE_MAP, irradiance_map);
	for (int i = 0; i < 6; i++)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, IRRADIANCE_MAP_SIZE, IRRADIANCE_MAP_SIZE, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	shader.use();
	shader.setInt("environment_map", 0);
	shader.setMat4("projection", capture_projection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_texture);
	glViewport(0, 0, IRRADIANCE_MAP_SIZE, IRRADIANCE_MAP_SIZE);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	for (unsigned int i = 0; i < 6; i++) {
		shader.setMat4("view", capture_views[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradiance_map, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		renderCube(shader);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
}

void Scene::generatePrefilterMap(Shader& shader)
{
	glGenTextures(1, &prefilter_map);
	glBindTexture(GL_TEXTURE_CUBE_MAP, prefilter_map);
	for (int i = 0; i < 6; i++)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, PREFILTER_MAP_SIZE, PREFILTER_MAP_SIZE, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	shader.use();
	shader.setInt("environment_map", 0);
	shader.setMat4("projection", capture_projection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_texture);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	unsigned int max_mip_levels = 5;
	for (unsigned int mip = 0; mip < max_mip_levels; mip++) {
		unsigned int mip_width = PREFILTER_MAP_SIZE * std::pow(0.5, mip);
		unsigned int mip_height = PREFILTER_MAP_SIZE * std::pow(0.5, mip);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mip_width, mip_height);
		glViewport(0, 0, mip_width, mip_height);
		shader.setFloat("roughness", (float) mip / (float) max_mip_levels);

		for (unsigned int i = 0; i < 6; ++i) {
			shader.setMat4("view", capture_views[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilter_map, mip);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			renderCube(shader);
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
}

void Scene::renderCube(Shader& shader)
{
	glBindVertexArray(cube_vao);
	glDrawElements(GL_TRIANGLES, cube_indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Scene::renderMeshes(Shader& shader)
{
	for (auto& mesh : meshes) {
		mesh.upload(shader);
		glBindVertexArray(mesh.vao);
		glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
}

void Scene::generate(Shader& cshader, Shader& ishader, Shader& pshader)
{
	generateSkyboxCubemap(cshader);
	generateIrradianceMap(ishader);
	generatePrefilterMap(pshader);
}

void Scene::render(Shader& eshader, Shader& sshader)
{
	eshader.use();
	eshader.setInt("environment_map", 0);
	camera.upload(eshader);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_texture);
	renderCube(eshader);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);

	sshader.use();
	camera.upload(sshader);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, irradiance_map);
	sshader.setInt("irradiance_map", 1);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, prefilter_map);
	sshader.setInt("prefilter_map", 2);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, brdf_lut);
	sshader.setInt("brdf_lut", 3);
	renderMeshes(sshader);
}

void Scene::create()
{
	glGenFramebuffers(1, &fbo);
	glGenRenderbuffers(1, &rbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, SKYBOX_SIZE, SKYBOX_SIZE);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

	glGenVertexArrays(1, &cube_vao);
	glGenBuffers(1, &cube_vbo);
	glGenBuffers(1, &cube_ibo);
	glBindVertexArray(cube_vao);
	glBindBuffer(GL_ARRAY_BUFFER, cube_vbo);
	glBufferData(GL_ARRAY_BUFFER, cube_vertices.size() * sizeof(float), cube_vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, cube_indices.size() * sizeof(unsigned int), cube_indices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	for (auto& mesh : meshes) {
		glGenVertexArrays(1, &mesh.vao);
		glGenBuffers(1, &mesh.vbo);
		glGenBuffers(1, &mesh.ibo);
		glBindVertexArray(mesh.vao);
		glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
		glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(vertex_t), mesh.vertices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(index_t), mesh.indices.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*) offsetof(vertex_t, position));
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*) offsetof(vertex_t, normal));
		glEnableVertexAttribArray(1);
		glBindVertexArray(0);
	}
}

void Scene::destroy()
{
	glDeleteFramebuffers(1, &fbo);
	glDeleteRenderbuffers(1, &rbo);

	glDeleteVertexArrays(1, &cube_vao);
	glDeleteBuffers(1, &cube_vbo);
	glDeleteBuffers(1, &cube_ibo);

	glDeleteVertexArrays(1, &quad_vao);
	glDeleteBuffers(1, &quad_vbo);
	glDeleteBuffers(1, &quad_ibo);

	glDeleteTextures(1, &hdr_texture);
	glDeleteTextures(1, &skybox_texture);
	glDeleteTextures(1, &irradiance_map);
	glDeleteTextures(1, &prefilter_map);
	glDeleteTextures(1, &brdf_lut);

	for (auto& mesh : meshes) {
		glDeleteVertexArrays(1, &mesh.vao);
		glDeleteBuffers(1, &mesh.vbo);
		glDeleteBuffers(1, &mesh.ibo);
	}
}

void Scene::loadSkybox(std::string_view file_path)
{
	int width, height, channels;
	stbi_set_flip_vertically_on_load(true);
	auto* data = stbi_load(file_path.data(), &width, &height, &channels, 0);
	if (!data) {
		std::cerr << "Failed to load Skybox texture: " << file_path << std::endl;
		return;
	}
	std::cout << "Loaded texture file: " << file_path << std::endl;

	glGenTextures(1, &hdr_texture);
	glBindTexture(GL_TEXTURE_2D, hdr_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	stbi_image_free(data);

	glGenTextures(1, &skybox_texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_texture);
	for (unsigned int i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB8, SKYBOX_SIZE, SKYBOX_SIZE, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Scene::loadBrdfLut(std::string_view file_path)
{
	int width, height, channels;
	stbi_set_flip_vertically_on_load(true);
	auto* data = stbi_load(file_path.data(), &width, &height, &channels, 0);
	if (!data) {
		std::cerr << "Failed to load Brdf Lut texture: " << file_path << std::endl;
		return;
	}
	std::cout << "Loaded texture file: " << file_path << std::endl;

	glGenTextures(1, &brdf_lut);
	glBindTexture(GL_TEXTURE_2D, brdf_lut);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	stbi_image_free(data);
}

void Scene::loadScene(std::string_view file_path)
{
	tinygltf::TinyGLTF loader;
	tinygltf::Model    model;
	std::string        error, warn;

	bool ret = loader.LoadBinaryFromFile(&model, &error, &warn, file_path.data());
	if (!error.empty())
		std::cout << "Error: " << error << std::endl;
	if (!warn.empty())
		std::cout << "Warn: " << warn << std::endl;
	if (!ret)
		throw std::runtime_error("Failed to load scene file.");
	std::cout << "Loaded glTF file: " << file_path << std::endl;

	const auto& scene = model.scenes.front();
	for (int id : scene.nodes) {
		const auto& node = model.nodes[id];
		if (node.mesh < 0)
			continue;

		const auto& gltfmesh = model.meshes[node.mesh];
		for (const auto& primitive : gltfmesh.primitives) {
			const auto &t = node.translation, r = node.rotation, s = node.scale;

			Mesh mesh{
			    .position = t.size() > 0 ? glm::vec3(t[0], t[1], t[2]) : glm::vec3(0.0f),
			    .rotation = r.size() > 0 ? glm::quat(r[0], r[1], r[2], r[3]) : glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
			    .scale = s.size() > 0 ? glm::vec3(s[0], s[1], s[2]) : glm::vec3(1.0f),
			};

			const float* positions = nullptr;
			if (primitive.attributes.count("POSITION")) {
				const auto& vtaccessor = model.accessors[primitive.attributes.at("POSITION")];
				const auto& vtview = model.bufferViews[vtaccessor.bufferView];
				const auto& vtbuffer = model.buffers[vtview.buffer];
				positions = reinterpret_cast<const float*>(vtbuffer.data.data() + vtview.byteOffset + vtaccessor.byteOffset);

				const float* normals = nullptr;
				if (primitive.attributes.count("NORMAL")) {
					const auto& nraccessor = model.accessors[primitive.attributes.at("NORMAL")];
					const auto& nrview = model.bufferViews[nraccessor.bufferView];
					const auto& nrbuffer = model.buffers[nrview.buffer];
					normals = reinterpret_cast<const float*>(nrbuffer.data.data() + nrview.byteOffset + nraccessor.byteOffset);
				}

				mesh.vertices.resize(vtaccessor.count);
				for (size_t i = 0; i < vtaccessor.count; i++) {
					vertex_t& vertex = mesh.vertices[i];
					vertex.position = glm::vec3(positions[i * 3], positions[i * 3 + 1], positions[i * 3 + 2]);
					if (normals)
						vertex.normal = glm::vec3(normals[i * 3], normals[i * 3 + 1], normals[i * 3 + 2]);
				}
			}

			if (primitive.indices >= 0) {
				const auto&    idaccessor = model.accessors[primitive.indices];
				const auto&    idview = model.bufferViews[idaccessor.bufferView];
				const auto&    idbuffer = model.buffers[idview.buffer];
				const uint8_t* indices = reinterpret_cast<const uint8_t*>(idbuffer.data.data() + idview.byteOffset + idaccessor.byteOffset);

				mesh.indices.resize(idaccessor.count);
				if (idaccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
					const unsigned int* iddata = reinterpret_cast<const unsigned int*>(indices);
					for (size_t i = 0; i < idaccessor.count; i++) {
						mesh.indices[i] = iddata[i];
					}
				} else if (idaccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
					const unsigned short* iddata = reinterpret_cast<const unsigned short*>(indices);
					for (size_t i = 0; i < idaccessor.count; i++) {
						mesh.indices[i] = static_cast<unsigned int>(iddata[i]);
					}
				} else if (idaccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
					const unsigned char* iddata = reinterpret_cast<const unsigned char*>(indices);
					for (size_t i = 0; i < idaccessor.count; i++) {
						mesh.indices[i] = static_cast<unsigned int>(iddata[i]);
					}
				}
			}

			meshes.emplace_back(std::move(mesh));
		}
	}
}
