#pragma once
#include <linmath.h>
#include <stdbool.h>

#include "../camera/camera.h"
#include "../level/level.h"

#define RAY_DOF 8
#define RAY_OBJECT_INTENSITY 1

typedef struct {
	vec2 position;
	vec2 size;
	vec2 uv_coords;
	bool vertical;
	u32 texture;
	f32 intensity;
	vec2 normal;
} Slice;

typedef struct {
	Slice* items;
	f32 distances[CAMERA_MAX_FOV];
	usize length;
} SliceArray;

SliceArray ray_slice_array_create(const Camera* camera);

typedef struct {
	bool hit;
	vec2 normal;
	vec2 intersection;
} RayCollision;

RayCollision ray_get_collision(const Level* level, const vec2 position, f32 angle, f32 magnitude);

void ray_set_resolution_scale(u32 scale);
f32 wrap_angle(f32 angle);
void ray_resolve_collision(RayCollision collision, vec2 velocity);
void ray_cast_level(const Camera* camera, const Level* level, SliceArray* out_slices);
void ray_render_level(const SliceArray* slices, const vec2 offset);