#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "Shader.hpp"
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

Scene& scene = Scene::instance();

int main(int argc, char const* argv[])
{
	auto* window = initGLFW("lab5", SCR_WIDTH, SCR_HEIGHT);
	initGLAD();
	initImgui(window);

	scene.loadGltf(ASSET_PATH "/teapot.gltf");
	scene.createMeshes();
	scene.createShadowMap(SCR_WIDTH, SCR_HEIGHT);
	scene.camera.position = glm::vec3(0.0f, 10.0f, 10.0f);
	scene.camera.rotation = glm::vec3(-45.0f, -90.0f, 0.0f);
	scene.light.direction = glm::vec3(0.0f, -2.5f, -5.0f);

	Shader phong_shader(SHADER_PATH "/phong.vert", SHADER_PATH "/phong.frag");
	Shader shadow_shader(SHADER_PATH "/shadow.vert", SHADER_PATH "/shadow.frag");
	Shader map_shader(SHADER_PATH "/map.vert", SHADER_PATH "/map.frag");

	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window)) {
		float current_time = static_cast<float>(glfwGetTime());
		delta_time = current_time - last_time;
		last_time = current_time;

		processInput(window);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		flushImgui();

		if (!scene.show_shadow_map) {
			scene.renderShadow(shadow_shader);
			scene.renderScene(phong_shader);
		} else {
			scene.renderShadow(shadow_shader);
			scene.renderShadowMap(map_shader);
		}

		renderImgui();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	scene.deleteShadowMap();
	scene.deleteMeshes();

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
	ImGui::Begin("Shadow Mapping");
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
	ImGui::Text("Press WASD to translate the camera.");
	ImGui::Text("Move Right Mouse to rotate the camera.");
	ImGui::Text("Scroll to toggle the camera.");
	ImGui::PopStyleColor();
	ImGui::Checkbox("Show Shadow Map", &scene.show_shadow_map);
	ImGui::Checkbox("PCF/PCSS:", &scene.PCF_or_PCSS);
	ImGui::SameLine();
	ImGui::Text(scene.PCF_or_PCSS ? "PCF" : "PCSS");
	if (!scene.PCF_or_PCSS) {
		ImGui::DragFloat("Light Size", &scene.shadow_map.light_size, 0.1f, 1.0f, 20.0f);
		ImGui::DragInt("Blocker Search Radius", &scene.shadow_map.blocker_search_radius, 0.1f, 1, 10);
	}
	ImGui::DragInt("Shadow Sampling Radius", &scene.shadow_map.shadow_sampling_radius, 0.1f, 1, 10);
	ImGui::DragFloat3("Light Position", &scene.light.direction.x, 0.01f);
	ImGui::DragFloat("Light Ambient", &scene.light.ambient, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("Light Diffuse", &scene.light.diffuse, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("Light Specular", &scene.light.specular, 0.01f, 0.0f, 1.0f);
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
}

void scrollCallback(GLFWwindow* window, double x_ofs, double y_ofs)
{
	auto& camera = scene.camera;
	float sensitivity = 0.5f;

	camera.fov -= sensitivity * (float) y_ofs;
	camera.fov = glm::clamp(camera.fov, 30.0f, 60.0f);
}
