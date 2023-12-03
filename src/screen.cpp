#include "screen.h"

#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_video.h>
#include <iostream>
#include <SDL2/SDL.h>

static SDL_Window *_window;

int Screen::screen_init(uint16_t width, uint16_t height) {
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		std::cout << "SDL failed with error: " << SDL_GetError() << std::endl;
		return 0;
	}

	_window = SDL_CreateWindow("Candle", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, 0);

	if (!_window) {
		std::cout << "Failed to initialize window with error: " << SDL_GetError() << std::endl;
		return 0;
	}
	
	return 1;
}

void Screen::screen_exit() {
	SDL_DestroyWindow(_window);

	SDL_Quit();
}

bool Screen::screen_get_close() {
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			return 1;
		default: break;
		}
	}

	return 0;
}
