#pragma once

#include <cstdint>

namespace Simulation {
	class GridNode {
		private:
			const std::uint16_t _x, _y;
			double _temp, _time;
			enum {
				SOLID, LIQUID, GAS
			} _state;

			const GridNode *_u, *_d, *_l, *_r, *_f, *_b;

		public:
			GridNode(const std::uint16_t x, const std::uint16_t y) : _x(x), _y(y) {}

			void update();

			inline void set_udlrfb(const GridNode *u, const GridNode *d, const GridNode *l, const GridNode *r, const GridNode *f, const GridNode *b) {
				this->_u = u;
				this->_d = d;
				this->_l = l;
				this->_r = r;
				this->_f = f;
				this->_b = b;
			}
	};
}
