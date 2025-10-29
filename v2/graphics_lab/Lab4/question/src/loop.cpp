#include <vector>
#include <string>
#include <span>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "Shader.hpp"
#include "Mesh.hpp"

void loopSubdivision(std::vector<vertex_t>& out_vertices, std::vector<index_t>& out_indices,
                     std::span<const vertex_t> in_vertices, std::span<const index_t> in_indices);
void createVertexObjects(std::span<const vertex_t> vertices, std::span<const index_t> indices, unsigned int& vbo, unsigned int& vao, unsigned int& ibo);
void deleteVertexObjects(unsigned int& vbo, unsigned int& vao, unsigned int& ibo);
void updateVertexObjects(std::span<const vertex_t> vertices, std::span<const index_t> indices, unsigned int vbo, unsigned int ibo);
auto initGLFW(const std::string& window_title, int width, int height) -> GLFWwindow*;
void initGLAD();
void initImgui(GLFWwindow* window);
void destroyImgui();
void flushImgui(unsigned int vbo, unsigned int ibo, int& last_loop, int& current_loop,
                const std::vector<std::vector<vertex_t>>& loop_vertices, const std::vector<std::vector<index_t>>& loop_indices);
void renderImgui();
void processInput(GLFWwindow* window);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);

constexpr unsigned int SCR_WIDTH = 1920;
constexpr unsigned int SCR_HEIGHT = 1080;
constexpr unsigned int LOOP_MAX_LEVELS = 5;

int main(int argc, char const* argv[])
{
	auto window = initGLFW("lab4", SCR_WIDTH, SCR_HEIGHT);
	initGLAD();
	initImgui(window);

	unsigned int loop_vbo, loop_vao, loop_ibo;

	int last_loop_level{0}, current_loop_level{0};

	std::vector<std::vector<vertex_t>> loop_vertices(LOOP_MAX_LEVELS);
	std::vector<std::vector<index_t>>  loop_indices(LOOP_MAX_LEVELS);

	Mesh::loadObj(ASSET_PATH "/bunny.obj", loop_vertices.front(), loop_indices.front());
	for (int i = 1; i < LOOP_MAX_LEVELS; i++)
		loopSubdivision(loop_vertices[i], loop_indices[i], loop_vertices[i - 1], loop_indices[i - 1]);

	createVertexObjects(loop_vertices.front(), loop_indices.front(), loop_vbo, loop_vao, loop_ibo);

	Shader shader(SHADER_PATH "/loop.vert", SHADER_PATH "/loop.frag");

	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window)) {
		processInput(window);

		glClearColor(0.f, 0.f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		flushImgui(loop_vbo, loop_ibo, last_loop_level, current_loop_level, loop_vertices, loop_indices);

		shader.use();
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-.25f, -.5f, -1.5f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.f, 0.f, 0.f));
		model = glm::scale(model, glm::vec3(0.01f));
		shader.setMat4("model", model);
		shader.setMat4("view", glm::mat4(1.0f));
		shader.setMat4("projection", glm::perspective(glm::radians(45.0f), (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f, 100.0f));
		shader.setVec3("light_dir", glm::vec3(0.f, 0.f, -1.f));
		shader.setVec3("light_color", glm::vec3(1.f, 1.f, 1.f));
		shader.setVec3("object_color", glm::vec3(1.f, 1.f, 1.f));

		glBindVertexArray(loop_vao);
		glDrawElements(GL_TRIANGLES, loop_indices.at(current_loop_level).size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		renderImgui();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	deleteVertexObjects(loop_vbo, loop_vao, loop_ibo);

	destroyImgui();
	glfwTerminate();

	return 0;
}

// TODO: Implement Loop Subdivision Algorithm
void loopSubdivision(std::vector<vertex_t>& out_vertices, std::vector<index_t>& out_indices,
                     std::span<const vertex_t> in_vertices, std::span<const index_t> in_indices)
{
	// Copy original vertices and update positions based on the old vertex rule

	// Construct new edge points based on the edge-to-triangle mapping

	// Recalculate indices for the new triangles
}

void createVertexObjects(std::span<const vertex_t> vertices, std::span<const index_t> indices, unsigned int& vbo, unsigned int& vao, unsigned int& ibo)
{
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ibo);
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex_t), vertices.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*) 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void deleteVertexObjects(unsigned int& vbo, unsigned int& vao, unsigned int& ibo)
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);
}

void updateVertexObjects(std::span<const vertex_t> vertices, std::span<const index_t> indices, unsigned int vbo, unsigned int ibo)
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex_t), vertices.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(index_t), indices.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

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

void initGLAD()
{
	if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
		std::cerr << "Failed to initialize GLAD." << std::endl;
		exit(EXIT_FAILURE);
	}
}

void initImgui(GLFWwindow* window)
{
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 450");
}

void destroyImgui()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void flushImgui(unsigned int vbo, unsigned int ibo, int& last_loop, int& current_loop,
                const std::vector<std::vector<vertex_t>>& loop_vertices, const std::vector<std::vector<index_t>>& loop_indices)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();

	ImGui::NewFrame();
	ImGui::Begin("Bezier Points");
	ImGui::DragInt("Subdivision Level", &current_loop, 0.01f, 0, LOOP_MAX_LEVELS - 1);

	if (current_loop != last_loop) {
		last_loop = current_loop;
		updateVertexObjects(loop_vertices[current_loop], loop_indices[current_loop], vbo, ibo);
	}

	ImGui::End();
}

void renderImgui()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
