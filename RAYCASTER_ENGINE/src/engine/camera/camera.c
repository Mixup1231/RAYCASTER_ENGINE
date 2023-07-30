#include <assert.h>

#include "camera.h"
#include "../util.h"

Camera camera_create(f32 x, f32 y, f32 angle, f32 fov, f32 view_width, f32 view_height) {
	Camera camera = {
		.position = {x, y},
		.angle = angle,
		.fov = fov,
		.view_width = view_width,
		.height_centre = view_height / 2,
	};
	camera.proj_plane_dist = camera.height_centre / tanf(RADIANS(fov / 2));

	return camera;
}

void camera_move(Camera* camera, CameraDirection direction, u32 units) {
	assert(camera);

	switch (direction) {
	case CAMERA_FORWARD:
		camera->position[0] += cos(camera->angle) * units;
		camera->position[1] += sin(camera->angle) * units;
		break;
	case CAMERA_BACK:
		camera->position[0] -= cos(camera->angle) * units;
		camera->position[1] -= sin(camera->angle) * units;
		break;
	case CAMERA_LEFT:
		camera->position[0] += cos(camera->angle - RADIANS(90)) * units;
		camera->position[1] += sin(camera->angle - RADIANS(90)) * units;
		break;
	case CAMERA_RIGHT:
		camera->position[0] += cos(camera->angle + RADIANS(90)) * units;
		camera->position[1] += sin(camera->angle + RADIANS(90)) * units;
		break;
	default:
		return;
	}
}

void camera_rotate(Camera* camera, f32 offset) {
	assert(camera);

	camera->angle -= offset;
	if (DEGREES(camera->angle) < 0)
		camera->angle += 2 * PI;
	else if (DEGREES(camera->angle) > 360)
		camera->angle -= 2 * PI;
}