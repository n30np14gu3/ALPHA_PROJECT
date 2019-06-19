#pragma once

#include "vector3d.hpp"
class vec2_t {
public:
	float x, y;

	vec2_t() {
		x = 0; y = 0;
	};
	vec2_t(float X, float Y) {
		x = X; y = Y;
	};
	vec2_t(vec3_t vec) {
		x = vec.x; y = vec.y;
	}

	inline vec2_t operator*(const float n) const {
		return vec2_t(x*n, y*n);
	}
	inline vec2_t operator+(const vec2_t& v) const {
		return vec2_t(x + v.x, y + v.y);
	}
	inline vec2_t operator-(const vec2_t& v) const {
		return vec2_t(x - v.x, y - v.y);
	}
	inline void operator+=(const vec2_t& v) {
		x += v.x;
		y += v.y;
	}
	inline void operator-=(const vec2_t& v) {
		x -= v.x;
		y -= v.y;
	}

	bool operator==(const vec2_t& v) const {
		return (v.x == x && v.y == y);
	}
	bool operator!=(const vec2_t& v) const {
		return (v.x != x || v.y != y);
	}

	inline float length() {
		return sqrt((x * x) + (y * y));
	}

	inline float distance_to(const vec2_t &other) {
		vec2_t delta;
		delta.x = x - other.x;
		delta.y = y - other.y;

		return delta.length();
	}

	inline void normalize(void) {
		auto vec_normalize = [&](vec2_t& v) {
			auto l = v.length();

			if (l != 0.0f) {
				v.x /= l;
				v.y /= l;

			}
			else {
				v.x = v.y = 0.0f;
			}

			return l;
		};

		vec_normalize(*this);
	}
};