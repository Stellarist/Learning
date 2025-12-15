#define STB_IMAGE_IMPLEMENTATION

#include "AssetManager.hpp"

#include <print>
#include <filesystem>

AssetManager::AssetManager()
{
	scene = nullptr;
	stbi_set_flip_vertically_on_load(false);
	std::println("AssetManager singleton instance created.");
}

AssetManager::~AssetManager()
{
	cleanup();
}

AssetManager& AssetManager::instance()
{
	static AssetManager instance;
	return instance;
}

void AssetManager::setScene(std::shared_ptr<Scene> scene)
{
	if (this->scene.get() != scene.get()) {
		cleanup();
		this->scene = scene;
	}
}

void AssetManager::cleanup()
{
	if (scene) {
		for (const auto& [_, mesh] : loaded_meshes)
			scene->removeComponent(*mesh);
		for (const auto& [_, submesh] : loaded_submeshes)
			scene->removeComponent(*submesh);
		for (const auto& [_, material] : loaded_materials)
			scene->removeComponent(*material);
		for (const auto& [_, texture] : loaded_textures)
			scene->removeComponent(*texture);
		for (const auto& [_, image] : loaded_images)
			scene->removeComponent(*image);
	}

	mesh_id = 0;
	loaded_meshes.clear();
	loaded_submeshes.clear();
	loaded_materials.clear();
	loaded_textures.clear();
	loaded_images.clear();
	scene.reset();
}

Image* AssetManager::getImage(const std::string& name)
{
	auto it = loaded_images.find(name);
	if (it != loaded_images.end())
		return it->second;

	return nullptr;
}

Image* AssetManager::createImage(const std::vector<uint8_t>& data, const std::string& path, int width, int height, int channels)
{
	assert(scene != nullptr && "Scene is not set");

	auto image = std::make_unique<Image>(path);
	image->setData(data);
	image->setFormat(channels);
	image->setWidth(static_cast<unsigned int>(width));
	image->setHeight(static_cast<unsigned int>(height));

	Image* image_ptr = image.get();
	scene->addComponent(std::move(image));
	loaded_images[path] = image_ptr;

	return image_ptr;
}

Image* AssetManager::loadImage(const std::string& raw_path)
{
	int width = 0, height = 0, channels = 0;

	std::string path = std::filesystem::canonical(raw_path).string();
	if (Image* image = getImage(path); image)
		return image;

	unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
	if (!data) {
		std::println("Failed to load image: {}", path);
		std::println("STB Error: {}", stbi_failure_reason());
		return nullptr;
	}

	std::vector<uint8_t> image_data(data, data + width * height * channels);
	stbi_image_free(data);

	Image* new_image = createImage(image_data, path, width, height, channels);
	return new_image;
}

Texture* AssetManager::getTexture(const std::string& name)
{
	auto it = loaded_textures.find(name);
	if (it != loaded_textures.end())
		return it->second;

	return nullptr;
}

Texture* AssetManager::createTexture(Image* image, const std::string& name)
{
	assert(scene != nullptr && "Scene is not set");

	std::unique_ptr<Texture> texture = std::make_unique<Texture>(name);
	texture->setImage(*image);

	Texture* texture_ptr = texture.get();
	scene->addComponent(std::move(texture));
	loaded_textures[name] = texture_ptr;

	return texture_ptr;
}

Texture* AssetManager::loadTexture(const std::string& raw_path, const std::string& name)
{
	std::string path = std::filesystem::canonical(raw_path).string();
	if (Texture* texture = getTexture(path); texture)
		return texture;

	Image* image = loadImage(path);
	if (!image) {
		std::println("Failed to load image for texture: {}", path);
		return nullptr;
	}

	Texture* new_texture = createTexture(image, name);
	return new_texture;
}

Material* AssetManager::getMaterial(const std::string& name)
{
	auto it = loaded_materials.find(name);
	if (it != loaded_materials.end())
		return it->second;

	return nullptr;
}

Material* AssetManager::createMaterial(aiMaterial* ai_material, const aiScene* ai_scene)
{
	assert(scene != nullptr && "Scene is not set");

	aiString material_name;
	ai_material->Get(AI_MATKEY_NAME, material_name);
	std::string name = material_name.C_Str();
	if (auto* material = getMaterial(name); material)
		return material;

	auto material = std::make_unique<PBRMaterial>(material_name.C_Str());

	if (aiColor4D base_color; ai_material->Get(AI_MATKEY_COLOR_DIFFUSE, base_color) == AI_SUCCESS)
		material->setBaseColorFactor(glm::vec4(base_color.r, base_color.g, base_color.b, base_color.a));
	else if (aiColor3D base_color; ai_material->Get(AI_MATKEY_COLOR_DIFFUSE, base_color) == AI_SUCCESS)
		material->setBaseColorFactor(glm::vec4(base_color.r, base_color.g, base_color.b, 1.0f));

	if (aiColor3D emissive; ai_material->Get(AI_MATKEY_COLOR_EMISSIVE, emissive) == AI_SUCCESS)
		material->setEmissive(glm::vec3(emissive.r, emissive.g, emissive.b));

	if (int two_sided = 0; ai_material->Get(AI_MATKEY_TWOSIDED, two_sided) == AI_SUCCESS)
		material->setDoubleSided(two_sided != 0);

	if (float opacity = 1.0f; ai_material->Get(AI_MATKEY_OPACITY, opacity) == AI_SUCCESS)
		if (opacity < 1.0f) {
			material->setAlphaMode(AlphaMode::BLEND);
			material->setAlphaCutoff(opacity);
		}

	float metallic = 0.0f;
	if (ai_material->Get(AI_MATKEY_METALLIC_FACTOR, metallic) != AI_SUCCESS)
		if (aiColor3D specular; ai_material->Get(AI_MATKEY_SPECULAR_FACTOR, specular) == AI_SUCCESS)
			metallic = (specular.r + specular.g + specular.b) / 3.0f;
	material->setMetallicFactor(metallic);

	float roughness = 0.5f;
	if (ai_material->Get(AI_MATKEY_SPECULAR_FACTOR, roughness) == AI_SUCCESS)
		if (float shiness = 0.0f; ai_material->Get(AI_MATKEY_SHININESS, shiness) == AI_SUCCESS)
			roughness = 1.0f - std::min(1.0f, std::sqrt(shiness / 10.0f));
	material->setRoughnessFactor(roughness);

	loadMaterialTextures(material.get(), ai_material, aiTextureType_DIFFUSE, "texture_diffuse");
	loadMaterialTextures(material.get(), ai_material, aiTextureType_AMBIENT, "texture_ambient");
	loadMaterialTextures(material.get(), ai_material, aiTextureType_SPECULAR, "texture_specular");
	loadMaterialTextures(material.get(), ai_material, aiTextureType_HEIGHT, "texture_height");
	loadMaterialTextures(material.get(), ai_material, aiTextureType_NORMALS, "texture_normal");

	Material* material_ptr = material.get();
	scene->addComponent(std::move(material));
	loaded_materials[name] = material_ptr;

	return material_ptr;
}

void AssetManager::loadMaterialTextures(Material* material, aiMaterial* ai_material, aiTextureType type, std::string type_name)
{
	for (unsigned int i = 0; i < ai_material->GetTextureCount(type); i++) {
		aiString str;
		ai_material->GetTexture(type, i, &str);

		std::string tex_path = std::filesystem::canonical(current_dir + '/' + str.C_Str()).string();
		std::string tex_key = type_name + std::to_string(i);
		if (auto* texture = getTexture(tex_path); texture) {
			material->addTexture(tex_key, texture);
			continue;
		}

		if (Texture* texture = loadTexture(tex_path, tex_path); texture)
			material->addTexture(tex_key, texture);
		else
			std::println("Failed to load texture: {}", tex_path);
	}
}

Mesh* AssetManager::getMesh(const std::string& name)
{
	auto it = loaded_meshes.find(name);
	if (it != loaded_meshes.end())
		return it->second;

	return nullptr;
}

Mesh* AssetManager::loadMesh(const std::string& raw_path, const std::string& name)
{
	assert(scene != nullptr && "Scene is not set");

	current_dir = raw_path.substr(0, raw_path.find_last_of('/'));

	auto path = std::filesystem::canonical(raw_path).string();
	if (auto* mesh = getMesh(name); mesh)
		return mesh;

	Assimp::Importer importer;
	const aiScene*   ai_scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	if (!ai_scene || ai_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !ai_scene->mRootNode) {
		std::println("ERROR::ASSIMP::{}", importer.GetErrorString());
		return nullptr;
	}

	auto  node = std::make_unique<Node>(mesh_id++, ai_scene->mRootNode->mName.C_Str());
	auto  mesh = std::make_unique<Mesh>(name);
	Node* node_ptr = node.get();
	Mesh* mesh_ptr = mesh.get();

	mesh_ptr->setNode(*node_ptr);
	node_ptr->setComponent(*mesh_ptr);
	processMesh(mesh_ptr, node_ptr, ai_scene->mRootNode, ai_scene);

	scene->addNode(std::move(node));
	scene->addComponent(std::move(mesh));
	scene->getRoot().addChild(*node_ptr);
	loaded_meshes[name] = mesh_ptr;

	return mesh_ptr;
}

void AssetManager::processMesh(Mesh* mesh, Node* parent_node, aiNode* ai_node, const aiScene* ai_scene)
{
	for (unsigned int i = 0; i < ai_node->mNumMeshes; i++) {
		aiMesh*  ai_mesh = ai_scene->mMeshes[ai_node->mMeshes[i]];
		SubMesh* submesh = createSubMesh(ai_mesh, ai_scene);
		mesh->addSubmesh(*submesh);
	}

	for (unsigned int i = 0; i < ai_node->mNumChildren; i++) {
		aiNode* ai_child = ai_node->mChildren[i];
		auto    child_node = std::make_unique<Node>(mesh_id, ai_child->mName.C_Str());
		Node*   child_ptr = child_node.get();
		scene->addNode(std::move(child_node));
		parent_node->addChild(*child_ptr);
		processMesh(mesh, child_ptr, ai_node->mChildren[i], ai_scene);
	}
}

SubMesh* AssetManager::createSubMesh(aiMesh* ai_mesh, const aiScene* ai_scene)
{
	auto submesh = std::make_unique<SubMesh>(ai_mesh->mName.C_Str());
	bool has_normals = ai_mesh->HasNormals();
	bool has_texcoords = ai_mesh->HasTextureCoords(0);
	bool has_tans_and_bitans = ai_mesh->HasTangentsAndBitangents();

	unsigned int offset = 0, element_size = sizeof(float);
	unsigned int stride = 3 + has_normals * 3 + has_texcoords * 2 + has_tans_and_bitans * 6;

	std::vector<float> vertices_data;
	vertices_data.reserve(ai_mesh->mNumVertices * stride);

	submesh->setAttribute("POSITION", {GL_FLOAT, 3, stride * element_size, offset * element_size});
	offset += 3;

	if (has_normals) {
		submesh->setAttribute("NORMAL", {GL_FLOAT, 3, stride * element_size, offset * element_size});
		offset += 3;
	}

	if (has_texcoords) {
		submesh->setAttribute("TEXCOORD_0", {GL_FLOAT, 2, stride * element_size, offset * element_size});
		offset += 2;
	}

	if (has_tans_and_bitans) {
		submesh->setAttribute("TANGENT", {GL_FLOAT, 3, stride * element_size, offset * element_size});
		offset += 3;
		submesh->setAttribute("BITANGENT", {GL_FLOAT, 3, stride * element_size, offset * element_size});
		offset += 3;
	}

	for (unsigned int i = 0; i < ai_mesh->mNumVertices; i++) {
		vertices_data.emplace_back(ai_mesh->mVertices[i].x);
		vertices_data.emplace_back(ai_mesh->mVertices[i].y);
		vertices_data.emplace_back(ai_mesh->mVertices[i].z);

		if (has_normals) {
			vertices_data.emplace_back(ai_mesh->mNormals[i].x);
			vertices_data.emplace_back(ai_mesh->mNormals[i].y);
			vertices_data.emplace_back(ai_mesh->mNormals[i].z);
		}

		if (has_texcoords) {
			vertices_data.emplace_back(ai_mesh->mTextureCoords[0][i].x);
			vertices_data.emplace_back(ai_mesh->mTextureCoords[0][i].y);
		}

		if (has_tans_and_bitans) {
			vertices_data.emplace_back(ai_mesh->mTangents[i].x);
			vertices_data.emplace_back(ai_mesh->mTangents[i].y);
			vertices_data.emplace_back(ai_mesh->mTangents[i].z);

			vertices_data.emplace_back(ai_mesh->mBitangents[i].x);
			vertices_data.emplace_back(ai_mesh->mBitangents[i].y);
			vertices_data.emplace_back(ai_mesh->mBitangents[i].z);
		}
	}

	submesh->setVertices(std::move(vertices_data), ai_mesh->mNumVertices);

	if (ai_mesh->HasFaces()) {
		std::vector<unsigned int> indices;
		indices.reserve(ai_mesh->mNumFaces * 3);

		for (unsigned int i = 0; i < ai_mesh->mNumFaces; i++) {
			aiFace face = ai_mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}

		submesh->setIndices(std::move(indices));
	}

	if (ai_mesh->mMaterialIndex >= 0) {
		auto* ai_material = ai_scene->mMaterials[ai_mesh->mMaterialIndex];
		auto* material = createMaterial(ai_material, ai_scene);
		submesh->setMaterial(*material);
	}

	SubMesh* submesh_ptr = submesh.get();
	scene->addComponent(std::move(submesh));
	loaded_submeshes[submesh_ptr->getName()] = submesh_ptr;

	return submesh_ptr;
}

void AssetManager::setFlipVertically(bool flip)
{
	stbi_set_flip_vertically_on_load(flip);
}
