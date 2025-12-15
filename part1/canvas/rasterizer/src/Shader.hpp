#pragma once

#include "global.hpp"
#include "Model.hpp"

struct ShaderInfo {
	vec3f_t view_pos;
	vec3f_t color;
	vec3f_t normal;
	vec2f_t texcoord;

	vec3f_t ambient;
	vec3f_t diffuse;
	vec3f_t specular;

	std::vector<Texture*> textures;
};

class Shader {
private:
	Model   origin_model;
	Model   current_model;
	vec3f_t view_pos;
	mat4f_t model_mat;
	mat4f_t view_mat;
	mat4f_t projection_mat;

public:
	Shader();

	vec3f_t getViewPos();
	mat4f_t getModelMat();
	mat4f_t getViewMat();
	mat4f_t getProjectionMat();
	Model&  getOriginModel();
	Model&  getCurrentModel();

	void setViewPos(const vec3f_t& view_pos);
	void setModelMat(const mat4f_t& model_mat);
	void setViewMat(const mat4f_t& view_mat);
	void setProjectionMat(const mat4f_t& projection_mat);

	void use();
	void flush();
	void transform();
	void render();

	static vec3f_t phongShader(const ShaderInfo& shader);
	static vec3f_t textureShader(const ShaderInfo& shader);
};
