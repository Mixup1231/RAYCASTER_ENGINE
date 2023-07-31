#include <assert.h>

#include "camera.h"
#include "../util.h"

Camera camera_create(f32 x, f32 y, f32 angle, f32 fov, f32 view_width, f32 view_height) {
	assert(0 <= fov && fov <= CAMERA_MAX_FOV);

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

void camera_move(Camera* camera, const vec2 velocity) {
	assert(camera);

	camera->position[0] += velocity[0];
	camera->position[1] += velocity[1];
}

void camera_rotate(Camera* camera, f32 offset) {
	assert(camera);

	camera->angle -= offset;
	if (camera->angle < 0)
		camera->angle += 2 * PI;
	else if (camera->angle > TWO_PI)
		camera->angle -= 2 * PI;
}
