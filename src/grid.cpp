#include "grid.h"

#include <cstdint>

#include "grid_node.h"
#include "vec.h"

using namespace Simulation;
Grid::Grid(std::uint16_t width, std::uint16_t height) : _width(width), _height(height) {
	_grid = new GridNode*[width * width * height];
	for (auto z = 0; z < width; z++) {
		for (auto y = 0; y < height; y++) {
			for (auto x = 0; x < width; x++) {
				_grid[x + z * width + y * width * width] = new GridNode(this, x, y, z);
			}
		}
	}

	for (auto z = 0; z < width; z++) {
		for (auto y = 0; y < height; y++) {
			for (auto x = 0; x < width; x++) {
				_grid[x + z * width + y * width * width]->set_udlrfb(get_grid_node(x,y+1,z), get_grid_node(x,y-1,z), get_grid_node(x-1,y,z), get_grid_node(x+1,y,z), get_grid_node(x,y,z+1), get_grid_node(x,y,z-1));
			}
		}
	}
}

Vec2 Grid::calc_grad(const std::uint16_t x, const std::uint16_t y, const std::uint16_t z) {
	std::int16_t xn = 0, xp = 0, yn = 0, yp = 0; // Where we hit the wax
	std::int8_t xnh = 0, xph = 0, ynh = 0, yph = 0; // If it goes up or down in that direction
	
	for (auto d = 1; xn == 0 || xp == 0 || yn == 0 || yp == 0; d++) {
		auto x_min = x - d;
		auto x_max = x + d;
		auto y_min = y - d;
		auto y_max = y + d;

		if (xn == 0 && x_min < 0) xn = -d;
		if (xp == 0 && x_max >= _width) xp = d;
		if (yn == 0 && y_min < 0) yn = -d;
		if (yp == 0 && y_max >= _width) yp = d;

		if (xn == 0 && !is_free(x_min, y, z)) {
			xn = -d;
			xnh = 1;
		}
		if (xp == 0 && !is_free(x_max, y, z)) {
			xp = d;
			xph = 1;
		}
		if (yn == 0 && !is_free(x, y_min, z)) {
			yn = -d;
			ynh = 1;
		}
		if (yp == 0 && !is_free(x, y_max, z)) {
			yp = d;
			yph = 1;
		}

		if (xn == 0 && is_free(x_min, y, z - 1)) {
			xn = -d;
			xnh = -1;
		}
		if (xp == 0 && is_free(x_max, y, z - 1)) {
			xp = d;
			xph = -1;
		}
		if (yn == 0 && is_free(x, y_min, z - 1)) {
			yn = -d;
			ynh = -1;
		}
		if (yp == 0 && is_free(x, y_max, z - 1)) {
			yp = d;
			yph = -1;
		}
	}

	return {(float) (xp - xn) / (xph - xnh), (float) (yp - yn) / (yph - ynh)};
}
