#include <assert.h>
#include <math.h>

#include "ray.h"
#include "../render/render.h"
#include "../util.h"

static u32 resolution_scale = 2;

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

SliceArray ray_slice_array_create(const Camera* camera) {
	assert(camera);

	SliceArray slice_array = {
		.length = camera->fov * resolution_scale,
	};
	slice_array.items = calloc(camera->fov * resolution_scale, sizeof(Slice));
	assert(slice_array.items);

	return slice_array;
}

void ray_set_resolution_scale(u32 scale) {
	resolution_scale = scale;
}

f32 wrap_angle(f32 angle) {
	if (angle > TWO_PI)
		angle -= TWO_PI;
	else if (angle < 0)
		angle += TWO_PI;

	return angle;
}

static void ray_cast(const Level* level, const vec2 start, f32 angle, u32 dof) {
	f32 tan_positive = tanf(angle);
	f32 tan_negative = tanf(-angle);

	if (angle > 0 && angle < PI) {
		ay = ((u64)(start[1] / level->width)) * level->width + level->width;
		y_offset = level->width;
		x_offset = level->width / tan_positive;
	}
	else if (angle < TWO_PI && angle > PI) {
		ay = ((u64)(start[1] / level->width)) * level->width - 0.0001;
		y_offset = -level->width;
		x_offset = level->width / tan_negative;
	}

	ax = start[0] + (start[1] - ay) / tan_negative;

	x = ax / level->width;
	y = ay / level->width;

	for (usize i = 0; i < dof; i++)
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
		(start[0] - ax) * (start[0] - ax) +
		(start[1] - ay) * (start[1] - ay)
	);

	hax = ax;
	hay = ay;

	if (angle < PI / 2 || angle > THREE_PI_ON_TWO) {
		ax = ((u64)(start[0] / level->width)) * level->width + level->width;
		x_offset = level->width;
		y_offset = level->width * tan_positive;
	}
	else if (angle > PI / 2 && angle < THREE_PI_ON_TWO) {
		ax = ((u64)(start[0] / level->width)) * level->width - 0.0001;
		x_offset = -level->width;
		y_offset = level->width * tan_negative;
	}

	ay = start[1] + (start[0] - ax) * tan_negative;

	x = ax / level->width;
	y = ay / level->width;

	for (usize i = 0; i < dof; i++)
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
		(start[0] - ax) * (start[0] - ax) +
		(start[1] - ay) * (start[1] - ay)
	);
	vax = ax;
	vay = ay;
}

RayCollision ray_get_collision(const Level* level, const vec2 position, f32 angle, f32 magnitude) {
	assert(level);

	RayCollision collision;

	// consider making this a table to optimise
	u32 dof;
	f32 min_mag = sqrtf(level->width * level->width * 2);
	if (magnitude <= min_mag)
		dof = 1;
	else
		dof = magnitude / min_mag;
	ray_cast(level, position, angle, dof);
	f32 distance;
	
	if (vert_dist < horiz_dist) {
		ax = vax;
		ay = vay;
		distance = vert_dist;
	} else {
		ax = hax;
		ay = hay;
		distance = horiz_dist;
	}
	collision.intersection[0] = ax;
	collision.intersection[1] = ay;

	if (vert_dist < horiz_dist) {
		if (position[0] < ax)
			collision.normal[0] = -1;
		else
			collision.normal[0] = 1;
		collision.normal[1] = 0;
	} else {
		if (position[1] < ay)
			collision.normal[1] = -1;
		else
			collision.normal[1] = 1;
		collision.normal[0] = 0;
	}

	collision.hit = (collision.normal[0] || collision.normal[1]) && (magnitude >= distance);

	return collision;
}

void ray_resolve_collision(RayCollision collision, vec2 velocity) {
	if (collision.hit) {
		if (collision.normal[0])
			velocity[0] = 0;
		if (collision.normal[1])
			velocity[1] = 0;
	}
}

void ray_cast_level(const Camera* camera, const Level* level, SliceArray* out_slices) {
	assert(camera && level && out_slices && out_slices->items);

	f32 scaled_slice_width = camera->view_width / (level->slice_width * camera->fov) * level->slice_width;
	scaled_slice_width /= resolution_scale;

	start_angle = camera->angle - RADIANS(camera->fov / 2);
	for (size_t i = 0; i < camera->fov * resolution_scale; i++) {
		ray_angle = wrap_angle(start_angle + RADIANS((f32)i / resolution_scale));
		ray_cast(level, camera->position, ray_angle, RAY_DOF);

		f32 distance = vert_dist < horiz_dist ? vert_dist * cosf(camera->angle - ray_angle) : horiz_dist * cosf(camera->angle - ray_angle);
		f32 proj_height = level->width / distance * camera->proj_plane_dist;
		proj_height *= resolution_scale;

		out_slices->items[i].vertical = vert_dist < horiz_dist;

		if (out_slices->items[i].vertical) {
			if (camera->position[0] < ax)
				out_slices->items[i].normal[0] = -1;
			else
				out_slices->items[i].normal[0] = 1;
			out_slices->items[i].normal[1] = 0;
		}
		else {
			if (camera->position[1] < ay)
				out_slices->items[i].normal[1] = -1;
			else
				out_slices->items[i].normal[1] = 1;
			out_slices->items[i].normal[0] = 0;
		}

		out_slices->distances[i] = distance;

		out_slices->items[i].position[0] = i * scaled_slice_width;
		out_slices->items[i].position[1] = camera->height_centre;

		out_slices->items[i].size[0] = scaled_slice_width;
		out_slices->items[i].size[1] = proj_height;

		out_slices->items[i].uv_coords[0] = (out_slices->items[i].vertical ?  fmodf(vay, level->width) : fmodf(hax, level->width)) / (f32)level->width;
		out_slices->items[i].uv_coords[1] = out_slices->items[i].uv_coords[0] + level->uv_width;

		out_slices->items[i].texture = out_slices->items[i].vertical ? level->data[vy * level->columns + vx] : level->data[hy * level->columns + hx];
		if (out_slices->items[i].texture == 0)
			out_slices->items[i].texture = 1;

		out_slices->items[i].intensity = RAY_OBJECT_INTENSITY / distance * (level->width - (level->width >> 2));
		CLAMP(out_slices->items[i].intensity, 1);
	}
}

void ray_render_level(const SliceArray* slices, const vec2 offset) {
	assert(slices && slices->items);

	vec2 position;
	for (usize i = 0; i < slices->length; i++) {
		vec2_scale(position, position, 0);
		vec2_add(position, slices->items[i].position, offset);
		if (slices->items[i].vertical)
			render_slice(position, slices->items[i].size, WHITE, slices->items[i].texture, slices->items[i].uv_coords, GREY, slices->items[i].intensity);
		else
			render_slice(position, slices->items[i].size, GREY, slices->items[i].texture, slices->items[i].uv_coords, GREY, slices->items[i].intensity);
	}
}