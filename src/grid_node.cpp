#include "grid_node.h"

#include "grid.h"

#include <cstdint>
#include <cmath>

#define DIFFUSIVITY 1.34/(900*2.14)
#define PIXEL_LENGTH (1.0/(1500.0 / SCREEN_SCALE)) //m/px
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
#define EXTINCTION_COEFFICIENT 1.5e-5

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
	double l_temp = l ? l->temp : ROOM_TEMP;
	double r_temp = r ? r->temp : ROOM_TEMP;
	double u_temp = u ? u->temp : ROOM_TEMP;
	double d_temp = d ? d->temp : ROOM_TEMP;
	double f_temp = f ? f->temp : ROOM_TEMP;
	double b_temp = b ? b->temp : ROOM_TEMP;

	// Via https://en.wikipedia.org/wiki/Discrete_Laplace_operator
	// Could also use 27-point stencil, but 7-point might be good enough
	
	double laplace = l_temp + r_temp + u_temp + d_temp + f_temp + b_temp - 6 * temp;
	laplace *= 1.0 / PIXEL_LENGTH;

	return DIFFUSIVITY * laplace * get_delta_time();
}

double GridNode::add_energy(double energy, double length) {
	auto T_r = std::exp(-EXTINCTION_COEFFICIENT * length * PIXEL_LENGTH);
	temp += energy / SPECIFIC_HEAT_CAPACITY_MASS * (1.0 - T_r);

	return energy * T_r;
}
