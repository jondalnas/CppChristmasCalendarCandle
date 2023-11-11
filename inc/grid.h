#pragma once

#include <cstdint>

#include "grid_node.h"

namespace Simulation {
	class Grid {
		private:
			GridNode **_grid;
			std::uint16_t _width, _height;

		public:
			Grid(std::uint16_t width, std::uint16_t height) : _width(width), _height(height) {
				_grid = new GridNode*[width * height];
				for (auto y = 0; y < height; y++) {
					for (auto x = 0; x < width; x++) {
						_grid[x + y * width] = new GridNode(x, y);
					}
				}
			}

			~Grid() {
				delete[] _grid;
			}

			inline GridNode *get_grid_node(const std::uint16_t x, const std::uint16_t y) const {
				return _grid[x + y * _width];
			}

	};
}
