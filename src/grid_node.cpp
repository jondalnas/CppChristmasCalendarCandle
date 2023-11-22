#include "grid_node.h"

#include "grid.h"
#include <cstdint>

#define DIFFUSIVITY 0.15e-6
#define PIXEL_LENGTH 1/(1500.0 / SCREEN_SCALE); //m/px
#define PIXEL_VOLUME PIXEL_LENGTH * PIXEL_LENGTH * PIXEL_LENGTH //m^3/px^3
#define SPECIFIC_HEAT_CAPACITY 2.5 //kJ/(kg*K)
#define MELTING_POINT 55 //C
#define BOILING_POINT 370 //C
#define LATENT_HEAT 176 //kJ/kg
#define ACTUAL_MELTING_POINT MELTING_POINT + LATENT_HEAT / SPECIFIC_HEAT_CAPACITY //C
#define DENSITY 900 //kg/m^3
#define PIXEL_MASS PIXEL_VOLUME * DENSITY //kg/px^3 / kg
#define SPECIFIC_HEAT_CAPACITY_MASS SPECIFIC_HEAT_CAPACITY * PIXEL_MASS //kJ/K
#define LIQUID_VELOCITY 1.0 * SCREEN_SCALE / 1.0 //px/s
#define LIQUID_DOWN_VELOCITY 2.0 * SCREEN_SCALE / 1.0 //px/s

extern const int SCREEN_SCALE;
extern double get_delta_time();

using namespace Simulation;

void GridNode::_remove() {
	temp = ROOM_TEMP;
	_dir = {0, 0};
	_time = 0;
	_state = GAS;
	_color = 0;
}

bool GridNode::_move() {
	if (pos.z - (std::uint16_t) pos.z > 0.001 || _grid->is_free((std::uint16_t) pos.x, (std::uint16_t) pos.y, (std::uint16_t) pos.z - 1)) {
		auto new_z = pos.z - LIQUID_DOWN_VELOCITY * get_delta_time();

		if (!_grid->is_free((std::uint16_t) pos.x, (std::uint16_t) pos.y, (std::uint16_t) new_z)) {
			new_z = (std::uint16_t) new_z + 1;
		}

		pos.z = new_z;

		if ((std::uint16_t) pos.z != (std::uint16_t) new_z) {
			_grid->swap_nodes(this, pos.x, pos.y, new_z);
		}

		return true;
	}

	Vec3 new_pos = pos + _dir * (LIQUID_VELOCITY * get_delta_time());

	if (_grid->is_free((std::uint16_t) new_pos.x, (std::uint16_t) new_pos.y, (std::uint16_t) pos.z)) {
		if ((std::uint16_t) pos.x != (std::uint16_t) new_pos.x || (std::uint16_t) pos.y != (std::uint16_t) new_pos.y) {
			_grid->swap_nodes(this, new_pos.x, new_pos.y, new_pos.z);
		}

		pos = new_pos;

		return true;
	}

	return false;
}

void GridNode::update() {
	temp += _calc_heat_change();

	switch (_state) {
	case SOLID:
		if (temp > MELTING_POINT) {
			_state = LIQUID;
		}
		break;

	case LIQUID:
		if (temp < MELTING_POINT) {
			_state = SOLID;
		} else if (temp > BOILING_POINT) {
			_remove();
		} else {
			if (_dir.x == 0 && _dir.y == 0) {
				_dir = _grid->calc_grad(pos.x, pos.y, pos.z);
			}

			_move();
		}

		break;
		
	case GAS:
		break;
	}
}

double GridNode::_calc_heat_change() const {
	double l_diff = 0.0;
	double r_diff = 0.0;
	double u_diff = 0.0;
	double d_diff = 0.0;
	double f_diff = 0.0;
	double b_diff = 0.0;

	l_diff = (l->temp - temp) / PIXEL_LENGTH;
	r_diff = (r->temp - temp) / PIXEL_LENGTH;

	u_diff = (u->temp - temp) / PIXEL_LENGTH;
	d_diff = (d->temp - temp) / PIXEL_LENGTH;

	f_diff = (f->temp - temp) / PIXEL_LENGTH;
	b_diff = (b->temp - temp) / PIXEL_LENGTH;

	return DIFFUSIVITY * (l_diff * l_diff + r_diff * r_diff + u_diff * u_diff + d_diff * d_diff + f_diff * f_diff + b_diff * b_diff) * get_delta_time();
}
