#pragma once

#include "grid.h"
#include <cstdint>

namespace Screen {
	bool screen_init(const uint16_t width, const uint16_t height, const uint16_t scale);

	void update(const Simulation::Grid *_grid);
	
	void screen_exit();

	bool screen_get_close();
}
