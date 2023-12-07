#include <ctime>
#include <iostream>
#include <unistd.h>

#include "grid.h"

#include "screen.h"

#define TARGET_FPS 60
#define SLEEP_US 1000000.0 / TARGET_FPS

#define WIDTH 640
#define HEIGHT 480

extern const unsigned int SCREEN_SCALE = 4;

double delta_time = 0.0;

extern double get_delta_time() {
	return delta_time;
}

int main() {
	Simulation::Grid grid(WIDTH / SCREEN_SCALE, HEIGHT / SCREEN_SCALE);

	for (auto y = HEIGHT / SCREEN_SCALE / 3; y < HEIGHT / SCREEN_SCALE; y++) {
		for (auto x = WIDTH / SCREEN_SCALE / 3; x < WIDTH / SCREEN_SCALE * 2 / 3; x++) {
			for (auto z = WIDTH / SCREEN_SCALE / 3; z < WIDTH / SCREEN_SCALE * 2 / 3; z++) {
				grid.get_grid_node(x, y, z)->set_color(0xffffff);
			}
		}
	}

	Screen::screen_init(WIDTH, HEIGHT, SCREEN_SCALE);

	time_t time = clock();

	while(!Screen::screen_get_close()) {
		grid.update();
		Screen::update(&grid);

		auto sleep_time = SLEEP_US - (clock() - time) * 1000000.0 / CLOCKS_PER_SEC;
		usleep(sleep_time < 0 ? 0 : sleep_time);
		time_t now = clock();
		delta_time = (double) (now - time) / CLOCKS_PER_SEC;

		std::cout << 1.0/delta_time << "FPS" << std::endl;

		time = now;
	}

	Screen::screen_exit();

	return 0;
}
