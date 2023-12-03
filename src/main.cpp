#include <ctime>
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

	Screen::screen_init(WIDTH, HEIGHT);

	time_t time = clock();

	while(!Screen::screen_get_close()) {
		grid.update();

		usleep(SLEEP_US - (clock() - time) * 1000000.0 / CLOCKS_PER_SEC);
		time_t now = clock();
		delta_time = (double) (now - time) / CLOCKS_PER_SEC;
		time = now;
	}

	Screen::screen_exit();

	return 0;
}
