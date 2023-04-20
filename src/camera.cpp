#include "camera.h"

camera::camera()
	:orientation_camera(), eye_position({ 0.0f, 0.0f, 0.1f }), phi(0), theta(0)
{}

void camera::manipulator_translate_in_horizontal_plane(vec3 const& tr) {
	vec3 translation(0,0,0);

	translation += tr.x * vec3(cos(phi), sin(phi), 0);
	translation += tr.y * vec3(sin(phi), -cos(phi), 0);
	translation += tr.z * vec3(0, 0, 1);

	look_at(eye_position + translation, phi, theta);
}

void camera::manipulator_rotate(double const& roll, double const& pitch, double const& yaw) {
	phi += yaw;
	theta += pitch;
	
	if (cgp::abs(theta - Pi) < 0.01)
		theta = Pi - 0.01;
	if (cgp::abs(theta + Pi) < 0.01)
		theta = -Pi + 0.01;

	look_at(eye_position, phi, theta);
}

void camera::look_at(vec3 const& eye, double _phi, double _theta)
{
	phi = _phi;
	theta = _theta;
	eye_position = eye;

	vec3 dir = vec3(cos(phi) * sin(theta), sin(phi) * sin(theta), cos(theta));

	frame F = camera_frame_look_at(eye_position, eye_position + dir, vec3(0,0,1));
	orientation_camera = F.orientation;
}