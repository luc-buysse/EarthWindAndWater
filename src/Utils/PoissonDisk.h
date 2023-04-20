#pragma once

#include <vector>
#include <map>
#include <cgp/cgp.hpp>
#include <iostream>
#include <math.h>

using namespace std;
using namespace cgp;

float distance(const vec3& v1, const vec3& v2);

class PoissonDisk
{
private:
	double minDist;
	cgp::vec3 dimensions;
	vector<cgp::vec3> points;

	double bsize;
	int bx,by,bz;
	int * grid;
	vector<int> remain;

	int k = 15;

	int get(vec3& p);
	void set(vec3& p, int i);

	int get(int x, int y, int z);
	void set(int x, int y, int z, int i);

	int index(int x, int y,int z);

public:
	PoissonDisk(double _d, vec3 dim, vector<vec3> init) : minDist(_d), points(init), dimensions(dim) {
		bsize = minDist / sqrt(3);

		bx = dimensions.x / bsize;
		by = dimensions.y / bsize;
		bz = dimensions.z / bsize;

		grid = new int[bx * by * bz];

		fill(grid, grid + bx * by * bz, -1);

		for (int i = 0; i < points.size(); i++) {
			cgp::vec3& p = points[i];

			remain.push_back(i);

			set(p, i);
		}
	};

	~PoissonDisk() {
		delete[] grid;
	}

	vector<cgp::vec3>& generate();
};

