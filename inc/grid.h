#pragma once

#include <cstdint>

#include "grid_node.h"
#include "vec.h"

namespace Simulation {
	class Grid {
		private:
			GridNode **_grid;
			std::uint16_t _width, _height;

		public:
			Grid(std::uint16_t width, std::uint16_t height);

			~Grid() {
				delete[] _grid;
			}

			inline void update() {
				for (auto z = 0; z < _width; z++) {
					for (auto y = 0; y < _height; y++) {
						for (auto x = 0; x < _width; x++) {
							_grid[x + z * _width + y * _width * _width]->update();
						}
					}
				}
			}

			inline GridNode *get_grid_node(const std::uint16_t x, const std::uint16_t y, const std::uint16_t z) const {
				return _grid[x + z * _width + y * _width * _width];
			}

			inline void set_grid_node(GridNode *node, const std::uint16_t x, const std::uint16_t y, const std::uint16_t z) {
				_grid[x + z * _width + y * _width * _width] = node;	
			}

			inline bool is_free(const std::uint16_t x, const std::uint16_t y, const std::uint16_t z) const {
				return get_grid_node(x, y, z)->empty();
			}

			inline void swap_nodes(GridNode *node, const std::uint16_t x, const std::uint16_t y, const std::uint16_t z) {
				GridNode *tmp = get_grid_node(x, y, z);
				set_grid_node(node, x, y, z);
				set_grid_node(tmp, node->pos.x, node->pos.y, node->pos.z);
				node->swap_udlrfb(tmp);
			}

			Vec2 calc_grad(const std::uint16_t x, const std::uint16_t y, const std::uint16_t z);
	};
}
