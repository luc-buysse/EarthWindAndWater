#include "cgp/cgp.hpp" 
#include <iostream> 
#include "custom_drawable.h"
#include "TPG/TPG.h"
#include "scene_environment.h"

using namespace std;
using namespace cgp;

class Application {
public:

	void Initialize();
	void Run();

	// Callbacks

	static void global_window_size_callback(GLFWwindow*, int width, int height);

	static void global_mouse_move_callback(GLFWwindow*, double xpos, double ypos);

	static void global_mouse_click_callback(GLFWwindow*, int button, int action, int /*mods*/);

	static void global_keyboard_callback(GLFWwindow*, int key, int, int action, int /*mods*/);

	void window_size_callback(GLFWwindow*, int width, int height);

	void mouse_move_callback(GLFWwindow*, double xpos, double ypos);

	void mouse_click_callback(GLFWwindow*, int button, int action, int /*mods*/);

	void keyboard_callback(GLFWwindow*, int key, int, int action, int /*mods*/);

	void camera_behavior(camera& cam);

private:
	void display_scene();
	void display_gui();

	void load_cube_map();
	void load_mirror();

	// A helper tool to factorize common default functions (Window initialization, code that starts frame, etc)
	helper_common_scene helper_common;

	double window_width;
	double window_height;

	bool gui_display_frame = true;
	mesh_drawable global_frame;  // The standard global frame

	custom_drawable ground;
	Tree tree;
	custom_drawable floor;
	custom_drawable cube_map;

	scene_environment environment; // Standard environment controler

	GLFWwindow* window;

	GLFWwindow* standard_window_initialization(int width = 0, int height = 0);

	GLuint frameBuffer;
	GLuint floorTexture;
	GLuint floorDepthStencilBuffer;
	GLuint floorDisplacementTexture;

	GLuint basic_shader, tesselation_shader, blur_shader;

	double last_time;
};