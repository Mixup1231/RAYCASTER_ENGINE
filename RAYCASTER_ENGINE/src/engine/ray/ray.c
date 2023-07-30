#include <assert.h>
#include <math.h>

#include "ray.h"
#include "../render/render.h"
#include "../util.h"

static f32 start_angle;
static f32 ray_angle;

static f32 ay;
static f32 ax;

static f32 y_offset;
static f32 x_offset;

static usize x;
static usize y;
static usize vx;
static usize vy;
static usize hx;
static usize hy;

static f32 hax;
static f32 hay;
static f32 vax;
static f32 vay;

static f32 horiz_dist;
static f32 vert_dist;

static f32 wrap_angle(f32 angle) {
	if (angle >  PI * 2)
		angle -= PI * 2;
	if (angle < 0)
		angle += PI * 2;

	return angle;
}

SliceArray ray_slice_array_create(const Camera* camera) {
	assert(camera);

	SliceArray slice_array = {
		.length = camera->fov,
	};
	slice_array.items = calloc(camera->fov, sizeof(Slice));
	assert(slice_array.items);

	return slice_array;
}

void ray_cast_level(const Camera* camera, const Level* level, SliceArray* out_slices) {
	assert(camera && level && out_slices && out_slices->items);

	start_angle = camera->angle - RADIANS(camera->fov / 2);

	for (size_t i = 0; i < camera->fov; i++) {
		ray_angle = wrap_angle(start_angle + RADIANS(i));

		if (ray_angle >= 0 && ray_angle <= PI) {
			ay = ((u64)(camera->position[1] / level->width)) * level->width + level->width;
			y_offset = level->width;
			x_offset = level->width / tanf(ray_angle);
		} else if (ray_angle <= RADIANS(360) && ray_angle > PI) {	
			ay = ((u64)(camera->position[1] / level->width)) * level->width - 1;
			y_offset = -level->width;
			x_offset = level->width / tanf(-ray_angle);
		}

		ax = camera->position[0] + (camera->position[1] - ay) / tanf(-ray_angle);

		x = ax / level->width;
		y = ay / level->width;

		for (usize i = 0; i < RAY_DOF; i++)
			if (0 <= x && x < level->columns && 0 <= y && y < level->rows)
				if (level->data[y * level->columns + x])
					break;
				else {
					ax += x_offset;
					x = ax / level->width;
					ay += y_offset;
					y = ay / level->width;
				}
		hx = x;
		hy = y;

		horiz_dist = sqrtf(
			(camera->position[0] - ax) * (camera->position[0] - ax) +
			(camera->position[1] - ay) * (camera->position[1] - ay)
		);

		hax = ax;
		hay = ay;

		if (ray_angle < PI / 2 || ray_angle > RADIANS(270)) {
			ax = ((u64)(camera->position[0] / level->width)) * level->width + level->width;
			x_offset = level->width;
			y_offset = level->width * tanf(ray_angle);
		} else if (ray_angle > PI / 2 && ray_angle < RADIANS(270)) {
			ax = ((u64)(camera->position[0] / level->width)) * level->width - 1;
			x_offset = -level->width;
			y_offset = level->width * tanf(- ray_angle);
		}

		ay = camera->position[1] + (camera->position[0] - ax) * tanf(-ray_angle);

		x = ax / level->width;
		y = ay / level->width;

		for (usize i = 0; i < RAY_DOF; i++)
			if (0 <= x && x < level->columns && 0 <= y && y < level->rows)
				if (level->data[y * level->columns + x])
					break;
				else {
					ax += x_offset;
					x = ax / level->width;
					ay += y_offset;
					y = ay / level->width;
				}
		vx = x;
		vy = y;

		vert_dist = sqrtf(
			(camera->position[0] - ax) * (camera->position[0] - ax) +
			(camera->position[1] - ay) * (camera->position[1] - ay)
		);
		vax = ax;
		vay = ay;

		f32 distance = vert_dist < horiz_dist ? vert_dist * cosf(camera->angle - ray_angle) : horiz_dist * cosf(camera->angle - ray_angle);
		f32 proj_height;
		if (vert_dist <= horiz_dist)
			proj_height = level->width / distance * camera->proj_plane_dist;
		else
			proj_height = level->width / distance * camera->proj_plane_dist;

		f32 scaled_slice_width = camera->view_width / (level->slice_width * camera->fov) * level->slice_width;
		out_slices->items[i].position[0] = i * scaled_slice_width;
		out_slices->items[i].position[1] = camera->height_centre;

		out_slices->items[i].size[0] = scaled_slice_width;
		out_slices->items[i].size[1] = proj_height;

		out_slices->items[i].uv_coords[0] = (vert_dist < horiz_dist ?  fmodf(vay, level->width) : fmodf(hax, level->width)) / (f32)level->width;
		out_slices->items[i].uv_coords[1] = out_slices->items[i].uv_coords[0] + level->uv_width;
		out_slices->items[i].vertical = vert_dist < horiz_dist;

		out_slices->items[i].texture = vert_dist < horiz_dist ? level->data[vy * level->columns + vx] : level->data[hy * level->columns + hx];
		if (out_slices->items[i].texture == 0)
			out_slices->items[i].texture = 1;

		out_slices->items[i].intensity = RAY_OBJECT_INTENSITY / distance * RAY_MULTIPLIER;
		CLAMP(out_slices->items[i].intensity, 1);
	}
}

void ray_render_level(const SliceArray* slices, const vec2 offset) {
	assert(slices && slices->items);

	vec2 position;
	for (usize i = 0; i < slices->length; i++) {
		vec2_scale(position, position, 0);
		vec2_add(position, slices->items[i].position, offset);

		if (slices->items[i].vertical) {
			vec4 colour;
			if (slices->items[i].texture == 1)
				vec4_scale(colour, BLACK, slices->items[i].intensity);
			else
				vec4_scale(colour, WHITE, slices->items[i].intensity);
			colour[3] = 1;
			render_slice(position, slices->items[i].size, colour, slices->items[i].texture, slices->items[i].uv_coords);
		} else {
			vec4 colour;
			if (slices->items[i].texture == 1)
				vec4_scale(colour, BLACK, slices->items[i].intensity);
			else
				vec4_scale(colour, GREY, slices->items[i].intensity);
			colour[3] = 1;
			render_slice(position, slices->items[i].size, colour, slices->items[i].texture, slices->items[i].uv_coords);
		}
	}
}