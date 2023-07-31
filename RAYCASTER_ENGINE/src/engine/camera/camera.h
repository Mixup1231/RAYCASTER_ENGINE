#pragma once
#include <linmath.h>

#include "../types.h"

#define CAMERA_MAX_FOV 360

typedef struct {
	vec2 position;
	f32 angle;
	f32 fov;
	f32 view_width;
	f32 height_centre;
	f32 proj_plane_dist;
} Camera;

Camera camera_create(f32 x, f32 y, f32 angle, f32 fov, f32 view_width, f32 view_height);
void camera_move(Camera* camera, vec2 velocity);
void camera_rotate(Camera* camera, f32 offset);