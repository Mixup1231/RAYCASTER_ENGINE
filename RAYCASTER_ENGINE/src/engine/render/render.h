#pragma once
#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <linmath.h>
#include <stdbool.h>

#include "../types.h"

typedef struct {
	u32 id;
	u32 width;
	u32 height;
} Texture;

Texture render_load_texture(const char* path, bool flip);

SDL_Window* render_init(u32 width, u32 height, const char* window_name);
void render_begin(void);
void render_end(void);
u32 render_get_window_width(void);
u32 render_get_window_height(void);

void render_quad(const vec2 position, const vec2 size, const vec4 colour, const u32 texture);
void render_slice(const vec2 position, const vec2 size, const vec4 colour, const u32 texture, vec2 uv_shift);
void render_line(const vec2 start, const vec2 end, const vec4 colour, const u32 line_width);