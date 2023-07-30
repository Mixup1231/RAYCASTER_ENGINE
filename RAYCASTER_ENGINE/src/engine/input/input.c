#include "input.h"

static Input states;
static bool constrained_cursor;

void input_init(void) {
	states.quit = false;
	constrained_cursor = false;

	for (u16 i = 0; i < SDL_NUM_SCANCODES; i++)
		states.keys[i] = INPUT_UNPRESSED;
	for (u16 i = 0; i < SDL_BUTTON_RIGHT; i++)
		states.mouse_buttons[i] = INPUT_UNPRESSED;
}

void input_poll_events(void) {
	u8* keyboard = SDL_GetKeyboardState(NULL);
	for (usize i = 0; i < SDL_NUM_SCANCODES; i++)
		if (keyboard[i] && states.keys[i] == INPUT_UNPRESSED)
			states.keys[i] = INPUT_PRESSED;
		else if (keyboard[i] && states.keys[i] == INPUT_PRESSED)
			states.keys[i] = INPUT_HELD;
		else if (!keyboard[i] && states.keys[i] == INPUT_HELD)
			states.keys[i] = INPUT_RELEASED;
		else if (!keyboard[i] && states.keys[i] == INPUT_RELEASED)
			states.keys[i] = INPUT_UNPRESSED;

	u32 button_mask = SDL_GetMouseState(&states.mouse_x, &states.mouse_y);
	for (u16 i = 1; i < SDL_BUTTON_RIGHT; i++)
		if (button_mask << SDL_BUTTON(i) && states.mouse_buttons[i] == INPUT_UNPRESSED)
			states.mouse_buttons[i] = INPUT_PRESSED;
		else if (button_mask << SDL_BUTTON(i) && states.mouse_buttons[i] == INPUT_PRESSED)
			states.mouse_buttons[i] = INPUT_HELD;
		else if (!button_mask << SDL_BUTTON(i) && states.mouse_buttons[i] == INPUT_HELD)
			states.mouse_buttons[i] = INPUT_RELEASED;
		else if (!button_mask << SDL_BUTTON(i) && states.mouse_buttons[i] == INPUT_RELEASED)
			states.mouse_buttons[i] = INPUT_UNPRESSED;

	states.rel_mouse_x = 0;
	states.rel_mouse_y = 0;
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			states.quit = true;
			break;
		case SDL_MOUSEMOTION:
			states.rel_mouse_x = event.motion.xrel;
			states.rel_mouse_y = event.motion.yrel;
			break;
		}
	}
}

InputState input_get_key_state(SDL_Scancode code) {
	return states.keys[code];
}

bool input_is_key_pressed(SDL_Scancode code) {
	return states.keys[code] == INPUT_PRESSED;
}

bool input_is_key_held(SDL_Scancode code) {
	return states.keys[code] == INPUT_HELD;
}

bool input_is_key_released(SDL_Scancode code) {
	return states.keys[code] == INPUT_RELEASED;
}

bool input_is_key_unpressed(SDL_Scancode code) {
	return states.keys[code] == INPUT_UNPRESSED;
}

InputState input_get_mouse_state(u16 button) {
	return states.mouse_buttons[button];
}
bool input_is_mouse_pressed(u16 button) {
	return states.mouse_buttons[button] == INPUT_PRESSED;
}

bool input_is_mouse_held(u16 button) {
	return states.mouse_buttons[button] == INPUT_HELD;
}

bool input_is_mouse_released(u16 button) {
	return states.mouse_buttons[button] == INPUT_RELEASED;
}

bool input_is_mouse_unpressed(u16 button) {
	return states.mouse_buttons[button] == INPUT_UNPRESSED;
}

void input_get_mouse_position(u32* x, u32* y) {
	if (x)
		*x = states.mouse_x;
	if (y)
		*y = states.mouse_y;
}

void input_get_mouse_motion(u32* x, u32* y) {
	if (x)
		*x = states.rel_mouse_x;
	if (y)
		*y = states.rel_mouse_y;
}

bool input_get_quit_state(void) {
	return states.quit;
}

void input_constrain_cursor(void) {
	constrained_cursor = !constrained_cursor;
	SDL_SetRelativeMouseMode(constrained_cursor);
}