#pragma once

#include <random>
#include <eigen3/Eigen/Eigen>

constexpr float PI = 3.14159265358979323846;

using vec2f_t = Eigen::Vector2f;
using vec3f_t = Eigen::Vector3f;
using vec4f_t = Eigen::Vector4f;
using mat4f_t = Eigen::Matrix4f;
using mat3f_t = Eigen::Matrix3f;

namespace Geometry
{
inline float radians(float deg)
{
	return deg * PI / 180.0f;
}

inline vec3f_t lerp(const vec3f_t& a, const vec3f_t& b, float t)
{
	return a * (1 - t) + b * t;
}

inline mat4f_t translate(const mat4f_t& mat, const vec3f_t& vec)
{
	mat4f_t res = mat;
	res(0, 3) += vec[0];
	res(1, 3) += vec[1];
	res(2, 3) += vec[2];

	return res;
}

inline mat4f_t rotate(const mat4f_t& mat, float deg, const vec3f_t& vec)
{
	float   norm = vec.norm();
	vec3f_t rot = vec / norm;

	mat3f_t N;
	N << 0, -rot[2], rot[1],
	    rot[2], 0, -rot[0],
	    -rot[1], rot[0], 0;
	mat3f_t I = mat3f_t::Identity();
	mat3f_t R = I * cos(deg) + rot * rot.transpose() * (1 - cos(deg)) + N * sin(deg);
	mat4f_t res = mat;
	res.block(0, 0, 3, 3) = R * mat.block(0, 0, 3, 3);

	return res;
}

inline mat4f_t scale(const mat4f_t& mat, const vec3f_t& vec)
{
	mat4f_t res = mat;
	res(0, 0) *= vec[0];
	res(1, 1) *= vec[1];
	res(2, 2) *= vec[2];

	return res;
}

inline mat4f_t lookAt(const vec3f_t& eye, const vec3f_t& center, const vec3f_t& up)
{
	vec3f_t f = (center - eye).normalized();
	vec3f_t s = f.cross(up).normalized();
	vec3f_t u = s.cross(f);

	mat4f_t res;
	res << s.x(), s.y(), s.z(), -s.dot(eye),
	    u.x(), u.y(), u.z(), -u.dot(eye),
	    -f.x(), -f.y(), -f.z(), f.dot(eye),
	    0.f, 0.f, 0.f, 1.f;

	return res;
}

inline mat4f_t perspective(float fovy, float aspect, float zNear, float zFar)
{
	float   tan_half_fovy = tan(radians(fovy / 2));
	mat4f_t res;
	res << 1.f / (aspect * tan_half_fovy), 0.f, 0.f, 0.f,
	    0.f, 1.f / tan_half_fovy, 0.f, 0.f,
	    0.f, 0.f, -(zFar + zNear) / (zFar - zNear), -2 * zFar * zNear / (zFar - zNear),
	    0.f, 0.f, -1.f, 0.f;

	return res;
}

inline mat4f_t ortho(float left, float right, float bottom, float top, float zNear, float zFar)
{
	mat4f_t res;
	res << 2.f / (right - left), 0.f, 0.f, -(right + left) / (right - left),
	    0.f, 2.f / (top - bottom), 0.f, -(top + bottom) / (top - bottom),
	    0.f, 0.f, -2.f / (zFar - zNear), -(zFar + zNear) / (zFar - zNear),
	    0.f, 0.f, 0.f, 1.f;

	return res;
}

inline float randomFloat()
{
	std::random_device                    dev;
	std::mt19937                          rng(dev());
	std::uniform_real_distribution<float> dist(0.0f, 1.0f);

	return dist(rng);
}

inline bool solveQuadratic(const float& a, const float& b, const float& c, float& x0, float& x1)
{
	float delta = b * b - 4 * a * c;
	if (delta < 0)
		return false;
	if (delta == 0) {
		x0 = x1 = -0.5 * b / a;
		return true;
	}

	float q = (b > 0) ? -0.5 * (b + sqrt(delta)) : -0.5 * (b - sqrt(delta));
	x0 = q / a;
	x1 = c / q;
	if (x0 > x1)
		std::swap(x0, x1);

	return true;
}
};        // namespace Geometry
