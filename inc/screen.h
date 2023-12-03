#pragma once

#include <cstdint>

namespace Screen {
	int screen_init(uint16_t width, uint16_t height);
	
	void screen_exit();

	bool screen_get_close();
}
