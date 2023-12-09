#pragma once

class Vec2 {
	private:
	public:
		double x, y;

		inline Vec2 operator*(const int v) const {
			return {x * v, y * v};
		}

		inline Vec2 operator*(const double v) const {
			return {x * v, y * v};
		}

		inline void operator+=(const Vec2 v) {
			x += v.x;
			y += v.y;
		}		
};

class Vec3 {
	private:
	public:
		double x, y, z;

		inline Vec3 operator*(const double c) const {
			return {x * c, y * c, z * c};
		}
		
		inline Vec3 operator+(const Vec2 v) const {
			return {x + v.x, y + v.y, z};
		}
		
		inline void operator+=(const Vec3 v) {
			x += v.x;
			y += v.y;
			z += v.z;
		}
};
