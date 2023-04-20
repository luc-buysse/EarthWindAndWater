#pragma once

#include "cgp/cgp.hpp"

using namespace cgp;

class Spline
{
public:
	vec3 a, b, c, d;

	vec3 operator() (double x);
	vec3 derivative(double x);
	
	void cardinal(vec3 p0, vec3 p1, vec3 p2, vec3 p3);

	Spline(vec3 p0, vec3 p1, vec3 p2, vec3 p3) { cardinal(p0,p1,p2,p3); }
};

