#include <array>
#include <span>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Shader.hpp"

void createVertexObjects(std::span<const float> vertices, unsigned int& vbo, unsigned int& vao);
void deleteVertexObjects(unsigned int& vbo, unsigned int& vao);
auto initGLFW(const std::string& window_title, int width, int height) -> GLFWwindow*;
void initGLAD();
void processInput(GLFWwindow* window);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);

int main(int argc, char const* argv[])
{
	constexpr unsigned int SCR_WIDTH = 800;
	constexpr unsigned int SCR_HEIGHT = 800;

	constexpr std::array triangle_vertices = {
	    -0.5f, -0.5f, 0.0f,        // left
	    0.5f, -0.5f, 0.0f,         // right
	    0.0f, 0.5f, 0.0f           // top
	};

	auto* window = initGLFW("lab1", SCR_WIDTH, SCR_HEIGHT);
	initGLAD();

	unsigned int triangle_vbo, triangle_vao;
	createVertexObjects(triangle_vertices, triangle_vbo, triangle_vao);

	Shader shader(SHADER_PATH "/triangle.vert", SHADER_PATH "/triangle.frag");

	while (!glfwWindowShouldClose(window)) {
		processInput(window);

		glClearColor(1.f, 1.f, 1.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		shader.use();

		glBindVertexArray(triangle_vao);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	deleteVertexObjects(triangle_vbo, triangle_vao);

	glfwTerminate();
	return 0;
}

// This function generates and binds a VAO and VBO, sets up the vertex attribute pointers,
void createVertexObjects(std::span<const float> vertices, unsigned int& vbo, unsigned int& vao)
{
	// Generate and bind a Vertex Array Object (VAO)
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);

	// Bind the vertex buffer object (VBO) to the GL_ARRAY_BUFFER target
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	// Define the vertex attribute layout
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);

	// Unbind the VBO and VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

// This function deletes the VAO and VBO to free up resources.
void deleteVertexObjects(unsigned int& vbo, unsigned int& vao)
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
}

// This function initializes GLFW, creates a window, and sets the framebuffer size callback.
GLFWwindow* initGLFW(const std::string& window_title, int width, int height)
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
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	return window;
}

// This function loads all OpenGL function pointers using GLAD and checks for errors.
void initGLAD()
{
	if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
		std::cerr << "Failed to initialize GLAD." << std::endl;
		exit(EXIT_FAILURE);
	}
}

// This function checks if the ESC key is pressed and sets the window to close if it is.
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

// This function sets the viewport to match the new window dimensions.
void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
