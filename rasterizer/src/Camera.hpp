#pragma once

#include "global.hpp"

enum class CameraMovement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

class Camera {
private:
	vec3f_t position;
	vec3f_t front;
	vec3f_t up;
	vec3f_t right;
	vec3f_t world_up;

	float movement_speed;
	float mouse_sensitivity;

	float zoom;
	float yaw;
	float pitch;
	float fov;

	void updateCameraVectors();

public:
	Camera(vec3f_t position = vec3f_t(0.0f, 0.0f, 0.0f),
	       vec3f_t up = vec3f_t(0.0f, 1.0f, 0.0f),
	       float   yaw = -90.0f,
	       float   pitch = 0.0f);
	Camera(float x_pos, float y_pos, float z_pos,
	       float x_up, float y_up, float z_up,
	       float yaw, float pitch);

	vec3f_t getPosition();
	vec3f_t getFront();
	mat4f_t getView();
	mat4f_t getProjection();

	void processKeyboard(CameraMovement direction, float delta_time);
	void processMouseMovement(float x_ofs, float y_ofs, bool constrain_pitch = true);
	void processMouseScroll(float y_ofs);
};

inline vec3f_t Camera::getPosition()
{
	return position;
}

inline vec3f_t Camera::getFront()
{
	return front;
}

inline mat4f_t Camera::getView()
{
	return Geometry::lookAt(position, position + front, up);
}

inline mat4f_t Camera::getProjection()
{
	return Geometry::perspective(Geometry::radians(zoom), fov, 0.1f, 100.0f);
}
