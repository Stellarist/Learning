#pragma once

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <glad/glad.h>
#include <stb_image.h>

#include "base/Scene.hpp"
#include "components/Mesh.hpp"
#include "components/Material.hpp"

class AssetManager {
private:
	std::shared_ptr<Scene> scene;

	unsigned int mesh_id{0};
	std::string  current_dir{};

	std::unordered_map<std::string, Image*>    loaded_images;
	std::unordered_map<std::string, Texture*>  loaded_textures;
	std::unordered_map<std::string, Material*> loaded_materials;
	std::unordered_map<std::string, SubMesh*>  loaded_submeshes;
	std::unordered_map<std::string, Mesh*>     loaded_meshes;

	AssetManager();
	~AssetManager();

	void cleanup();

public:
	static AssetManager& instance();

	AssetManager(const AssetManager&) = delete;
	AssetManager& operator=(const AssetManager&) = delete;

	AssetManager(AssetManager&&) noexcept = delete;
	AssetManager& operator=(AssetManager&&) noexcept = delete;

	void setScene(std::shared_ptr<Scene> scene);

	Image* getImage(const std::string& name);
	Image* createImage(const std::vector<uint8_t>& data, const std::string& path, int width, int height, int channels);
	Image* loadImage(const std::string& raw_path);

	Texture* getTexture(const std::string& name);
	Texture* createTexture(Image* image, const std::string& name);
	Texture* loadTexture(const std::string& raw_path, const std::string& name = {});

	Material* getMaterial(const std::string& name);
	Material* createMaterial(aiMaterial* ai_material, const aiScene* ai_scene);
	void      loadMaterialTextures(Material* material, aiMaterial* ai_material, aiTextureType type, std::string type_name);

	Mesh*    getMesh(const std::string& name);
	Mesh*    loadMesh(const std::string& raw_path, const std::string& name);
	void     processMesh(Mesh* mesh, Node* parent_node, aiNode* ai_node, const aiScene* ai_scene);
	SubMesh* createSubMesh(aiMesh* ai_mesh, const aiScene* ai_scene);

	static void setFlipVertically(bool flip);

	friend void printAssetManager();
};