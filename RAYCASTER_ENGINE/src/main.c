#include <stdio.h>
#include <stdbool.h>

#include "engine/util.h"
#include "engine/render/render.h"
#include "engine/level/level.h"
#include "engine/input/input.h"
#include "engine/camera/camera.h"
#include "engine/ray/ray.h"

u32 level_data[] = {
	2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
	2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,
	2,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3,0,0,0,2,
	2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,
	2,3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,
	2,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3,0,0,0,2,
	2,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,2,
	2,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,2,
	2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
};
Level level;

/*
TODO:
	fix level struct (normals)
	beautify / optimise ray casting function
	serialise and deserialise levels
	collision
	fog colour
	variable height
	floor
	roof
*/

int main() {
	f32 width = 1920, height = 1080;
	render_init(width, height, "GAME");
	input_init();
	input_constrain_cursor();

	level = level_create(9, 20, 64);
	memcpy(level.data, level_data, sizeof(level_data));
	Camera player = camera_create(500, 500, RADIANS(90), 75, width, height);
	SliceArray slices = ray_slice_array_create(&player, &level);

	Texture fence = render_load_texture("assets/metal_wall.png", false);
	Texture wall = render_load_texture("assets/sandstone_wall.png", false);
	Texture gun = render_load_texture("assets/gun_fps.png", false);
	gun.width *= width / gun.width / 1.5;
	gun.height *= height / gun.height / 1.5;	
	
	i32 rel_x = 0;
	while (input_get_quit_state() == false) {
		input_poll_events();

		render_begin();

		render_quad((vec2) { width / 2, height }, (vec2) { width, height }, (vec4){0.2, 0.2, 0.1, 1}, 0);

		if (input_get_key_state(SDL_SCANCODE_W) == INPUT_HELD)
			camera_move(&player, CAMERA_FORWARD, 1);
		if (input_get_key_state(SDL_SCANCODE_S) == INPUT_HELD)
			camera_move(&player, CAMERA_BACK, 1);
		if (input_get_key_state(SDL_SCANCODE_A) == INPUT_HELD)
			camera_move(&player, CAMERA_LEFT, 1);
		if (input_get_key_state(SDL_SCANCODE_D) == INPUT_HELD)
			camera_move(&player, CAMERA_RIGHT, 1);
		
		input_get_mouse_motion(&rel_x, NULL);
		camera_rotate(&player, RADIANS(-rel_x * 0.5));

		ray_cast_level(&player, &level, &slices);
		ray_render_level(&slices, (vec2){0, 0});
		render_quad((vec2) { width / 2, height / 2 }, (vec2) { 10, 10 }, (vec4) { 1, 1, 1, 0.5 }, 0);
		render_quad((vec2) { width - gun.width / 2, height - gun.height / 2 }, (vec2) { gun.width, gun.height }, WHITE, gun.id);
	
		render_end();
	}

	level_free(&level);
	free(slices.items);
}