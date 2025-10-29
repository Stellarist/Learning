#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "Scene.hpp"

#include <array>
#include <iostream>

#include <tiny_gltf.h>
#include <glad/glad.h>

constexpr std::array border = {1.0f, 1.0f, 1.0f, 1.0f};
constexpr std::array indices = {0, 1, 2, 2, 3, 0};
constexpr std::array vertices = {
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
    1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 0.0f, 0.0f, 1.0f};

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
	shader.setMat4("model", glm::mat4(1.0f));
	shader.setMat4("view", glm::lookAt(position, position + front(), up()));
	shader.setMat4("projection", glm::perspective(glm::radians(fov), aspect, znear, zfar));
	shader.setVec3("camera_pos", position);
}

void Light::upload(Shader& shader)
{
	shader.setVec3("light.direction", direction);
	shader.setVec3("light.color", color);
	shader.setFloat("light.ambient", ambient);
	shader.setFloat("light.diffuse", diffuse);
	shader.setFloat("light.specular", specular);
}

void Mesh::upload(Shader& shader)
{
	shader.setMat4("model", glm::translate(glm::mat4(1.0f), position) *
	                            glm::mat4_cast(rotation) *
	                            glm::scale(glm::mat4(1.0f), scale));
	shader.setVec3("object_color", glm::vec3(1.0f, 1.0f, 1.0f));
}

void ShadowMap::upload(Shader& shader)
{
	// TODO: Create projection and view matrices based on directional light
	shader.setMat4("light_space", projection * view);
	shader.setFloat("light_size", light_size);
	shader.setInt("blocker_search_radius", blocker_search_radius);
	shader.setInt("shadow_sampling_radius", shadow_sampling_radius);
}

Scene& Scene::instance()
{
	static Scene instance;
	return instance;
}

void Scene::renderScene(Shader& shader)
{
	shader.use();
	shader.setInt("shadow_map", 0);
	shader.setBool("PCF_or_PCSS", PCF_or_PCSS);
	camera.upload(shader);
	light.upload(shader);
	shadow_map.upload(shader);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, shadow_map.texture);
	render(shader);
}

void Scene::renderShadow(Shader& shader)
{
	shader.use();
	shadow_map.upload(shader);
	glBindFramebuffer(GL_FRAMEBUFFER, shadow_map.fbo);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	render(shader);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Scene::renderShadowMap(Shader& shader)
{
	shader.use();
	shader.setInt("shadow_map", 0);
	shadow_map.upload(shader);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, shadow_map.texture);
	glBindVertexArray(shadow_map.vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Scene::render(Shader& shader)
{
	for (auto& mesh : meshes) {
		mesh.upload(shader);
		glBindVertexArray(mesh.vao);
		glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
}

void Scene::createMeshes()
{
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

void Scene::deleteMeshes()
{
	for (auto& mesh : meshes) {
		glDeleteVertexArrays(1, &mesh.vao);
		glDeleteBuffers(1, &mesh.vbo);
		glDeleteBuffers(1, &mesh.ibo);
	}
}

void Scene::createShadowMap(unsigned int width, unsigned int height)
{
	glGenFramebuffers(1, &shadow_map.fbo);
	glGenTextures(1, &shadow_map.texture);

	glBindTexture(GL_TEXTURE_2D, shadow_map.texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border.data());

	glBindFramebuffer(GL_FRAMEBUFFER, shadow_map.fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadow_map.texture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenVertexArrays(1, &shadow_map.vao);
	glGenBuffers(1, &shadow_map.vbo);
	glGenBuffers(1, &shadow_map.ibo);

	glBindVertexArray(shadow_map.vao);
	glBindBuffer(GL_ARRAY_BUFFER, shadow_map.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shadow_map.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

void Scene::deleteShadowMap()
{
	glDeleteFramebuffers(1, &shadow_map.fbo);
	glDeleteTextures(1, &shadow_map.texture);
	glDeleteVertexArrays(1, &shadow_map.vao);
	glDeleteBuffers(1, &shadow_map.vbo);
	glDeleteBuffers(1, &shadow_map.ibo);
}

void Scene::loadGltf(std::string_view file_path)
{
	tinygltf::TinyGLTF loader;
	tinygltf::Model    model;
	std::string        error, warn;

	bool ret = loader.LoadASCIIFromFile(&model, &error, &warn, file_path.data());
	if (!error.empty())
		std::cout << "Error: " << error << std::endl;
	if (!warn.empty())
		std::cout << "Warn: " << warn << std::endl;
	if (!ret)
		throw std::runtime_error("Failed to load glTF file.");
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
			    .name = gltfmesh.name,
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
