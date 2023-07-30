#pragma once
#include <linmath.h>
#include <stdbool.h>

#include "../camera/camera.h"
#include "../level/level.h"

#define RAY_DOF 8
#define RAY_OBJECT_INTENSITY 1
#define RAY_MULTIPLIER 64

typedef struct {
	vec2 position;
	vec2 size;
	vec2 uv_coords;
	bool vertical;
	u32 texture;
	f32 intensity;
} Slice;

typedef struct {
	Slice* items;
	usize length;
} SliceArray;

SliceArray ray_slice_array_create(const Camera* camera);
void ray_cast_level(const Camera* camera, const Level* level, SliceArray* out_slices);
void ray_render_level(const SliceArray* slices, const vec2 offset);