#pragma once

#include <glad/glad.h>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include "../types.h"

SDL_Window* render_init_window(u32 width, u32 height, const char* window_name);
u32 render_init_shaders(const char* vertex_path, const char* fragment_path);
void render_init_texture_default(u32* texture_id);
void render_init_quad(u32* vbo_quad, u32* ebo_quad, u32* vao_quad);
void render_init_line(u32* vbo_line, u32* vao_line);
