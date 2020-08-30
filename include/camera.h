#pragma once

#ifndef _CAMERA
#define _CAMERA
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum PROJECTION {
	PROJ_ORTH,
	PROJ_PERS,
};

const int default_width = 800;
const int default_height = 600;
const float default_fov_radians = 45.0f;

class Camera {
public:
	Camera() :
		near_plane(0.1f),
		far_plane(100.f),
		ortho_left(default_width / -2.0f),
		ortho_right(default_width / 2.0f),
		ortho_buttom(default_height / -2.0f),
		ortho_top(default_height / 2.0f),
		projection_type(PROJECTION::PROJ_PERS),
		fov(glm::radians(default_fov_radians)),
		aspect(float(default_width) / float(default_height)),
		up(glm::vec3(0, 1, 0)),
		position(glm::vec3(0, 0, 0)),
		pitch(0),
		yaw(0)
	{

	};
	~Camera() {};

	glm::mat4 GetProjectionMat() {
		if (projection_type == PROJECTION::PROJ_PERS)
			return glm::perspective(fov, aspect, near_plane, far_plane);
		else
			return glm::ortho(ortho_left, ortho_right, ortho_buttom, ortho_top, near_plane, far_plane);
	}

	glm::mat4 GetViewMat() {
		glm::vec3 forward = GetForward();
		return glm::lookAt(position, position + forward, up);
	}

	glm::vec3 GetForward() {
		glm::vec3 forward = glm::vec3(cos(glm::radians(pitch)) * cos(glm::radians(yaw)),
										sin(glm::radians(pitch)),
										cos(glm::radians(pitch)) * sin(glm::radians(yaw)));
		return glm::normalize(forward);
	}

	void SetProjType(PROJECTION type) {
		projection_type = type;
	}

	//perspective projection
	float fov;
	float aspect; // width / height
	float near_plane;
	float far_plane;

	//ortho projection
	float ortho_left;
	float ortho_right;
	float ortho_buttom;
	float ortho_top;

	PROJECTION projection_type;
	glm::vec3 position;
	glm::vec3 up;

	float pitch;
	float yaw;
};

#endif
