#include <vector>
#include <span>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Shader.hpp"

void createVertexObjects(std::span<const float> vertices, std::span<const unsigned int> indices, unsigned int& vbo, unsigned int& vao, unsigned int& ibo);
void deleteVertexObjects(unsigned int& vbo, unsigned int& vao, unsigned int& ibo);
auto initGLFW(const std::string& window_title, int width, int height) -> GLFWwindow*;
void initGLAD();
void processInput(GLFWwindow* window);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);

int main(int argc, char const* argv[])
{
	constexpr unsigned int SCR_WIDTH = 800;
	constexpr unsigned int SCR_HEIGHT = 800;

	auto* window = initGLFW("lab1", SCR_WIDTH, SCR_HEIGHT);
	initGLAD();

	std::vector<float> cube_vertices = {
	    // Positions(x, y, z) and Colors (r, g, b)
	    // Front face
	    -0.4f, 0.0f, 0.3f, 1.0f, 0.0f, 0.0f,
	    -0.4f, 0.5f, 0.3f, 1.0f, 1.0f, 0.0f,
	    0.1f, 0.5f, 0.3f, 0.0f, 1.0f, 0.0f,
	    0.1f, 0.0f, 0.3f, 0.0f, 1.0f, 1.0f,

	    // Back face
	    -0.1f, -0.3f, 0.0f, 0.5f, 0.0f, 0.0f,
	    -0.1f, 0.2f, 0.0f, 0.5f, 0.5f, 0.0f,
	    0.4f, 0.2f, 0.0f, 0.0f, 0.5f, 0.0f,
	    0.4f, -0.3f, 0.0f, 0.0f, 0.0f, 1.0f};

	// TODO: define the indices for the cube
	std::vector<unsigned int> cube_indices = {};

	unsigned int cube_vbo, cube_vao, cube_ibo;
	createVertexObjects(cube_vertices, cube_indices, cube_vbo, cube_vao, cube_ibo);

	Shader shader(SHADER_PATH "/cube.vert", SHADER_PATH "/cube.frag");

	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window)) {
		processInput(window);

		glClearColor(1.f, 1.f, 1.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();

		// TODO: bind the VAO and draw the cube

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glBindVertexArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	deleteVertexObjects(cube_vbo, cube_vao, cube_ibo);

	glfwTerminate();
	return 0;
}

// This function generates and binds a VAO, VBO and IBO, sets up the vertex attribute pointers,
// TODO: complete the function to create the cube.
void createVertexObjects(std::span<const float> vertices, std::span<const unsigned int> indices, unsigned int& vbo, unsigned int& vao, unsigned int& ibo)
{
	// Generate and bind a Vertex Array Object (VAO)

	// Bind the vertex buffer object (VBO) to the GL_ARRAY_BUFFER target

	// Bind the element buffer object (IBO) to the GL_ELEMENT_ARRAY_BUFFER target

	// Define the vertex attribute layout

	// Unbind the VBO and VAO
}

// This function deletes the VAO, VBO and IBO to free up resources.
// TODO: complete the function to delete the cube.
void deleteVertexObjects(unsigned int& vbo, unsigned int& vao, unsigned int& ibo)
{
	// delete the VBO, VAO and IBO
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
