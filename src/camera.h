#pragma once

#include "cgp/base/base.hpp"
#include "cgp/interaction/camera/common_functions/common_functions.hpp"
#include "cgp/interaction/camera/camera_base/camera_base.hpp"

#include <iostream>

using namespace std;

using namespace cgp;

struct camera : camera_base
{
	camera();

	vec3 position() const {
		return eye_position;
	}

	rotation_transform orientation() const {
		return orientation_camera;
	}

	rotation_transform orientation_camera;
	vec3 eye_position;

	double phi;
	double theta;

	void manipulator_translate_in_horizontal_plane(vec3 const& tr);
	void manipulator_rotate(double const& roll, double const& pitch, double const& yaw);

	void look_at(vec3 const& eye, double, double);
};

