#include "Base.h"
#include "TPG/Utils/Utils.h"

void Base::rotateYto(vec3 ny) {
	ny = cgp::normalize(ny);
	vec3 n = cross(ny, y);

	if (norm(n) > 0.01) {
		n = cgp::normalize(n);

		vec3 perp_y = cross(n, y);
		vec3 perp_ny = cross(n, ny);

		mat3 y_base{ y, n, perp_y };
		y_base = transpose(y_base);

		mat3 ny_base{ ny, n, perp_ny };
		ny_base = transpose(ny_base);

		mat3 plan_rot = ny_base * inverse(y_base);

		x = plan_rot * x;
		y = ny;
		z = plan_rot * z;
	}

	normalize();
}

Base& Base::normalize() {
	x = cgp::normalize(x);
	y = cgp::normalize(y);
	z = cgp::normalize(z);

	return *this;
}

Base Base::operator+(Base& b) {
	Base nb;
	nb.x = x + b.x;
	nb.y = y + b.y;
	nb.z = z + b.z;

	return nb;
}

mat3 Base::matrix() {
	return transpose(mat3{ x, y, z });
}

Base::Base(vec3 _y) {
	y = cgp::normalize(_y);

	while (norm(x = cross(y, randDir())) < 0.1);

	x = cgp::normalize(x);
	z = cgp::normalize(cross(x, y));
}