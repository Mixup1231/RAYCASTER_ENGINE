#pragma once
#include <linmath.h>

#include "../types.h"

typedef enum {
	CAMERA_FORWARD,
	CAMERA_BACK,
	CAMERA_LEFT,
	CAMERA_RIGHT,
} CameraDirection;

typedef struct {
	vec2 position;
	f32 angle;
	f32 fov;
	f32 view_width;
	f32 height_centre;
	f32 proj_plane_dist;
} Camera;

Camera camera_create(f32 x, f32 y, f32 angle, f32 fov, f32 view_width, f32 view_height);
void camera_move(Camera* camera, CameraDirection direction, u32 units);
void camera_rotate(Camera* camera, f32 offset);