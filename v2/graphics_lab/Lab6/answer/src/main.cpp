#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "Scene.hpp"

auto initGLFW(const std::string& window_title, int width, int height) -> GLFWwindow*;
void initImgui(GLFWwindow* window);
void destroyImgui();
void flushImgui();
void renderImgui();
void initGLAD();
void processInput(GLFWwindow* window);
void processInput(GLFWwindow* window);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void cursorPosCallback(GLFWwindow* window, double x_pos_in, double y_pos_in);
void scrollCallback(GLFWwindow* window, double x_ofs, double y_ofs);

constexpr unsigned int SCR_WIDTH = 1920;
constexpr unsigned int SCR_HEIGHT = 1080;

float delta_time{0.0f};
float last_time{0.0f};

auto& scene = Scene::instance();

int main(int argc, char const* argv[])
{
	auto* window = initGLFW("lab6", SCR_WIDTH, SCR_HEIGHT);
	initGLAD();
	initImgui(window);

	Shader cubemap_shader(SHADER_PATH "/cubemap.vert", SHADER_PATH "/cubemap.frag");
	Shader skybox_shader(SHADER_PATH "/cubemap.vert", SHADER_PATH "/skybox.frag");
	Shader irradiance_shader(SHADER_PATH "/cubemap.vert", SHADER_PATH "/irradiance.frag");
	Shader prefilter_shader(SHADER_PATH "/cubemap.vert", SHADER_PATH "/prefilter.frag");
	Shader shading_shader(SHADER_PATH "/shading.vert", SHADER_PATH "/shading.frag");

	scene.loadScene(ASSET_PATH "/dragon.glb");
	scene.loadSkybox(ASSET_PATH "/skybox.jpeg");
	scene.loadBrdfLut(ASSET_PATH "/brdf.png");
	scene.create();
	scene.generate(
	    cubemap_shader,
	    irradiance_shader,
	    prefilter_shader);
	scene.camera.position = glm::vec3(0.0f, 0.0f, 8.0f);
	scene.camera.rotation = glm::vec3(0.0f, -90.0f, 0.0f);

	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window)) {
		float current_time = static_cast<float>(glfwGetTime());
		delta_time = current_time - last_time;
		last_time = current_time;

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		processInput(window);
		flushImgui();

		scene.render(skybox_shader, shading_shader);

		renderImgui();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	scene.destroy();

	destroyImgui();
	glfwTerminate();

	return 0;
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
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	window = glfwCreateWindow(width, height, window_title.data(), nullptr, nullptr);
	if (!window) {
		std::cerr << "GLFW window creation failed." << std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
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

void initImgui(GLFWwindow* window)
{
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 450");
	ImGui::GetIO().FontGlobalScale = 1.5f;
}

void destroyImgui()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void flushImgui()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();

	ImGui::NewFrame();
	ImGui::Begin("Environment Mapping");
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
	ImGui::Text("Press WASD to translate the camera.");
	ImGui::Text("Move Right Mouse to rotate the camera.");
	ImGui::Text("Scroll to toggle the camera.");
	ImGui::PopStyleColor();
	ImGui::DragFloat3("Albedo", &scene.meshes.front().albedo.x, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("Metallic", &scene.meshes.front().metallic, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("Roughness", &scene.meshes.front().roughness, 0.01f, 0.0f, 1.0f);
	ImGui::End();
}

void renderImgui()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void processInput(GLFWwindow* window)
{
	auto& camera = scene.camera;
	float time = delta_time;
	float sensitivity = 5.0f;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.position += time * sensitivity * camera.front();
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.position -= time * sensitivity * camera.front();
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.position -= time * sensitivity * camera.right();
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.position += time * sensitivity * camera.right();
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		scene.camera.rotatable = true;
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
		scene.camera.rotatable = false;
}

void cursorPosCallback(GLFWwindow* window, double x_pos_in, double y_pos_in)
{
	static bool  first_mouse = true;
	static float last_x{}, last_y{};

	auto& camera = scene.camera;
	if (!camera.rotatable) {
		first_mouse = true;
		return;
	}

	float sensitivity = 0.05f;
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

	camera.rotation.x += y_ofs * sensitivity;
	camera.rotation.y += x_ofs * sensitivity;
	camera.rotation.x = glm::clamp(camera.rotation.x, -89.0f, 89.0f);
}

void scrollCallback(GLFWwindow* window, double x_ofs, double y_ofs)
{
	auto& camera = scene.camera;
	float sensitivity = 0.5f;

	camera.fov -= sensitivity * (float) y_ofs;
	camera.fov = glm::clamp(camera.fov, 30.0f, 60.0f);
}
