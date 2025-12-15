#include <vector>
#include <string>
#include <span>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "Shader.hpp"

void createVertexObjects(std::span<const glm::vec4> vertices, unsigned int& vbo, unsigned int& vao, unsigned int& ubo);
void deleteVertexObjects(unsigned int& vbo, unsigned int& vao, unsigned int& ubo);
void updateVertexObjects(std::span<const glm::vec4> vertices, unsigned int& ubo);
auto initGLFW(const std::string& window_title, int width, int height) -> GLFWwindow*;
void initGLAD();
void initImgui(GLFWwindow* window);
void destroyImgui();
void flushImgui(std::span<glm::vec4> bezier_points, unsigned int& ubo);
void renderImgui();
void processInput(GLFWwindow* window);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);

constexpr unsigned int SCR_WIDTH = 1600;
constexpr unsigned int SCR_HEIGHT = 900;
constexpr unsigned int UBO_MAX_COUNTS = 16;

int main(int argc, char const* argv[])
{
	auto* window = initGLFW("lab4", SCR_WIDTH, SCR_HEIGHT);
	initGLAD();
	initImgui(window);

	unsigned int bezier_vbo, bezier_vao, bezier_ubo;

	std::vector<glm::vec4> bezier_points = {
	    {-0.500f, -0.5000f, 0.000f, 0.000f},
	    {-0.250f, -0.300f, 0.000f, 0.000f},
	    {-0.125f, -0.200f, 0.000f, 0.000f},
	    {0.000f, 0.000f, 0.000f, 0.000f},
	    {0.250f, 0.200f, 0.000f, 0.000f},
	    {0.350f, 0.300f, 0.000f, 0.000f},
	    {0.500f, 0.500f, 0.000f, 0.000f}};

	createVertexObjects(bezier_points, bezier_vbo, bezier_vao, bezier_ubo);

	Shader shader(SHADER_PATH "/bezier.vert", SHADER_PATH "/bezier.frag", SHADER_PATH "/bezier.geom");

	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window)) {
		processInput(window);

		glClearColor(0.f, 0.f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		flushImgui(bezier_points, bezier_ubo);

		shader.use();
		shader.setInt("counts", bezier_points.size());
		shader.setInt("segments", 128);

		glBindVertexArray(bezier_vao);
		glDrawArrays(GL_POINTS, 0, 1);
		glBindVertexArray(0);

		renderImgui();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	deleteVertexObjects(bezier_vbo, bezier_vao, bezier_ubo);

	destroyImgui();
	glfwTerminate();

	return 0;
}

void createVertexObjects(std::span<const glm::vec4> vertices, unsigned int& vbo, unsigned int& vao, unsigned int& ubo)
{
	assert(vertices.size() <= UBO_MAX_COUNTS && "Too many vertices for UBO.");

	glm::vec2 dummy(0.0f, 0.0f);

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ubo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(dummy), &dummy, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*) 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferData(GL_UNIFORM_BUFFER, UBO_MAX_COUNTS * sizeof(glm::vec4), nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo);

	glBufferSubData(GL_UNIFORM_BUFFER, 0, vertices.size() * sizeof(glm::vec4), vertices.data());
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void deleteVertexObjects(unsigned int& vbo, unsigned int& vao, unsigned int& ubo)
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ubo);
}

void updateVertexObjects(std::span<const glm::vec4> vertices, unsigned int& ubo)
{
	assert(vertices.size() <= UBO_MAX_COUNTS && "Too many vertices for UBO.");

	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, vertices.size() * sizeof(glm::vec4), vertices.data());
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
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

void flushImgui(std::span<glm::vec4> bezier_points, unsigned int& ubo)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();

	ImGui::NewFrame();
	ImGui::Begin("Bezier Points");
	for (int i = 0; i < bezier_points.size(); i++) {
		std::string label = "point " + std::to_string(i);
		ImGui::DragFloat2(label.c_str(), &bezier_points[i].x, 0.002f, -1.f, 1.f, "%.3f");
		updateVertexObjects(bezier_points, ubo);
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
