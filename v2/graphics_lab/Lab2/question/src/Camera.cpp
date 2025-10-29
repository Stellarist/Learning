#include "Camera.hpp"

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) :
    position(position),
    front(glm::vec3(0.0f, 0.0f, -1.0f)),
    world_up(up),
    movement_speed(2.5f),
    mouse_sensitivity(0.1f),
    fov(45.0f),
    aspect_ratio(16.f / 9.f),
    yaw(yaw),
    pitch(pitch)
{
	updateCameraVectors();
}

// TODO: Implement the view matrix calculation
glm::mat4 Camera::getView()
{
}

// TODO: Implement the projection matrix calculation
glm::mat4 Camera::getProjection()
{
}

void Camera::processKeyboard(CameraMovement direction, float delta_time)
{
	float velocity = movement_speed * delta_time;
	switch (direction) {
	case CameraMovement::FORWARD:
		position += front * velocity;
		break;
	case CameraMovement::BACKWARD:
		position -= front * velocity;
		break;
	case CameraMovement::LEFT:
		position -= right * velocity;
		break;
	case CameraMovement::RIGHT:
		position += right * velocity;
		break;
	}
}

void Camera::processMouseMovement(float x_ofs, float y_ofs, bool constrain_pitch)
{
	x_ofs *= mouse_sensitivity;
	y_ofs *= mouse_sensitivity;

	yaw += x_ofs;
	pitch += y_ofs;

	if (constrain_pitch) {
		if (pitch > 89.0f)
			pitch = 89.0f;
		else if (pitch < -89.0f)
			pitch = -89.0f;
	}

	updateCameraVectors();
}

void Camera::processMouseScroll(float y_ofs)
{
	fov -= (float) y_ofs;
	if (fov < 1.0f)
		fov = 1.0f;
	if (fov > 45.0f)
		fov = 45.0f;
}

// TODO: Recalculate the variables based on yaw and pitch (includes front, right, and up vectors)
void Camera::updateCameraVectors()
{
}
