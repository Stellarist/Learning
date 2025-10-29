#include <vector>
#include <span>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Camera.hpp"
#include "Shader.hpp"

struct ControlState {
	bool    first_mouse{true};
	float   last_x{0.0f};
	float   last_y{0.0f};
	float   delta_time{0.0f};
	float   last_time{0.0f};
	Camera* camera{nullptr};
};

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

	std::vector<float> cube_vertices = {
	    // Position           // Color
	    -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,        // 0: bottom-left-back
	    0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,         // 1: bottom-right-back
	    0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 1.0f,          // 2: top-right-back
	    -0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f,         // 3: top-left-back
	    -0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 1.0f,         // 4: bottom-left-front
	    0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 1.0f,          // 5: bottom-right-front
	    0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f,           // 6: top-right-front
	    -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f           // 7: top-left-front
	};

	std::vector<unsigned int> cube_indices = {
	    // Front face
	    4, 5, 6, 6, 7, 4,
	    // Back face
	    0, 1, 2, 2, 3, 0,
	    // Left face
	    0, 4, 7, 7, 3, 0,
	    // Right face
	    1, 5, 6, 6, 2, 1,
	    // Top face
	    3, 7, 6, 6, 2, 3,
	    // Bottom face
	    0, 4, 5, 5, 1, 0};

	Camera       camera(glm::vec3(0.0f, 0.0f, 3.0f));
	ControlState state{.camera = &camera};

	auto* window = initGLFW("lab2", SCR_WIDTH, SCR_HEIGHT, state);
	initGLAD();

	unsigned int cube_vbo, cube_vao, cube_ibo;
	createVertexObjects(cube_vertices, cube_indices, cube_vbo, cube_vao, cube_ibo);

	Shader shader(SHADER_PATH "/cube.vert", SHADER_PATH "/cube.frag");

	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window)) {
		float current_time = static_cast<float>(glfwGetTime());
		state.delta_time = current_time - state.last_time;
		state.last_time = current_time;

		processInput(window, state);

		glClearColor(1.f, 1.f, 1.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();

		shader.setMat4("model", glm::mat4(1.0f));
		shader.setMat4("view", camera.getView());
		shader.setMat4("projection", camera.getProjection());

		glBindVertexArray(cube_vao);
		glDrawElements(GL_TRIANGLES, cube_indices.size(), GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	deleteVertexObjects(cube_vbo, cube_vao, cube_ibo);

	glfwTerminate();
	return 0;
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
