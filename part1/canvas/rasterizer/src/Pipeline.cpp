#include "Pipeline.hpp"

Pipeline Pipeline::instance;

Pipeline::Pipeline()
{
	model_path = PROJECT_PATH_1 "/assets/diablo/diablo3_pose.obj";
	texture_path = PROJECT_PATH_1 "/assets/diablo/diablo3_pose_diffuse.tga";

	model = new Model(model_path);
	model->addTextures(texture_path, TextureType::DIFFUSE);

	camera = new Camera(vec3f_t{width / 2.f, height / 2.f, 3.0f});
	rasterizer = new Rasterizer(width, height);
	shader = new Shader();
}

Pipeline::~Pipeline()
{
	delete model;
	delete camera;
	delete rasterizer;
	delete shader;
}

void Pipeline::init()
{
	for (auto& func : instance.callback_queue[static_cast<int>(CallbackType::INIT)]) {
		func();
	}
}

void Pipeline::tick()
{
	for (auto& func : instance.callback_queue[static_cast<int>(CallbackType::TICK)]) {
		func();
	}
}

void Pipeline::destroy()
{
	for (auto& func : instance.callback_queue[static_cast<int>(CallbackType::DESTROY)]) {
		func();
	}
}

void Pipeline::clear(vec3f_t color)
{
	instance.rasterizer->clear(color);
}

void Pipeline::render()
{
	if (!instance.model || !instance.shader)
		return;
	instance.shader->setViewPos(instance.camera->getPosition());
	instance.shader->flush();
	instance.shader->transform();
	instance.shader->render();
}
