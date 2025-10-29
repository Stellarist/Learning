#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "tools/helper.hpp"

const int SCR_WIDTH = 1920;
const int SCR_HEIGHT = 1080;

auto initWindow(Camera* camera) -> GLFWwindow*;
void frameSizeCallback(GLFWwindow* window, int width, int height);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window, double delta_time);

int main(int argc, char const* argv[])
{
	auto* scene = initDefaultScene();
	auto* camera = initDefaultCamera(*scene);
	auto* light = initDefaultLight(*scene);
	auto& asset_manager = AssetManager::instance();
	auto& graphics_manager = GraphicsManager::instance();

	auto* window = initWindow(camera);
	std::println("Scene Initializing.");

	auto* mesh = addMesh(*scene, SOURCE_DIR "/assets/nanosuit/nanosuit.obj", "Nanosuit");
	auto* mesh2 = addMesh(*scene, SOURCE_DIR "/assets/backpack/backpack.obj", "Backpack");
	auto* mesh3 = addMesh(*scene, SOURCE_DIR "/assets/backpack/backpack.obj", "Backpack2");
	auto* shader = graphics_manager.uploadGLShader("ModelShader", SHADER_DIR "/default.vert", SHADER_DIR "/default.frag");

	auto& camera_transform = camera->getNode()->getTransform();
	auto& light_transform = light->getNode()->getTransform();
	auto& mesh_transform = mesh->getNode()->getTransform();
	auto& mesh2_transform = mesh2->getNode()->getTransform();
	auto& mesh3_transform = mesh3->getNode()->getTransform();

	mesh_transform.setScale(glm::vec3(0.1f, 0.1f, 0.1f));
	mesh_transform.setTranslation(glm::vec3(0.0f, -0.5f, 0.0f));
	mesh2_transform.setScale(glm::vec3(0.1f, 0.1f, 0.1f));
	mesh2_transform.setTranslation(glm::vec3(1.0f, -0.5f, 1.0f));
	mesh3_transform.setScale(glm::vec3(0.1f, 0.1f, 0.1f));
	mesh3_transform.setTranslation(glm::vec3(-1.0f, -0.5f, -1.0f));
	light_transform.setTranslation(glm::vec3(0.0f, 5.0f, 0.0f));

	std::println("Scene initialized.");

	glEnable(GL_DEPTH_TEST);

	bool first = true;
	while (!glfwWindowShouldClose(window)) {
		static double delta_time = 0.0;
		static double last_frame = 0.0;

		double current_frame = glfwGetTime();
		delta_time = current_frame - last_frame;
		last_frame = current_frame;

		processInput(window, delta_time);

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader->use();
		shader->setMat4("model", mesh_transform.getWorldMatrix());
		shader->setMat4("view", camera->getView());
		shader->setMat4("projection", camera->getProjection());
		graphics_manager.drawMesh(*mesh, *shader);

		shader->use();
		shader->setMat4("model", mesh2_transform.getWorldMatrix());
		shader->setMat4("view", camera->getView());
		shader->setMat4("projection", camera->getProjection());
		graphics_manager.drawMesh(*mesh2, *shader);

		shader->use();
		shader->setMat4("model", mesh3_transform.getWorldMatrix());
		shader->setMat4("view", camera->getView());
		shader->setMat4("projection", camera->getProjection());
		graphics_manager.drawMesh(*mesh3, *shader);

		if (first) {
			printSceneNodes(*scene);
			printSceneComponents(*scene);
			printAssetManager();
			printGraphicsManager();
			first = false;
		}

		std::println("FPS: {:.2f}", 1.0f / delta_time);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

GLFWwindow* initWindow(Camera* camera)
{
	if (!glfwInit()) {
		std::println("Failed to initialize GLFW.");
		throw std::runtime_error("Failed to initialize GLFW.");
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Viewer", nullptr, nullptr);
	if (window == nullptr) {
		std::println("Failed to create GLFW window.");
		glfwTerminate();
		throw std::runtime_error("Failed to create GLFW window.");
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, frameSizeCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetScrollCallback(window, scrollCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetWindowUserPointer(window, camera);

	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
		std::println("Failed to initialize GLAD.");
		throw std::runtime_error("Failed to initialize GLAD.");
	}

	return window;
}

void frameSizeCallback(GLFWwindow* window, int width, int height)
{
	const_cast<int&>(SCR_WIDTH) = width;
	const_cast<int&>(SCR_HEIGHT) = height;
	glViewport(0, 0, width, height);
}

void mouseCallback(GLFWwindow* window, double xpos_in, double ypos_in)
{
	static float last_x = SCR_WIDTH / 2.0f;
	static float last_y = SCR_HEIGHT / 2.0f;
	static bool  first_mouse = true;

	auto* camera = dynamic_cast<PerspectiveCamera*>(
	    static_cast<Camera*>(glfwGetWindowUserPointer(window)));
	auto& camera_transform = camera->getNode()->getTransform();

	auto could_scroll = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
	auto xpos = static_cast<float>(xpos_in);
	auto ypos = static_cast<float>(ypos_in);

	if (!could_scroll) {
		first_mouse = true;
		return;
	}

	if (first_mouse) {
		last_x = xpos;
		last_y = ypos;
		first_mouse = false;
		return;
	}

	float xoffset = last_x - xpos;
	float yoffset = last_y - ypos;
	last_x = xpos;
	last_y = ypos;

	float sensitivity = 0.001f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	glm::quat yaw_rotation = glm::angleAxis(xoffset, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::quat pitch_rotation = glm::angleAxis(yoffset, camera->getRight());

	camera_transform.setRotation(yaw_rotation * pitch_rotation * camera_transform.getRotation());
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	auto* camera = dynamic_cast<PerspectiveCamera*>(
	    static_cast<Camera*>(glfwGetWindowUserPointer(window)));
	auto camera_fov = camera->getFov();

	camera_fov -= static_cast<float>(yoffset);
	camera_fov = std::clamp(camera_fov, 1.0f, 90.0f);

	camera->setFov(camera_fov);
}

void processInput(GLFWwindow* window, double delta_time)
{
	auto* camera = dynamic_cast<PerspectiveCamera*>(
	    static_cast<Camera*>(glfwGetWindowUserPointer(window)));
	auto& camera_transform = camera->getNode()->getTransform();
	auto  camera_pos = camera_transform.getTranslation();

	float camera_speed = 2.5f * static_cast<float>(delta_time);
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera_pos += camera_speed * camera->getFront();
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera_pos -= camera_speed * camera->getFront();
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera_pos -= camera_speed * camera->getRight();
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera_pos += camera_speed * camera->getRight();

	camera_transform.setTranslation(camera_pos);
}
