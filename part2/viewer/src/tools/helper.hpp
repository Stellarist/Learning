#pragma once

#include <print>

#include "base/Scene.hpp"
#include "components/Camera.hpp"
#include "components/Light.hpp"
#include "resource/AssetManager.hpp"
#include "resource/GraphicsManager.hpp"

extern const int SCR_WIDTH;
extern const int SCR_HEIGHT;

inline Scene* initDefaultScene()
{
	auto scene = std::make_shared<Scene>(0, "MainScene");
	AssetManager::instance().setScene(scene);
	GraphicsManager::instance().setScene(scene);

	return scene.get();
}

inline Camera* initDefaultCamera(Scene& scene)
{
	auto camera_node = std::make_unique<Node>(0, "MainCamera");
	auto camera_component = std::make_unique<PerspectiveCamera>("MainCamera");
	camera_component->setAspectRatio(static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT));
	camera_component->setFov(45.0f);
	camera_component->setNearPlane(0.1f);
	camera_component->setFarPlane(1000.0f);
	camera_node->getTransform().setTranslation(glm::vec3(0.0f, 0.0f, 3.0f));

	Camera* camera_ptr = camera_component.get();
	camera_component->setNode(*camera_node);
	camera_node->setComponent(*camera_component);
	scene.addComponent(std::move(camera_component));
	scene.getRoot().addChild(*camera_node);
	scene.addNode(std::move(camera_node));

	return camera_ptr;
}

inline Light* initDefaultLight(Scene& scene)
{
	auto light_node = std::make_unique<Node>(0, "Light");
	auto light_component = std::make_unique<DirectionalLight>("Light");
	light_component->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
	light_component->setIntensity(1.0f);
	light_component->setDirection(glm::vec3(0.0f, -1.0f, 0.0f));
	light_node->getTransform().setTranslation(glm::vec3(0.0f, 5.0f, 0.0f));

	Light* light_ptr = light_component.get();
	light_component->setNode(*light_node);
	light_node->setComponent(*light_component);
	scene.addComponent(std::move(light_component));
	scene.getRoot().addChild(*light_node);
	scene.addNode(std::move(light_node));

	return light_ptr;
}

inline Mesh* addMesh(Scene& scene, const std::string& model_path, const std::string& model_name)
{
	AssetManager& asset_manager = AssetManager::instance();

	auto* mesh = asset_manager.loadMesh(model_path, model_name);
	if (!mesh) {
		std::println("Failed to load mesh from path: {}", model_path);
		return nullptr;
	}

	auto* mesh_node = mesh->getNode();
	if (mesh_node) {
		auto& transform = mesh_node->getTransform();
		transform.setScale(glm::vec3(0.1f, 0.1f, 0.1f));
		transform.setTranslation(glm::vec3(0.0f, -0.5f, 0.0f));
		transform.setRotation(glm::vec3(0.0f, 0.0f, 0.0f));
	}

	return mesh;
}

inline void checkGLError(const char* file, int line)
{
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		std::println("OpenGL error in {} at line {}: {}", file, line, error);
	}
}

inline void printSceneNodes(const Scene& scene)
{
	std::println("\n==================== Scene Nodes Tree ====================");
	std::println("Scene: {}", scene.getName());

	auto print_node_tree = [&](
	                           this auto&&        print_node_tree,
	                           Node*              node,
	                           const std::string& prefix = "",
	                           bool               is_last = true) {
		if (!node)
			return;

		std::string node_icon = is_last ? "└── " : "├── ";
		std::string child_prefix = prefix + (is_last ? "    " : "│   ");
		std::println("{}{}[{}] {} (ID: {})", prefix, node_icon, node->getType().name(), node->getName(), node->getId());

		if (node->hasComponent<Transform>()) {
			const auto& transform = node->getComponent<Transform>();
			const auto& pos = transform.getTranslation();
			const auto& scale = transform.getScale();
			std::println("{}├── Transform: pos({:.2f}, {:.2f}, {:.2f}) scale({:.2f}, {:.2f}, {:.2f})",
			             child_prefix, pos.x, pos.y, pos.z, scale.x, scale.y, scale.z);
		}

		if (node->hasComponent<Camera>()) {
			const auto& camera = node->getComponent<Camera>();
			std::println("{}├── Camera: {}", child_prefix, camera.getName());
		}

		if (node->hasComponent<Light>()) {
			const auto& light = node->getComponent<Light>();
			const auto& color = light.getColor();
			std::println("{}├── Light: {} - color({:.2f}, {:.2f}, {:.2f}) intensity({:.2f})",
			             child_prefix, light.getName(), color.r, color.g, color.b, light.getIntensity());
		}

		if (node->hasComponent<Mesh>()) {
			const auto& mesh = node->getComponent<Mesh>();
			std::println("{}├── SubMesh: {} - submeshes({})",
			             child_prefix, mesh.getName(), mesh.getSubmeshes().size());
		}

		const auto& children = node->getChildren();
		for (size_t i = 0; i < children.size(); ++i) {
			bool is_last_child = (i == children.size() - 1);
			print_node_tree(children[i], child_prefix, is_last_child);
		}
	};

	const Node& root = const_cast<Scene&>(scene).getRoot();
	if (root.getChildren().empty()) {
		std::println("└── (No child nodes)");
		return;
	}

	const auto& children = root.getChildren();
	for (size_t i = 0; i < children.size(); ++i) {
		bool is_last = (i == children.size() - 1);
		print_node_tree(children[i], "", is_last);
	}

	std::println("=========================================================\n");
}

inline void printSceneComponents(const Scene& scene)
{
	std::println("\n================== Scene Components ==================");
	std::println("Scene: {}", scene.getName());

	auto get_component_type_name = [](const std::type_index& type) {
		std::string type_name = type.name();
		if (type_name.find("Transform") != std::string::npos)
			return "Transform";
		if (type_name.find("PerspectiveCamera") != std::string::npos)
			return "PerspectiveCamera";
		if (type_name.find("OrthoCamera") != std::string::npos)
			return "OrthoCamera";
		if (type_name.find("Camera") != std::string::npos)
			return "Camera";
		if (type_name.find("DirectionalLight") != std::string::npos)
			return "DirectionalLight";
		if (type_name.find("PointLight") != std::string::npos)
			return "PointLight";
		if (type_name.find("SpotLight") != std::string::npos)
			return "SpotLight";
		if (type_name.find("Light") != std::string::npos)
			return "Light";
		if (type_name.find("SubMesh") != std::string::npos)
			return "SubMesh";
		if (type_name.find("Mesh") != std::string::npos)
			return "Mesh";
		if (type_name.find("PBRMaterial") != std::string::npos)
			return "PBRMaterial";
		if (type_name.find("Material") != std::string::npos)
			return "Material";
		if (type_name.find("Texture") != std::string::npos)
			return "Texture";
		if (type_name.find("Image") != std::string::npos)
			return "Image";
		if (type_name.find("AABB") != std::string::npos)
			return "AABB";
		return "";
	};

	std::vector<std::type_index> component_types = {
	    typeid(Transform),
	    typeid(PerspectiveCamera),
	    typeid(OrthoCamera),
	    typeid(DirectionalLight),
	    typeid(PointLight),
	    typeid(SpotLight),
	    typeid(Mesh),
	    typeid(SubMesh),
	    typeid(PBRMaterial),
	    typeid(Material),
	    typeid(Texture),
	    typeid(Image),
	    typeid(AABB)};

	bool has_any_components = false;
	for (const auto& type : component_types) {
		if (scene.hasComponent(type)) {
			has_any_components = true;
			const auto& components = scene.getComponents(type);
			std::string type_name = get_component_type_name(type);

			std::println("├── {} ({})", type_name, components.size());

			for (size_t i = 0; i < components.size(); ++i) {
				const auto& component = components[i];
				bool        is_last = (i == components.size() - 1);
				std::string item_icon = is_last ? "│   └── " : "│   ├── ";

				std::println("{}[{}] {} (UID: {})", item_icon,
				             get_component_type_name(component->getType()),
				             component->getName(),
				             component->getUid());

				if (auto* camera = dynamic_cast<PerspectiveCamera*>(component.get())) {
					std::println("{}    FOV: {:.1f}°, Aspect: {:.2f}, Near: {:.2f}, Far: {:.2f}",
					             is_last ? "        " : "│       ",
					             glm::degrees(camera->getFov()), camera->getAspectRatio(),
					             camera->getNearPlane(), camera->getFarPlane());
				} else if (auto* light = dynamic_cast<DirectionalLight*>(component.get())) {
					const auto& color = light->getColor();
					const auto& dir = light->getDirection();
					std::println("{}    Color: ({:.2f}, {:.2f}, {:.2f}), Intensity: {:.2f}",
					             is_last ? "        " : "│       ",
					             color.r, color.g, color.b, light->getIntensity());
					std::println("{}    Direction: ({:.2f}, {:.2f}, {:.2f})",
					             is_last ? "        " : "│       ",
					             dir.x, dir.y, dir.z);
				} else if (auto* mesh = dynamic_cast<Mesh*>(component.get())) {
					std::println("{}    Submeshes: {}",
					             is_last ? "        " : "│       ",
					             mesh->getSubmeshes().size());
				} else if (auto* submesh = dynamic_cast<SubMesh*>(component.get())) {
					std::println("{}    Vertices: {}, Indices: {}, Visible: {}",
					             is_last ? "        " : "│       ",
					             submesh->getVerticesCount(), submesh->getIndicesCount(),
					             submesh->isVisible() ? "Yes" : "No");
				} else if (auto* material = dynamic_cast<PBRMaterial*>(component.get())) {
					const auto& base_color = material->getBaseColorFactor();
					std::println("{}    BaseColor: ({:.2f}, {:.2f}, {:.2f}, {:.2f})",
					             is_last ? "        " : "│       ",
					             base_color.r, base_color.g, base_color.b, base_color.a);
					std::println("{}    Metallic: {:.2f}, Roughness: {:.2f}",
					             is_last ? "        " : "│       ",
					             material->getMetallicFactor(), material->getRoughnessFactor());
				} else if (auto* image = dynamic_cast<Image*>(component.get())) {
					std::println("{}    Size: {}x{}, Format: {}, Data: {} bytes",
					             is_last ? "        " : "│       ",
					             image->getWidth(), image->getHeight(), image->getFormat(),
					             image->getData().size());
				}
			}
		}
	}

	if (!has_any_components) {
		std::println("└── (No components found)");
	}

	std::println("=====================================================\n");
}

inline void printAssetManager()
{
	std::println("\n================== Asset Manager ==================");
	auto& asset_manager = AssetManager::instance();

	std::println("Loaded Meshes: {}", asset_manager.loaded_meshes.size());
	for (const auto& [name, mesh] : asset_manager.loaded_meshes)
		std::println("  - {} (UID: {})", name, mesh->getUid());

	std::println("Loaded SubMeshes: {}", asset_manager.loaded_submeshes.size());
	for (const auto& [name, submesh] : asset_manager.loaded_submeshes)
		std::println("  - {} (UID: {})", name, submesh->getUid());

	std::println("Loaded Images: {}", asset_manager.loaded_images.size());
	for (const auto& [name, image] : asset_manager.loaded_images)
		std::println("  - {} (UID: {})", name, image->getUid());

	std::println("Loaded Textures: {}", asset_manager.loaded_textures.size());
	for (const auto& [name, texture] : asset_manager.loaded_textures)
		std::println("  - {} (UID: {})", name, texture->getUid());

	std::println("Loaded Materials: {}", asset_manager.loaded_materials.size());
	for (const auto& [name, material] : asset_manager.loaded_materials)
		std::println("  - {} (UID: {})", name, material->getUid());

	std::println("=====================================================\n");
}

inline void printGraphicsManager()
{
	std::println("\n================== Graphics Manager ==================");
	auto& graphics_manager = GraphicsManager::instance();

	std::println("GL Meshes: {}", graphics_manager.gl_meshes.size());
	for (const auto& [mesh, glmeshes] : graphics_manager.gl_meshes) {
		std::println("  - Mesh UID: {} - GL Meshes: {}", mesh->getUid(), glmeshes.size());
		for (const auto& glmesh : glmeshes) {
			std::println("    - GLMesh Name: {}", glmesh.getSubMesh()->getName());
		}
	}

	std::println("GL Shaders: {}", graphics_manager.gl_shaders.size());
	for (const auto& [name, gl_shader] : graphics_manager.gl_shaders)
		std::println("  - {} (ID: {})", name, gl_shader->getId());

	std::println("GL Textures: {}", graphics_manager.gl_textures.size());
	for (const auto& [name, gl_texture] : graphics_manager.gl_textures)
		std::println("  - {} (ID: {})", name, gl_texture->getId());

	std::println("=====================================================\n");
}
