#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

/**
 * @brief Enum class for camera movement directions.
 * @note This enum is used to specify the direction of camera movement in the scene.
 *       It is used in the Camera class to process keyboard input for moving the camera.
 */
enum class CameraMovement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

/**
 * @brief Camera class for managing camera position, orientation, and movement.
 * @note This class provides methods to control the camera's position, orientation,
 *       and movement speed. It also provides methods to process keyboard and mouse input.
 *       The camera can be used to render 3D scenes from different perspectives.
 */
struct Camera {
	/**
	 * @brief The position of the camera in world coordinates.
	 * @note This vector represents the camera's position in the 3D space.
	 */
	glm::vec3 position;

	/**
	 * @brief The direction the camera is facing.
	 * @note This vector represents the forward direction of the camera.
	 */
	glm::vec3 front;

	/**
	 * @brief The up direction of the camera.
	 * @note This vector represents the upward direction of the camera, used to calculate the view matrix.
	 */
	glm::vec3 up;

	/**
	 * @brief The right direction of the camera.
	 * @note This vector represents the rightward direction of the camera, calculated from the front and up vectors.
	 */
	glm::vec3 right;

	/**
	 * @brief The world up direction, used to calculate the camera's orientation.
	 * @note This vector represents the global upward direction, typically (0.0f, 1.0f, 0.0f).
	 */
	glm::vec3 world_up;

	/**
	 * @brief The speed of camera movement.
	 * @note This value determines how fast the camera moves when processing keyboard input.
	 */
	float movement_speed;

	/**
	 * @brief The sensitivity of mouse movement.
	 * @note This value determines how sensitive the camera is to mouse movements for orientation changes.
	 */
	float mouse_sensitivity;

	/**
	 * @brief The zoom level of the camera.
	 * @note This value determines the field of view of the camera, affecting how zoomed in or out the view is.
	 */
	float fov;

	/**
	 * @brief The aspect ratio of the camera.
	 * @note This value determines the width-to-height ratio of the camera's view.
	 */
	float aspect_ratio;

	/**
	 * @brief The yaw angle of the camera.
	 * @note This value represents the horizontal rotation of the camera, affecting its left/right orientation.
	 */
	float yaw;

	/**
	 * @brief The pitch angle of the camera.
	 * @note This value represents the vertical rotation of the camera, affecting its up/down orientation.
	 */
	float pitch;

	/**
	 * @brief Updates the camera's front, right, and up vectors based on the current yaw and pitch angles.
	 * @note This function recalculates the camera's orientation vectors to ensure they are always correct
	 *       after changes in yaw or pitch. It uses trigonometric functions to compute the new front vector
	 *       and then derives the right and up vectors from it.
	 */
	void updateCameraVectors();

	/**
	 * @brief Constructs a Camera object with specified position, up vector, yaw, and pitch.
	 * @param position The initial position of the camera in world coordinates.
	 * @param up The initial up vector of the camera, typically (0.0f, 1.0f, 0.0f).
	 * @param yaw The initial yaw angle of the camera, affecting its left/right orientation.
	 * @param pitch The initial pitch angle of the camera, affecting its up/down orientation.
	 */
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
	       glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
	       float     yaw = -90.0f,
	       float     pitch = 0.0f);

	glm::vec3 getPosition();
	glm::vec3 getFront();
	glm::mat4 getView();
	glm::mat4 getProjection();

	void processKeyboard(CameraMovement direction, float delta_time);
	void processMouseMovement(float x_ofs, float y_ofs, bool constrain_pitch = true);
	void processMouseScroll(float y_ofs);
};
