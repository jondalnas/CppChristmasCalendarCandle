#include <ctime>
#include <unistd.h>

#include "grid.h"

#define TARGET_FPS 60
#define SLEEP_US 1000000.0 / TARGET_FPS

extern const unsigned int SCREEN_SCALE = 4;

double delta_time = 0.0;

extern double get_delta_time() {
	return delta_time;
}

int main() {
	Simulation::Grid grid(640 / SCREEN_SCALE, 480 / SCREEN_SCALE);

	time_t time = clock();

	while(1) {
		grid.update();

		usleep(SLEEP_US - (clock() - time) * 1000000.0 / CLOCKS_PER_SEC);
		time_t now = clock();
		delta_time = (double) (now - time) / CLOCKS_PER_SEC;
		time = now;
	}

	return 0;
}
