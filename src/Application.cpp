#include "Application.h"

// Callbacks

// This function is called everytime a keyboard touch is pressed/released
void Application::global_keyboard_callback(GLFWwindow* wn, int key, int, int action, int mods)
{
	Application* app = (Application*)glfwGetWindowUserPointer(wn);
	app->keyboard_callback(wn, key, 0, action, mods);
}

// This function is called everytime the window is resized
void Application::global_window_size_callback(GLFWwindow* wn, int width, int height)
{
	Application * app = (Application*)glfwGetWindowUserPointer(wn);
	app->window_size_callback(wn, width, height);
}

// This function is called everytime the mouse is moved
void Application::global_mouse_move_callback(GLFWwindow* wn, double xpos, double ypos)
{
	Application* app = (Application*)glfwGetWindowUserPointer(wn);
	app->mouse_move_callback(wn, xpos, ypos);
}

// This function is called everytime a mouse button is clicked/released
void Application::global_mouse_click_callback(GLFWwindow* wn, int button, int action, int mods)
{
	Application* app = (Application*)glfwGetWindowUserPointer(wn);
	app->mouse_click_callback(wn, button, action, mods);
}

void Application::camera_behavior(camera& cam)
{
	double dt = glfwGetTime() - last_time;
	last_time = glfwGetTime();

	double px, py;
	glfwGetCursorPos(window, &px, &py);

	vec2 const p1 = vec2(px, py);
	vec2 const& p0 = vec2(window_width / 2, window_height / 2);
	
	bool const forward = (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS);
	bool const backward = (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS);
	bool const left = (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS);
	bool const right = (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS);
	bool const up = (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS);
	bool const down = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS);

	if(p1.x >= 0 && p1.x <= window_width && p1.y >= 0 && p1.y <= window_height)
		cam.manipulator_rotate(0, (p1.y - p0.y) * 0.001, -(p1.x - p0.x) * 0.001 );

	cam.manipulator_translate_in_horizontal_plane(vec3(forward - backward, right - left, up - down) * dt * 10);
}


void Application::window_size_callback(GLFWwindow* wn, int width, int height) {
	window_width = width;
	window_height = height;
}

void Application::mouse_move_callback(GLFWwindow* wn, double xpos, double ypos)
{
	
}

void Application::mouse_click_callback(GLFWwindow* wn, int button, int action, int mods)
{
	
}

void Application::keyboard_callback(GLFWwindow* wn, int key, int, int action, int /*mods*/)
{
	
}

// This function is called only once at the beginning of the program
void Application::Initialize()
{
	window = standard_window_initialization();
	glfwSetWindowUserPointer(window, this);


	// Load shaders
	// ***************************************** //

	basic_shader = opengl_load_shader(
		"C:/Users/Luc/OneDrive/Bureau/INF443/code_inf443/projects/Project2_MakeOpenGLWork/shaders/mesh_reference/vert.glsl",
		"C:/Users/Luc/OneDrive/Bureau/INF443/code_inf443/projects/Project2_MakeOpenGLWork/shaders/mesh_reference/frag.glsl");

	blur_shader = opengl_load_shader(
		"C:/Users/Luc/OneDrive/Bureau/INF443/code_inf443/projects/Project2_MakeOpenGLWork/shaders/forward/vert.glsl",
		"C:/Users/Luc/OneDrive/Bureau/INF443/code_inf443/projects/Project2_MakeOpenGLWork/shaders/forward/frag.glsl");

	tesselation_shader = opengl_load_shader_custom(
		"C:/Users/Luc/OneDrive/Bureau/INF443/code_inf443/projects/Project2_MakeOpenGLWork/shaders/tesselation/vert.glsl",
		"C:/Users/Luc/OneDrive/Bureau/INF443/code_inf443/projects/Project2_MakeOpenGLWork/shaders/tesselation/frag.glsl",
		"C:/Users/Luc/OneDrive/Bureau/INF443/code_inf443/projects/Project2_MakeOpenGLWork/shaders/tesselation/tesco.glsl",
		"C:/Users/Luc/OneDrive/Bureau/INF443/code_inf443/projects/Project2_MakeOpenGLWork/shaders/tesselation/tesev.glsl");


	// Initialize the shapes of the scene
	// ***************************************** //
	// Set the behavior of the camera and its initial position
	environment.camera.look_at({ 0,0,10 }, 0, Pi - 0.01);

	// Create a visual frame representing the coordinate system
	global_frame.initialize(mesh_primitive_frame(), "Frame");
	// set a specific shader for the global_frame (to avoids modifying it)
	global_frame.shader = basic_shader;


	// Create a mesh structure (here a cube)
	mesh cube_mesh = mesh_primitive_cube(/*center*/{ 0,0,0 }, /*edge length*/ 1.0f);
	// a mesh is simply a container of vertex data (position,normal,color,uv) and triangle index
	// the mesh data are stored on the CPU memory - they will need to be sent to the GPU memory before being drawn

	// On initialise carré
	mesh square;
	square.position = { {0,0,0}, {0,0,1}, {1,0,1}, {1,0,0} };
	square.connectivity = { {0,1,2}, {0,2,3} };
	square.uv = { {0,0}, {0,1}, {1,1}, {1,0} };
	square.fill_empty_field();

	// Same process for the ground which is a plane
	mesh ground_mesh = mesh_primitive_quadrangle({ -3,-3,-0.51f }, { -3,3,-0.51f }, { 3,3,-0.51f }, { 3,-3,-0.5f });
	ground.initialize(ground_mesh, "Ground");
	ground.texture = opengl_load_texture_image("C:/Users/Luc/OneDrive/Bureau/INF443/code_inf443/projects/Project2_MakeOpenGLWork/assets/checkboard.png");
	ground.shader = basic_shader;
	ground.shader_type = SHADER_BASIC;

	tree.tesselationShader = tesselation_shader;
	tree.regularShader = basic_shader;

	// Use to generate a new tree
	//tree.make(TreeModel(TREE_SEQUOIA));
	//tree.saveAs("sequoia1.tree");

	tree.load("sequoia1.tree");

	load_mirror();

	load_cube_map();
}

void Application::Run() {
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	last_time = glfwGetTime();

	// ************************ //
	//     Animation Loop
	// ************************ //
	std::cout << "Start animation loop ..." << std::endl;
	while (!glfwWindowShouldClose(window) && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS )
	{
		display_scene();

		imgui_create_frame();
		ImGui::Begin("GUI", NULL, ImGuiWindowFlags_AlwaysAutoResize);

		// Display the ImGUI interface (button, sliders, etc)
		display_gui();

		camera_behavior(environment.camera);

		glfwSetCursorPos(window, window_width / 2, window_height / 2);

		// End of ImGui display and handle GLFW events
		helper_common.frame_end(window);
	}

	// Cleanup
	glDeleteFramebuffers(1, &frameBuffer);

	imgui_cleanup();
	glfwDestroyWindow(window);
	glfwTerminate();
}

// This function is constantly called at every frame
void Application::display_scene()
{
	// Display the reverse world in th frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glViewport(0, 0, window_width, window_height);
	glClearColor(environment.background_color.x, environment.background_color.y, environment.background_color.z, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now
	glEnable(GL_DEPTH_TEST);
	environment.projection.update_aspect_ratio(window_width / window_height);

	tree.model_matrix = mat4::diagonal(vec3(1, 1, -1));
	cube_map.model_matrix = mat4::diagonal(vec3(1, 1, -1));
	draw(cube_map, environment);
	tree.render(environment);

	// Display the result on the screen
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
	glViewport(0, 0, window_width, window_height);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	draw(floor, environment);

	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	
	tree.model_matrix = mat4::diagonal(vec3(1, 1, 1));
	cube_map.model_matrix = mat4::diagonal(vec3(1, 1, 1));
	draw(cube_map, environment);
	tree.render(environment);

	// Set the light to the current position of the camera
	environment.light = environment.camera.position();
}


// This function is constantly called at every frame
void Application::display_gui()
{
	ImGui::Checkbox("Frame", &gui_display_frame);
}

// Standard initialization procedure
GLFWwindow* Application::standard_window_initialization(int width_target, int height_target)
{
	// Create the window using GLFW

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();

	if (monitor == NULL) {
		cout << "MONITOR NULL";
	}
	
	GLFWwindow* window = cgp::create_window(0, 0, "Tree Generator"); 

	// Update storage for window size for the scene
	int width = 0, height = 0;
	glfwGetWindowSize(window, &width, &height);
	window_width = width;
	window_height = height;

	// Display debug information on command line
	std::cout << cgp::opengl_info_display() << std::endl;

	cgp::imgui_init(window);

	glfwSetKeyCallback(window, global_keyboard_callback);            // Event called when a keyboard touch is pressed/released
	glfwSetMouseButtonCallback(window, global_mouse_click_callback); // Event called when a button of the mouse is clicked/released
	glfwSetCursorPosCallback(window, global_mouse_move_callback);    // Event called when the mouse is moved
	glfwSetWindowSizeCallback(window, global_window_size_callback);  // Event called when the window is rescaled        

	helper_common.initialize();

	return window;
}

void Application::load_cube_map() {
	double dist = 100;

	mesh cube_mesh;
	cube_mesh.uv = { vec2(0,0.33334),
		vec2(0.25,0.33334),
		vec2(0.25,0),
		vec2(0.50,0),
		vec2(0.50,0.33334),
		vec2(0.75,0.33334),
		vec2(1,0.33334),
		vec2(1,1./2 ),
		vec2(3. / 4,1. / 2),
		vec2(1. / 2,1./2),
		vec2(1. / 2,1),
		vec2(1. / 4,1),
		vec2(1. / 4,1. / 2),
		vec2(0,1./2)
	};

	for (vec2& v : cube_mesh.uv) {
		v.y = 1 - v.y;
	}

	cube_mesh.position = {
		vec3(-dist, -dist,dist),
		vec3(-dist, dist, dist),
		vec3(-dist, -dist,dist),
		vec3(dist, -dist, dist),
		vec3(dist, dist, dist),
		vec3(dist, -dist, dist),
		vec3(-dist, -dist, dist),
		vec3(-dist, -dist, 0),
		vec3(dist, -dist, 0),
		vec3(dist, dist, 0),
		vec3(dist, -dist, 0),
		vec3(-dist, -dist, 0),
		vec3(-dist, dist, 0),
		vec3(-dist, -dist, 0)
	};

	cube_mesh.connectivity = {
		uint3(13,0,1),
		uint3(1,2,3),
		uint3(1,3,4),
		uint3(13,1,12),
		uint3(12,1,4),
		uint3(12,4,9),
		uint3(9,4,5),
		uint3(9,5,8),
		uint3(8,5,6),
		uint3(8,6,7)
	};

	cube_mesh.fill_empty_field();

	cube_map.shader_type = SHADER_BASIC;
	cube_map.initialize(cube_mesh);
	cube_map.shader = basic_shader;

	cube_map.shading.phong.ambient = 1;
	cube_map.shading.phong.diffuse = 0;
	cube_map.shading.phong.specular = 0;
	cube_map.shading.phong.specular_exponent = 1;
	cube_map.shading.alpha = 1.;
	
	cube_map.texture = opengl_load_texture_image("C:/Users/Luc/OneDrive/Bureau/INF443/code_inf443/projects/Project2_MakeOpenGLWork/assets/cubeMap.png");
	cube_map.translate = false;
}

void Application::load_mirror() {
	// Initialize the floor frame buffer;
	glGenFramebuffers(1, &frameBuffer);

	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	glGenTextures(1, &floorTexture); opengl_check;
	glBindTexture(GL_TEXTURE_2D, floorTexture); opengl_check;

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window_width, window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL); opengl_check;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); opengl_check;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); opengl_check;

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, floorTexture, 0); opengl_check;

	glGenRenderbuffers(1, &floorDepthStencilBuffer); opengl_check;
	glBindRenderbuffer(GL_RENDERBUFFER, floorDepthStencilBuffer); opengl_check;
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, window_width, window_height); opengl_check;
	glBindRenderbuffer(GL_RENDERBUFFER, 0); opengl_check;

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, floorDepthStencilBuffer); opengl_check;

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Problème : le frame buffer n'a pas été correctement initialisé." << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0); opengl_check;

	mesh floor_mesh;
	floor_mesh.position = { vec3(-1,-1,0), vec3(-1, 1, 0), vec3(1,1,0), vec3(1,-1,0) };
	floor_mesh.uv = { vec2(0,0), vec2(0,1), vec2(1,1), vec2(1,0) };
	floor_mesh.connectivity = { uint3(0,1,2), uint3(0,2,3) };
	floor_mesh.fill_empty_field();
	floor.shader_type = SHADER_BLUR;

	floor.initialize(floor_mesh, "Floor");
	floor.shader = blur_shader;
	floor.texture = floorTexture;

	// Generate the displacement texture for the floor

	glGenTextures(1, &floorTexture); opengl_check;
	glBindTexture(GL_TEXTURE_2D, floorTexture); opengl_check;

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window_width, window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL); opengl_check;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); opengl_check;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); opengl_check;

	// Generate displacement texture

	vec2 displacement[100][100];

	for (int i = 0; i < 100; i++) {
		for (int j = 0; j < 100; j++) {
			float x = rand() * 0.001 / RAND_MAX;
			float y = rand() * 0.001 / RAND_MAX;
			displacement[i][j] = vec2(x,y);
		}
	}

	glGenTextures(1, &floor.displacement_texture); opengl_check;
	glBindTexture(GL_TEXTURE_2D, floor.displacement_texture); opengl_check;

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, 100, 100, 0, GL_RG, GL_FLOAT, displacement); opengl_check;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); opengl_check;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); opengl_check;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}