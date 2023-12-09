#include "grid.h"

#include <cstdint>
#include <cmath>
#include <cstdlib>
#include <numbers>

#include "grid_node.h"
#include "vec.h"

#define JITTERS 10

using namespace Simulation;
Grid::Grid(std::uint16_t width, std::uint16_t height) : width(width), height(height) {
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
				const auto u = y == height-1 ? nullptr : get_grid_node(x,y+1,z);
				const auto d = y == 0 ? nullptr : get_grid_node(x,y-1,z);
				const auto l = x == 0 ? nullptr : get_grid_node(x-1,y,z);
				const auto r = x == width-1 ? nullptr : get_grid_node(x+1,y,z);
				const auto f = z == width-1 ? nullptr : get_grid_node(x,y,z+1);
				const auto b = z == 0 ? nullptr : get_grid_node(x,y,z-1);
				_grid[x + z * width + y * width * width]->set_udlrfb(u, d, l, r, f, b);
			}
		}
	}
}

static constexpr Vec3 _calc_direction(const double u1, const double u2) {
	// Based on https://www.rorydriscoll.com/2009/01/07/better-sampling/
	
	double r = std::sqrt(1.0 - u1 * u1);
	double phi = 2.0 * std::numbers::pi * u2;

	return { std::cos(phi) * r, std::sin(phi) * r, u1 };
}

template<int n>
struct JitterGrid {
	constexpr JitterGrid() : array() {
		for (auto y = 0; y < n; y++) {
			for (auto x = 0; x < n; x++) {
				array[x + y * n] = { (x + 0.5) / n, (y + 0.5) / n };
			}
		}
	}

	Vec2 array[n * n];
};

void Grid::_trace_flame_ray(const Vec3 pos, const Vec3 dir) {
	static constexpr double STEP_SIZE = 1;
	static constexpr double RAY_ENERGY = 80 / (2 * std::numbers::pi * JITTERS * JITTERS);

	auto step = dir * STEP_SIZE;

	auto curr_pos = pos;
	auto curr_energy = RAY_ENERGY;
	while (1) {
		if (is_outside_grid((uint16_t)curr_pos.x, (uint16_t)curr_pos.y, (uint16_t)curr_pos.z)) break;

		auto node = get_grid_node((uint16_t)curr_pos.x, (uint16_t)curr_pos.y, (uint16_t)curr_pos.z);
		if (!node->empty()) {
			curr_energy = node->add_energy(curr_energy, STEP_SIZE);
		}

		// TODO: Add russian rouletting

		curr_pos += step;
	}
}

void Grid::_update_flame(const Vec3 pos) {
	static constexpr JitterGrid grid = JitterGrid<JITTERS>();

	for (auto y = 0; y < JITTERS; y++) {
		for (auto x = 0; x < JITTERS; x++) {
			Vec2 jit = grid.array[x + y * JITTERS];
			jit += { (double) std::rand() / RAND_MAX / JITTERS, (double) std::rand() / RAND_MAX / JITTERS };
			Vec3 dir = _calc_direction(jit.x, jit.y);

			_trace_flame_ray(pos, dir);
		}
	}
}

Vec2 Grid::calc_grad(const std::uint16_t x, const std::uint16_t y, const std::uint16_t z) {
	std::int16_t xn = 0, xp = 0, zn = 0, zp = 0; // Where we hit the wax
	std::int8_t xnh = 0, xph = 0, znh = 0, zph = 0; // If it goes up or down in that direction
	
	for (auto d = 1; xn == 0 || xp == 0 || zn == 0 || zp == 0; d++) {
		auto x_min = x - d;
		auto x_max = x + d;
		auto y_min = y - d;
		auto y_max = y + d;

		if (xn == 0 && x_min < 0) xn = -d;
		if (xp == 0 && x_max >= width) xp = d;
		if (zn == 0 && y_min < 0) zn = -d;
		if (zp == 0 && y_max >= width) zp = d;

		if (xn == 0 && !is_free(x_min, y, z)) {
			xn = -d;
			xnh = 1;
		}
		if (xp == 0 && !is_free(x_max, y, z)) {
			xp = d;
			xph = 1;
		}
		if (zn == 0 && !is_free(x, y_min, z)) {
			zn = -d;
			znh = 1;
		}
		if (zp == 0 && !is_free(x, y_max, z)) {
			zp = d;
			zph = 1;
		}

		if (xn == 0 && is_free(x_min, y, z - 1)) {
			xn = -d;
			xnh = -1;
		}
		if (xp == 0 && is_free(x_max, y, z - 1)) {
			xp = d;
			xph = -1;
		}
		if (zn == 0 && is_free(x, y_min, z - 1)) {
			zn = -d;
			znh = -1;
		}
		if (zp == 0 && is_free(x, y_max, z - 1)) {
			zp = d;
			zph = -1;
		}
	}

	return {(float) (xp - xn) / (xph - xnh), (float) (zp - zn) / (zph - znh)};
}
