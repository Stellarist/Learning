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

		// TODO: bind the VAO and draw the triangle

		glBindVertexArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	deleteVertexObjects(triangle_vbo, triangle_vao);

	glfwTerminate();
	return 0;
}

// This function generates and binds a VAO and VBO, sets up the vertex attribute pointers,
// TODO: complete the function to create the triangle.
void createVertexObjects(std::span<const float> vertices, unsigned int& vbo, unsigned int& vao)
{
	// Generate and bind a Vertex Array Object (VAO)

	// Bind the vertex buffer object (VBO) to the GL_ARRAY_BUFFER target

	// Define the vertex attribute layout

	// Unbind the VBO and VAO
}

// This function deletes the VAO and VBO to free up resources.
// TODO: complete the function to delete the triangle.
void deleteVertexObjects(unsigned int& vbo, unsigned int& vao)
{
	// Delete the VBO and VAO
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
