#include "GraphicsManager.hpp"

#include <print>

GraphicsManager::GraphicsManager()
{
	scene = nullptr;
	std::println("GraphicsManager singleton instance created.");
}

GraphicsManager::~GraphicsManager()
{
	cleanup();
}

GraphicsManager& GraphicsManager::instance()
{
	static GraphicsManager instance;
	return instance;
}

void GraphicsManager::setScene(std::shared_ptr<Scene> scene)
{
	if (this->scene.get() != scene.get()) {
		cleanup();
		this->scene = std::move(scene);
	}
}

void GraphicsManager::cleanup()
{
	gl_meshes.clear();
	gl_textures.clear();
	gl_shaders.clear();
	scene.reset();
}

GLTexture* GraphicsManager::getGLTexture(const std::string& name)
{
	auto it = gl_textures.find(name);
	if (it != gl_textures.end())
		return it->second.get();

	return nullptr;
}

GLTexture* GraphicsManager::uploadGLTexture(Texture* texture)
{
	assert(scene && "Scene is not set");

	if (auto* gl_tex = getGLTexture(texture->getName()); gl_tex)
		return gl_tex;

	unsigned int channels = texture->getImage()->getFormat();
	unsigned int format = 0;
	if (channels == 1)
		format = GL_RED;
	else if (channels == 2)
		format = GL_RG;
	else if (channels == 3)
		format = GL_RGB;
	else if (channels == 4)
		format = GL_RGBA;
	else {
		std::println("Unsupported image format: {} channels", channels);
		return nullptr;
	}

	std::unique_ptr<GLTexture> gl_texture = std::make_unique<GLTexture>(
	    texture->getImage()->getData().data(),
	    texture->getImage()->getWidth(),
	    texture->getImage()->getHeight(),
	    format, GL_UNSIGNED_BYTE);

	gl_texture->setParameteri({{GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR},
	                           {GL_TEXTURE_MAG_FILTER, GL_LINEAR},
	                           {GL_TEXTURE_WRAP_S, GL_REPEAT},
	                           {GL_TEXTURE_WRAP_T, GL_REPEAT}});
	gl_texture->generateMipmap();

	auto* gl_texture_ptr = gl_texture.get();
	gl_textures[texture->getName()] = std::move(gl_texture);

	return gl_texture_ptr;
}

GLShader* GraphicsManager::getGLShader(const std::string& name)
{
	auto it = gl_shaders.find(name);
	if (it != gl_shaders.end())
		return it->second.get();

	return nullptr;
}

GLShader* GraphicsManager::uploadGLShader(const std::string& name, const std::string& vertex_path, const std::string& fragment_path)
{
	assert(scene && "Scene is not set");

	if (auto* shader = getGLShader(name); shader)
		return shader;

	std::unique_ptr<GLShader> gl_shader = std::make_unique<GLShader>(
	    vertex_path,
	    fragment_path);

	auto* shader_ptr = gl_shader.get();
	gl_shaders[name] = std::move(gl_shader);

	return shader_ptr;
}

void GraphicsManager::uploadMesh(Mesh& mesh)
{
	for (const auto& submesh : mesh.getSubmeshes()) {
		GLMesh gl_mesh(submesh);
		for (const auto& [name, texture] : submesh->getMaterial()->getTextures())
			if (auto* tex = uploadGLTexture(texture); tex)
				gl_mesh.setTexture(name, *tex);
		gl_meshes[&mesh].emplace_back(std::move(gl_mesh));
	}
}

void GraphicsManager::drawMesh(Mesh& mesh, GLShader& shader)
{
	if (gl_meshes.find(&mesh) == gl_meshes.end())
		uploadMesh(mesh);

	for (auto& gl_mesh : gl_meshes[&mesh])
		gl_mesh.draw(shader);
}
