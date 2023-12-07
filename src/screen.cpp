#include "screen.h"


#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <iostream>
#include <SDL2/SDL.h>
#include <cstdint>

static SDL_Window *_window;
static SDL_Renderer *_renderer;
static SDL_Texture *_tex;

static uint32_t *_pixels;
static int pitch;

bool Screen::screen_init(const uint16_t width, const uint16_t height, const uint16_t scale) {
	int err = 0;
	
	if ((err = SDL_Init(SDL_INIT_VIDEO))) {
		std::cout << "SDL failed with error: " << err << std::endl;
		return false;
	}

	_window = SDL_CreateWindow("Candle", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, 0);

	if (!_window) {
		std::cout << "Failed to initialize window with error: " << SDL_GetError() << std::endl;
		return 0;
	}

	_renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	if (!_renderer) {
		std::cout << "Failed to create renderer with error: " << SDL_GetError() << std::endl;

		SDL_DestroyWindow(_window);
		SDL_Quit();

		return false;
	}

	_tex = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, width / scale, height / scale);

	if (!_tex) {
		std::cout << "Failed to create texture with error: " << SDL_GetError() << std::endl;

		SDL_DestroyRenderer(_renderer);
		SDL_DestroyWindow(_window);
		SDL_Quit();

		return false;
	}

	pitch = width / scale * sizeof(uint32_t);

	return true;
}

void Screen::update(const Simulation::Grid *_grid) {
	if (SDL_LockTexture(_tex, nullptr, (void **) &_pixels, &pitch)) {
		std::cout << "Failed to lock texture with error: " << SDL_GetError() << std::endl;

		return;
	}
	
	for (auto y = 0; y < _grid->height; y++) {
		for (auto x = 0; x < _grid->width; x++) {
			auto node = _grid->get_grid_node(x, y, _grid->width / 2);
			_pixels[x + y * _grid->width] = node->get_color();
		}
	}

	SDL_UnlockTexture(_tex);
	SDL_RenderCopy(_renderer, _tex, nullptr, nullptr);
	SDL_RenderPresent(_renderer);
}

void Screen::screen_exit() {
	SDL_DestroyTexture(_tex);
	SDL_DestroyRenderer(_renderer);
	SDL_DestroyWindow(_window);

	SDL_Quit();
}

bool Screen::screen_get_close() {
	SDL_Event event;
	
	while (SDL_PollEvent(&event)) {
		if (event.type != SDL_WINDOWEVENT) continue;

		switch (event.window.event) {
		case SDL_WINDOWEVENT_CLOSE:
			return true;

		default: break;
		}
	}

	return false;
}
