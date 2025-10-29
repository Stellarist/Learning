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

glm::vec3 Camera::getPosition()
{
	return position;
}

glm::vec3 Camera::getFront()
{
	return front;
}

glm::mat4 Camera::getView()
{
	return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::getProjection()
{
	return glm::perspective(glm::radians(fov), aspect_ratio, 0.1f, 100.0f);
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

void Camera::updateCameraVectors()
{
	float x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	float y = sin(glm::radians(pitch));
	float z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(glm::vec3(x, y, z));
	right = glm::normalize(glm::cross(front, world_up));
	up = glm::normalize(glm::cross(right, front));
}
