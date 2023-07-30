#pragma once
#include "../types.h"

typedef struct {
	u32* data;
	u64 rows;
	u64 columns;
	i64 width;
	f32 slice_width;
	f32 uv_width;
} Level;

Level level_create(usize rows, usize columns, i64 width);
void level_free(Level* level);
void level_render_top_down(const Level* level, const vec2 offset, f32 scale, const vec4 wall_colour, const vec4 no_wall_colour);