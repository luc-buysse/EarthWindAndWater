#include "TPG/Utils/Utils.h"

bool operator ==(vec3 a, vec3 b) {
	return a[0] == b[0] && a[1] == b[1] && a[2] == b[2];
}

vec3 randDir() {
	double theta = ((double)rand() / RAND_MAX) * Pi;
	double phi = ((double)rand() / RAND_MAX) * Pi * 2;

	return vec3(cos(phi) * sin(theta), cos(theta), sin(phi) * sin(theta));
}