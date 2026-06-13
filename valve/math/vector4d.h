#pragma once

class vector4d
{
public:
	float x;
	float y;
	float z;
	float w;

	vector4d(float x, float y, float z, float w) {
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	vector4d() {
		this->x = 0;
		this->y = 0;
		this->z = 0;
		this->w = 0;
	}

	bool is_zero() {
		return x == 0 && y == 0 && z == 0 && w == 0;
	}

	float distance(vector4d v) {
		return sqrt(pow(x - v.x, 2) + pow(y - v.y, 2) + pow(z - v.z, 2) + pow(w - v.w, 2));
	}

	vector4d operator+(vector4d v) {
		return vector4d(x + v.x, y + v.y, z + v.z, w + v.w);
	}

	vector4d operator-(vector4d v) {
		return vector4d(x - v.x, y - v.y, z - v.z, w - v.w);
	}

	vector4d operator*(float f) {
		return vector4d(x * f, y * f, z * f, w * f);
	}

	vector4d operator/(float f) {
		return vector4d(x / f, y / f, z / f, w / f);
	}

	bool operator==(vector4d v) {
		return x == v.x && y == v.y && z == v.z && w == v.w;
	}

	bool operator!=(vector4d v) {
		return x != v.x || y != v.y || z != v.z || w != v.w;
	}

	vector4d operator-() {
		return vector4d(-x, -y, -z, -w);
	}

	vector4d operator+=(vector4d v) {
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;
		return *this;
	}

	vector4d operator-=(vector4d v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
		w -= v.w;
		return *this;
	}

	vector4d operator*=(float f) {
		x *= f;
		y *= f;
		z *= f;
		w *= f;
		return *this;
	}

	vector4d operator/=(float f) {
		x /= f;
		y /= f;
		z /= f;
		w /= f;
		return *this;
	}

};