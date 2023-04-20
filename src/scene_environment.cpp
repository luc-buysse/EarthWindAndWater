#include "scene_environment.h"

scene_environment::scene_environment()
{
	background_color = { 1,1,1 };
	camera.look_at({ 3,1,2 }, 0,0.1);
	light = { 1,1,1 };

	projection = camera_projection::perspective(50.0f * Pi / 180, 1.0f, 0.1f, 500.0f);
}

void opengl_uniform(GLuint shader, scene_environment const& environment)
{
	opengl_uniform(shader, "projection", environment.projection.matrix());
	opengl_uniform(shader, "view", environment.camera.matrix_view());
	opengl_uniform(shader, "light", environment.light, false);
}