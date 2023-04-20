#include "PoissonDisk.h"

#include <cgp/base/basic_types/basic_types.hpp>

using namespace cgp;

float distance(const vec3& v1, const vec3& v2) {
	return ((v1.x - v2.x) * (v1.x - v2.x) + (v1.y - v2.y) * (v1.y - v2.y) + (v1.z - v2.z) * (v1.z - v2.z));
}

int PoissonDisk::get(int x, int y, int z) {
	if (x < 0 || x >= bx || y < 0 || y >= by || z < 0 || z >= bz) {
		return -1;
	}

	return grid[index(x,y,z)];
}

int PoissonDisk::get(vec3& p) {
	int x = floor(p.x / bsize);
	int y = floor(p.y / bsize);
	int z = floor(p.z / bsize);

	return get(x, y, z);
}

int PoissonDisk::index(int x, int y, int z)	 {
	int index = 0;
	index += x * by * bz;
	index += y * bz;
	index += z;

	return index;
}

void PoissonDisk::set(int x, int y, int z, int i)  {
	if (x < 0 || x >= bx || y < 0 || y >= by || z < 0 || z >= bz) {
		return;
	}

	grid[index(x,y,z)] = i;
}

void PoissonDisk::set(vec3& p, int i)  {
	int x = floor(p.x / bsize);
	int y = floor(p.y / bsize);
	int z = floor(p.z / bsize);

	if (x < 0 || x >= bx || y < 0 || y >= by || z < 0 || z >= bz) {
		return;
	}

	grid[index(x, y, z)] = i;
}

vector<vec3>& PoissonDisk::generate() {
	
	
	while (!remain.empty()) {
		const vec3& cn = points[remain.back()];

		for (int a = 0; true; a++) {
			vec3 rpos;

			// Pick pos at random in [r, 2r]
			double theta = ((double)rand() / RAND_MAX) * Pi;
			double phi = ((double)rand() / RAND_MAX) * Pi * 2;
			double rad = ((double)rand() / RAND_MAX) * minDist + minDist;

			rpos = cn + vec3(cos(phi) * sin(theta) * rad, cos(theta) * rad, sin(phi) * sin(theta) * rad);

			int x = floor(rpos.x / bsize);
			int y = floor(rpos.y / bsize);
			int z = floor(rpos.z / bsize);

			bool go = x < bx && x >= 0 && y < by && y >= 0 && z < bz && z >= 0;

			if (go)
				for (int i = -2; i <= 2; i++) {
					for (int j = -2; j <= 2; j++) {
						for (int k = -2; k <= 2; k++) {
							bool c = (x + i < bx && x + i >= 0);
							c = (c && (y + j < by && y + j >= 0));
							c = (c && (z + k < bz && z + k >= 0));

							int gridval = get(x + i, y + j, z + k);

							c = (c && (gridval >= 0));
							c = (c && (distance(points[gridval], rpos) < minDist));

							go = (go && !c);
						}
					}
				}

			if (go)
			{
				points.push_back(vec3(rpos));

				int gridval = get(x, y, z);

				set(x, y, z, points.size() - 1);
				remain.push_back(points.size() - 1);

				for (int i = 0; i < points.size() - 1; i++) {
					double d = distance(points[i], points.back());
				}

				break;
			}

			if (a == k) {
				remain.pop_back();
				break;
			}
		}
	}

	return points;
}