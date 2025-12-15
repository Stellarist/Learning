#include <array>
#include <span>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.hpp"

struct Transform {
	glm::vec3 translation{0.0f, 0.0f, 0.0f};
	glm::vec3 rotation{0.0f, 0.0f, 0.0f};
	glm::vec3 scale{1.0f, 1.0f, 1.0f};
};

struct ControlState {
	bool      first_mouse{true};                        // Flag to check if it's the first mouse movement
	glm::vec2 delta_key_pos{glm::vec2(0.0f)};           // WASD to translate object
	glm::vec2 delta_cursor_pos{glm::vec2(0.0f)};        // Cursor movement to rotate object
	glm::vec2 last_cursor_pos{glm::vec2(0.0f)};         // Last cursor position for rotation
	float     zoom{1.0f};                               // Zoom level for scaling the object
};

void createVertexObjects(std::span<const float> vertices, unsigned int& vbo, unsigned int& vao);
void deleteVertexObjects(unsigned int& vbo, unsigned int& vao);
auto initGLFW(const std::string& window_title, int width, int height, void* state) -> GLFWwindow*;
void initGLAD();
void processInput(GLFWwindow* window, ControlState& state);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void cursorPosCallback(GLFWwindow* window, double x_pos_in, double y_pos_in);
void scrollCallback(GLFWwindow* window, double x_ofs, double y_ofs);
auto calculateModel(Transform& transform, ControlState& state) -> glm::mat4;

int main(int argc, char const* argv[])
{
	constexpr unsigned int SCR_WIDTH = 800;
	constexpr unsigned int SCR_HEIGHT = 800;
	constexpr std::array   pyramid_vertices = {
        // Face 1: Base (green)
        -0.4330f, -0.1667f, 0.25f, 0.0f, 1.0f, 0.0f,        // v1
        0.4330f, -0.1667f, 0.25f, 0.0f, 1.0f, 0.0f,         // v2
        0.0f, -0.1667f, -0.5f, 0.0f, 1.0f, 0.0f,            // v3

        // Face 2: Side 1 (red)
        0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f,                 // v0
        -0.4330f, -0.1667f, 0.25f, 1.0f, 0.0f, 0.0f,        // v1
        0.4330f, -0.1667f, 0.25f, 1.0f, 0.0f, 0.0f,         // v2

        // Face 3: Side 2 (blue)
        0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f,                 // v0
        0.0f, -0.1667f, -0.5f, 0.0f, 0.0f, 1.0f,            // v3
        -0.4330f, -0.1667f, 0.25f, 1.0f, 0.0f, 0.0f,        // v1

        // Face 4: Side 3 (yellow)
        0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f,                // v0
        0.4330f, -0.1667f, 0.25f, 1.0f, 1.0f, 0.0f,        // v2
        0.0f, -0.1667f, -0.5f, 0.0f, 0.0f, 1.0f,           // v3
    };

	unsigned int pyramid_vbo, pyramid_vao;
	Transform    pyramid_transform{};
	ControlState control_state{};

	auto* window = initGLFW("lab2", SCR_WIDTH, SCR_HEIGHT, &control_state);
	initGLAD();

	createVertexObjects(pyramid_vertices, pyramid_vbo, pyramid_vao);

	Shader shader(SHADER_PATH "/pyramid.vert", SHADER_PATH "/pyramid.frag");

	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window)) {
		processInput(window, control_state);

		glClearColor(1.f, 1.f, 1.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();

		shader.setMat4("model", calculateModel(pyramid_transform, control_state));

		glBindVertexArray(pyramid_vao);
		glDrawArrays(GL_TRIANGLES, 0, pyramid_vertices.size() / 6);
		glBindVertexArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	deleteVertexObjects(pyramid_vbo, pyramid_vao);

	glfwTerminate();
	return 0;
}

glm::mat4 calculateModel(Transform& transform, ControlState& state)
{
	// Calculate the model matrix based on the current transform and control state
	auto& [translation, rotation, scale] = transform;
	auto old_translation = translation;
	auto delta_translation = glm::vec3(state.delta_key_pos, 0.0f);
	auto delta_rotation = glm::vec3(state.delta_cursor_pos, 0.0f);
	auto new_scale = glm::vec3(state.zoom);

	// Reset the state for the next frame
	state.delta_key_pos = glm::vec2(0.0f);
	state.delta_cursor_pos = glm::vec2(0.0f);

	// Update translation, rotation, and scale based on input
	translation += delta_translation;
	rotation += delta_rotation;
	scale = new_scale;

	// Calculate the model matrix
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, translation);
	model = glm::rotate(model, glm::radians(-rotation.y), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(-rotation.x), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, scale);

	return model;
}

void createVertexObjects(std::span<const float> vertices, unsigned int& vbo, unsigned int& vao)
{
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void deleteVertexObjects(unsigned int& vbo, unsigned int& vao)
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
}

GLFWwindow* initGLFW(const std::string& window_title, int width, int height, void* state)
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
	glfwSetWindowUserPointer(window, state);
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

// This function processes input from the keyboard to control the object movement
void processInput(GLFWwindow* window, ControlState& state)
{
	constexpr float sensitivity = 5e-4f;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		state.delta_key_pos += glm::vec2(0.0f, 1.0f * sensitivity);        // Move object forward
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		state.delta_key_pos += glm::vec2(0.0f, -1.0f * sensitivity);        // Move object backward
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		state.delta_key_pos += glm::vec2(-1.0f * sensitivity, 0.0f);        // Move object left
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		state.delta_key_pos += glm::vec2(1.0f * sensitivity, 0.0f);        // Move object right
}

// This function is called whenever the window is resized to adjust the viewport
void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// This function is called whenever the mouse is moved to update the rotation of the object
void cursorPosCallback(GLFWwindow* window, double x_pos_in, double y_pos_in)
{
	auto* state = static_cast<ControlState*>(glfwGetWindowUserPointer(window));
	float x_pos = static_cast<float>(x_pos_in);
	float y_pos = static_cast<float>(y_pos_in);

	if (state->first_mouse) {
		state->last_cursor_pos = glm::vec2(x_pos, y_pos);
		state->first_mouse = false;
	}

	constexpr float sensitivity = 0.5f;
	state->delta_cursor_pos = glm::vec2(x_pos, y_pos) - state->last_cursor_pos;
	state->delta_cursor_pos *= sensitivity;
	state->last_cursor_pos = glm::vec2(x_pos, y_pos);
}

// This function is called whenever the mouse wheel is scrolled to update the zoom level
void scrollCallback(GLFWwindow* window, double x_ofs, double y_ofs)
{
	auto* state = static_cast<ControlState*>(glfwGetWindowUserPointer(window));
	float sensitivity = std::log(1.0f - y_ofs * 0.1f);

	state->zoom = glm::clamp(state->zoom - sensitivity, 0.5f, 2.0f);
}
