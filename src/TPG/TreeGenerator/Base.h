#pragma once

#include <cgp/cgp.hpp>

using namespace cgp;

class Base
{
public:
	vec3 x, y, z;

	Base& normalize();
	Base operator+(Base& b);
	void rotateYto(vec3 ny);

	mat3 matrix();

	Base() {};
	Base(vec3);
};

