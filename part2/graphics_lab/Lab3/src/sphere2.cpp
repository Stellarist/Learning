#include <vector>
#include <span>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Shader.hpp"
#include "Camera.hpp"
#include "Light.hpp"

struct ControlState {
	bool    first_mouse{true};
	float   last_x{0.0f};
	float   last_y{0.0f};
	float   delta_time{0.0f};
	float   last_time{0.0f};
	Camera* camera{nullptr};
};

void generateSphere(std::vector<float>& vertices, std::vector<unsigned int>& indices, float radius, int segments = 128, int rings = 64);
void createVertexObjects(std::span<const float> vertices, std::span<const unsigned int> indices, unsigned int& vbo, unsigned int& vao, unsigned int& ibo);
void deleteVertexObjects(unsigned int& vbo, unsigned int& vao, unsigned int& ibo);
auto initGLFW(const std::string& window_title, int width, int height, ControlState& state) -> GLFWwindow*;
void initGLAD();
void processInput(GLFWwindow* window, ControlState& state);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void cursorPosCallback(GLFWwindow* window, double x_pos_in, double y_pos_in);
void scrollCallback(GLFWwindow* window, double x_ofs, double y_ofs);

int main(int argc, char const* argv[])
{
	constexpr unsigned int SCR_WIDTH = 1600;
	constexpr unsigned int SCR_HEIGHT = 900;

	std::vector<float>        sphere_vertices;
	std::vector<unsigned int> sphere_indices;

	Camera       camera(glm::vec3(0.0f, 0.0f, 3.0f));
	ControlState state{.camera = &camera};
	unsigned int sphere_vbo, sphere_vao, sphere_ibo;

	Light dir_light(LightType::DIRECTIONAL, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	Light point_light(LightType::POINT, glm::vec3(2.0f, 2.0f, 2.0f));
	Light spot_light(LightType::SPOT, glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, -1.0f));

	auto* window = initGLFW("lab3", SCR_WIDTH, SCR_HEIGHT, state);
	initGLAD();

	generateSphere(sphere_vertices, sphere_indices, 3.0f);
	createVertexObjects(sphere_vertices, sphere_indices, sphere_vbo, sphere_vao, sphere_ibo);

	Shader shader(SHADER_PATH "/sphere.vert", SHADER_PATH "/sphere.frag");

	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window)) {
		float current_time = static_cast<float>(glfwGetTime());
		state.delta_time = current_time - state.last_time;
		state.last_time = current_time;

		processInput(window, state);

		glClearColor(0.f, 0.f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();

		shader.setMat4("model", glm::scale(glm::mat4(1.0f), glm::vec3(0.1f)));
		shader.setMat4("view", camera.getView());
		shader.setMat4("projection", camera.getProjection());
		shader.setVec3("view_pos", camera.getPosition());
		shader.setVec3("object_color", glm::vec3(0.18f, 0.55f, 0.73f));
		dir_light.applyToShader(shader, "dir_light");
		point_light.applyToShader(shader, "point_light");
		spot_light.applyToShader(shader, "spot_light");

		glBindVertexArray(sphere_vao);
		glDrawElements(GL_TRIANGLES, sphere_indices.size(), GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	deleteVertexObjects(sphere_vbo, sphere_vao, sphere_ibo);

	glfwTerminate();
	return 0;
}

void generateSphere(std::vector<float>& vertices, std::vector<unsigned int>& indices, float radius, int segments, int rings)
{
	constexpr float PI = 3.14159265358979323846f;

	vertices.reserve((segments + 1) * (rings + 1) * 3);
	indices.reserve(segments * rings * 6);

	for (int ring = 0; ring <= rings; ring++) {
		float phi = PI * ring / rings;
		float y = std::cosf(phi);
		float ring_radius = std::sinf(phi);

		for (int segment = 0; segment <= segments; segment++) {
			float theta = 2.0f * PI * segment / segments;
			float x = ring_radius * std::cosf(theta);
			float z = ring_radius * std::sinf(theta);

			vertices.push_back(x * radius);
			vertices.push_back(y * radius);
			vertices.push_back(z * radius);

			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(z);
		}
	}

	for (int ring = 0; ring < rings; ring++) {
		for (int segment = 0; segment < segments; segment++) {
			int current = (ring * (segments + 1)) + segment;
			int next = current + segments + 1;

			indices.push_back(current);
			indices.push_back(next);
			indices.push_back(current + 1);

			indices.push_back(current + 1);
			indices.push_back(next);
			indices.push_back(next + 1);
		}
	}
}

void createVertexObjects(std::span<const float> vertices, std::span<const unsigned int> indices, unsigned int& vbo, unsigned int& vao, unsigned int& ibo)
{
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ibo);
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void deleteVertexObjects(unsigned int& vbo, unsigned int& vao, unsigned int& ibo)
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);
}

GLFWwindow* initGLFW(const std::string& window_title, int width, int height, ControlState& state)
{
	GLFWwindow* window{};
	if (!glfwInit()) {
		std::cerr << "GLFW initialization failed." << std::endl;
		exit(EXIT_FAILURE);
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(width, height, window_title.data(), nullptr, nullptr);
	if (!window) {
		std::cerr << "GLFW window creation failed." << std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	glfwSetWindowUserPointer(window, &state);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetCursorPosCallback(window, cursorPosCallback);
	glfwSetScrollCallback(window, scrollCallback);

	return window;
}

void initGLAD()
{
	if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
		std::cerr << "Failed to initialize GLAD." << std::endl;
		exit(EXIT_FAILURE);
	}
}

void processInput(GLFWwindow* window, ControlState& state)
{
	auto& camera = state.camera;
	auto  time = state.delta_time;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera->processKeyboard(CameraMovement::FORWARD, time);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera->processKeyboard(CameraMovement::BACKWARD, time);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera->processKeyboard(CameraMovement::LEFT, time);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera->processKeyboard(CameraMovement::RIGHT, time);
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void cursorPosCallback(GLFWwindow* window, double x_pos_in, double y_pos_in)
{
	auto* state = static_cast<ControlState*>(glfwGetWindowUserPointer(window));
	auto& [first_mouse, last_x, last_y, _1, _2, camera] = *state;
	float x_pos = (float) x_pos_in;
	float y_pos = (float) y_pos_in;

	if (first_mouse) {
		last_x = x_pos;
		last_y = y_pos;
		first_mouse = false;
	}

	float x_ofs = x_pos - last_x;
	float y_ofs = -(y_pos - last_y);

	last_x = x_pos;
	last_y = y_pos;

	camera->processMouseMovement(x_ofs, y_ofs);
}

void scrollCallback(GLFWwindow* window, double x_ofs, double y_ofs)
{
	auto* state = static_cast<ControlState*>(glfwGetWindowUserPointer(window));
	auto& camera = state->camera;

	camera->processMouseScroll((float) y_ofs);
}
