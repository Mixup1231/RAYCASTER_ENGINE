#include <assert.h>
#include <linmath.h>
#include <stdlib.h>

#include "../render/render.h"
#include "level.h"
#include "../util.h"

Level level_create(usize rows, usize columns, i64 width) {
	Level level = {
		.rows = rows,
		.columns = columns,
		.width = width,
		.slice_width = sqrtf(width),
		.uv_width = 1 / width
	};
	level.data = calloc(rows * width, sizeof(u32));
	assert(level.data);

	return level;
}

void level_free(Level* level) {
	assert(level);

	if (level->data)
		free(level->data);
}

void level_render_top_down(const Level* level, const vec2 offset, f32 scale, const vec4 wall_colour, const vec4 no_wall_colour) {
	assert(level->data);

	for (size_t i = 0; i < level->rows; i++) {
		for (size_t j = 0; j < level->columns; j++) {
			if (level->data[i * level->columns + j])
				render_quad((vec2) { (j * level->width + offset[0]) * scale, (i * level->width + offset[1])* scale }, (vec2) { level->width* scale, level->width* scale }, wall_colour, 0);
			else
				render_quad((vec2) { (j * level->width + offset[0]) * scale, (i * level->width + offset[1])* scale }, (vec2) { level->width* scale, level->width* scale }, no_wall_colour, 0);
		}
	}
}