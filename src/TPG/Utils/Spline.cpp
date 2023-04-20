#include "Spline.h"

vec3 Spline::operator() (double x) {
	return a * (x * x * x) + b * (x * x) + c * x + d;
}

void Spline::cardinal(vec3 p0, vec3 p1, vec3 p2, vec3 p3) {
	a = -p0 / 2 + (3. / 2) * p1 - (3. / 2) * p2 + p3 / 2;
	b = p0 - (5. / 2) * p1 + 2 * p2 - p3 / 2;
	c = (p2 - p0) / 2;
	d = p1;
}

vec3 Spline::derivative(double x) {
	return 3 * a * (x * x) + 2 * b * x + c;
}