#pragma once

#include <unordered_map>

#include "base/Scene.hpp"
#include "components/Mesh.hpp"
#include "graphics/GLMesh.hpp"

class GraphicsManager {
private:
	std::shared_ptr<Scene> scene;

	std::unordered_map<std::string, std::unique_ptr<GLTexture>> gl_textures;
	std::unordered_map<std::string, std::unique_ptr<GLShader>>  gl_shaders;
	std::unordered_map<Mesh*, std::vector<GLMesh>>              gl_meshes;

	GraphicsManager();
	~GraphicsManager();

	void cleanup();

public:
	GraphicsManager(const GraphicsManager&) = delete;
	GraphicsManager& operator=(const GraphicsManager&) = delete;

	GraphicsManager(GraphicsManager&&) noexcept = delete;
	GraphicsManager& operator=(GraphicsManager&&) noexcept = delete;

	static GraphicsManager& instance();

	void setScene(std::shared_ptr<Scene> scene);

	GLTexture* getGLTexture(const std::string& name);
	GLTexture* uploadGLTexture(Texture* texture);

	GLShader* getGLShader(const std::string& name);
	GLShader* uploadGLShader(const std::string& name, const std::string& vertex_path, const std::string& fragment_path);

	void uploadMesh(Mesh& mesh);
	void drawMesh(Mesh& mesh, GLShader& shader);

	friend void printGraphicsManager();
};
