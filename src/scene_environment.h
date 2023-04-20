#pragma once

#include "cgp/interaction/camera/camera_projection/camera_projection.hpp"
#include "camera.h"
#include "cgp/cgp.hpp"

using namespace cgp;

struct scene_environment
{
	vec3 background_color;

	camera camera;

	camera_projection projection;

	vec3 light;

	scene_environment();
};

void opengl_uniform(GLuint shader, scene_environment const& environment);