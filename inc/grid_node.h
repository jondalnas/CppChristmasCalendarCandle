#pragma once

#include <cstdint>

#include "vec.h"

#define ROOM_TEMP 294.15

namespace Simulation {
	class Grid;

	class GridNode {
		private:
			Grid *const _grid;

			double _time;
			enum {
				SOLID, LIQUID, GAS
			} _state;

			std::uint16_t _color;

			Vec2 _dir;

			void _remove();

			bool _move();

			double _calc_heat_change() const;

		public:
			Vec3 pos;
			GridNode *u, *d, *l, *r, *f, *b;
			double temp = ROOM_TEMP;
			
			GridNode(Grid *const grid, const std::uint16_t xx, const std::uint16_t yy, const std::uint16_t zz) : _grid(grid), pos({xx + 0.5, yy + 0.5, zz + 0.5}) {}

			void update();

			inline void set_udlrfb(GridNode *u, GridNode *d, GridNode *l, GridNode *r, GridNode *f, GridNode *b) {
				this->u = u;
				this->d = d;
				this->l = l;
				this->r = r;
				this->f = f;
				this->b = b;
			}

			inline void swap_udlrfb(GridNode *const source) {
				GridNode *u = this->u, *d = this->d, *l = this->l, *r = this->r, *f = this->f, *b = this->b;
				this->u = source->u;
				this->d = source->d;
				this->l = source->l;
				this->r = source->r;
				this->f = source->f;
				this->b = source->b;

				source->u = u;
				source->d = d;
				source->l = l;
				source->r = r;
				source->f = f;
				source->b = b;
			}

			inline bool empty() const {
				return _state == GAS;
			}
	};
}
