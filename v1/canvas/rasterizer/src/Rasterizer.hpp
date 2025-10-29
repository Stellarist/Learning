#pragma once

#include <vector>

#include "global.hpp"
#include "Shader.hpp"

class Rasterizer {
private:
public:
	int                  width, height;
	std::vector<vec3f_t> frame_buffer;
	std::vector<float>   z_buffer;
	mat4f_t              model;
	mat4f_t              view;
	mat4f_t              projection;

public:
	Rasterizer() = default;
	Rasterizer(int width, int height);

	void  clear();
	void  clear(vec3f_t color);
	void  resize(int width, int height);
	void  setPixel(int x, int y, const vec3f_t& color);
	void  setPixel(const vec3f_t& point, const vec3f_t& color);
	bool  setDepth(int x, int y, int z);
	int   getIndex(int x, int y) const;
	void* getFramebufferData();

	void drawPoint(const vec3f_t& point, const vec3f_t& color = {0.f, 0.f, 0.f});
	void drawLine(const std::array<vec3f_t, 2>& line, const vec3f_t& color = {0.f, 0.f, 0.f});
	void drawTriangle(const triangle_t& triangle, ShaderInfo& shader);

	static bool isInsideTriangle(int x, int y, const triangle_t& triangle);
	static bool isTriangleBackface(const triangle_t& triangle);
	static auto computeBarycentric(int x, int y, const triangle_t& triangle) -> std::tuple<float, float, float>;

	template <typename T>
	auto interpolate(float alpha, float beta, float gamma, const T& v0, const T& v1, const T& v2) -> T
	{
		return alpha * v0 + beta * v1 + gamma * v2;
	}
};
