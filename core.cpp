#include <core.h>
#include <placement.h>
//#include <imgui.h>
#include <GUI.h>

#include <core_callbacks.h>

#include <physics_server_bullet.h>
#include <physics_server_physx.h>

#include <physics_shape.h>
#include <physics_joint.h>
#include <physics_actor.h>
#include <physics_scene.h>

#include <shared_vehicle.h>
#include <physics_globals.h>

// For time measurement.
#include <windows.h>

const bool IS_PHYSX = false;

const float SERVER_FREQUENCY = 1.0f / 60.0f;
const int SKIP_TICKS = 17;
const int MAX_FRAMESKIP = 5;
const float MASS_SCALING = 0.0001f;


coreApp::~coreApp()
{
	delete m_physics;

	glfwDestroyWindow(m_window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}


coreApp::coreApp()
	:
	is_paused(false),
	m_window(nullptr),
	m_physics(nullptr),
	m_shaders(nullptr),
	showGUI(false),
	vehicleMotionFlag(DEFAULT)
{
	// GL
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	m_window = glfwCreateWindow(1200, 820, "GL test window", NULL, NULL);
	if (! m_window) {
		glfwTerminate();
		exit(1);
	}

	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	Engine_InstallCallbacks(m_window);
	glfwMakeContextCurrent(m_window);
	glfwSwapInterval(1);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_ALPHA_TEST);

	glewExperimental = GL_TRUE;
	glewInit();

	// Shaders
	m_shaders = new myShaders();

	// Imgui
	//mGUI = new GUI(mWindow);

	// Key map
	for (int i = 0; i < 512; ++i) {
		keys_down[i] = false;
	}

	// Physics
	if (IS_PHYSX) {
		m_physics = new physics_server_physx(SERVER_FREQUENCY);
	}
	else {
		m_physics = new physics_server_bullet(SERVER_FREQUENCY);
	}
	m_scene = m_physics->create_scene();
	m_scene->init();
	create_physics_objects();
}


void coreApp::mainLoop(void)
{
	unsigned long next_game_tick = GetTickCount(); // windows only
	int loops;
	float interpolation;

	// The game loop: renders as fast as possible,
	// updates the physics at constant 60Hz frequency.
	while (!glfwWindowShouldClose(m_window)) {

		loops = 0;
		while (GetTickCount() >= next_game_tick && loops < MAX_FRAMESKIP) {
			if (!is_paused) {
				handle_simulation();
			}

			next_game_tick += SKIP_TICKS;
			loops++;
		}

		interpolation = float(GetTickCount() + SKIP_TICKS - next_game_tick) / float(SKIP_TICKS);
		handle_renderer(interpolation);
	}
}


void coreApp::handle_simulation(void)
{
	// Physics
	physics_update();
	m_physics->physics_tick();
	physics_post_update();
}


void coreApp::handle_renderer(const float timestep)
{
	// TODO: provide interpolation

	handle_input();

	//mGUI->NewFrame();

	//ImGui::ShowDemoWindow(&show_demo_window);

	m_scene->render_update();

	// Render
	int display_w, display_h;
	glfwGetFramebufferSize(m_window, &display_w, &display_h);
	glViewport(0, 0, display_w, display_h);
	glClearColor(0.4f, 0.7f, 0.9f, 0.5f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	int width, height;
	glfwGetWindowSize(m_window, &width, &height);
	glm::mat4 projMatrix = glm::perspective(glm::radians(fov), (float)width / (float)height, 0.1f, 1000.0f);
	glm::mat4 viewMatrix = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	//glm::mat4 modelMatrix = glm::rotate(modelMatrix, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 mvpMatrix = projMatrix * viewMatrix; // *modelMatrix;

	// Debug draw lines
	glUseProgram(m_shaders->linesShaderProgram);
	int mvp = glGetUniformLocation(m_shaders->linesShaderProgram, "mvp");
	glUniformMatrix4fv(mvp, 1, false, glm::value_ptr(mvpMatrix));
	m_scene->debug_draw_lines();

	// Debug draw triangles
	glUseProgram(m_shaders->trianglesShaderProgram);
	glUniformMatrix4fv(mvp, 1, false, glm::value_ptr(mvpMatrix));
	m_scene->debug_draw_triangles();

	glfwSwapBuffers(m_window);
}


void coreApp::handle_input(void)
{
	// TODO
	glfwPollEvents();

	// Input - ImGui
	if (keys_down[GLFW_KEY_U]) {
		keys_down[GLFW_KEY_U] = false;
		//ImGui_InstallCallbacks(mWindow);
	}

	// Input - Camera
	if (keys_down[GLFW_KEY_W] == true)
		cameraPos += cameraSpeed * cameraFront;
	if (keys_down[GLFW_KEY_S] == true)
		cameraPos -= cameraSpeed * cameraFront;
	if (keys_down[GLFW_KEY_A] == true)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (keys_down[GLFW_KEY_D] == true)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

	// Input - Vehicle
	if (keys_down[GLFW_KEY_UP] == true) {
		vehicleMotionFlag = UP;
	}
	else if (keys_down[GLFW_KEY_DOWN] == true) {
		vehicleMotionFlag = DOWN;
	}
	else if (keys_down[GLFW_KEY_LEFT] == true) {
		vehicleMotionFlag = LEFT;
	}
	else if (keys_down[GLFW_KEY_RIGHT] == true) {
		vehicleMotionFlag = RIGHT;
	}
	else if (keys_down[GLFW_KEY_C] == true) {
		vehicleMotionFlag = C;
	}
	else {
		vehicleMotionFlag = DEFAULT;
	}

	// Pausing the app
	if (keys_down[GLFW_KEY_P] == true) {
		keys_down[GLFW_KEY_P] = false;
		is_paused = !is_paused;
	}

	// Closing the app
	if (keys_down[GLFW_KEY_ESCAPE] == true) {
		glfwSetWindowShouldClose(m_window, GL_TRUE);
	}
}


void coreApp::create_physics_objects(void)
{
	// The ground (modelled as a heightmap)
	for (size_t idx = 0; idx < 900; ++idx) {
		// Random bump structure for the heightmap.
		mHeightmap[idx] = idx % 7;
	}

	void *const data = &mHeightmap[0];
	const float minHeight = -30.f;
	const float maxHeight = 30.f;
	const int terrainWidth = 10;
	const int terrainLength = 10;
	m_ground_shape = m_physics->create_heightfield(terrainWidth, terrainLength, data, minHeight, maxHeight);

	placement_m groundState;
	if (IS_PHYSX) {
		groundState = placement_m(quat_m(0, 0, 0, 1), vec3_m(-100, -2, -100));
	}
	else {
		groundState = placement_m(quat_m(0, 0, 0, 1), vec3_m(0, -2, 0));
	}

	m_ground_actor = m_physics->new_actor_static(groundState);
	m_ground_actor->attach_shape(m_ground_shape, groundState);
	m_scene->insert(m_ground_actor, COL_GROUND, COL_ALL);

	// The truck
	const placement_m truck_placement(quat_m(0, 0, 0, 1), vec3_m(0, 3, 0));
	const float truck_mass = 7000.f * MASS_SCALING;
	m_truck = new shared_vehicle(m_scene, m_physics, truck_placement, truck_mass, vehicle_type_TRUCK);

	// The trailer
	const placement_m trailer_placement(quat_m(0, 0, 0, 1), vec3_m(14.f, 2.f, 0));
	const float trailer_mass = 12000.f * MASS_SCALING;
	m_trailer = new shared_vehicle(m_scene, m_physics, trailer_placement, trailer_mass, vehicle_type_TRAILER);

	// The hook joint
	const placement_m frame_in_A(vec3_m(5.f, 3.f, 0));
	const placement_m frame_in_B(vec3_m(-10.f, 3.f, 0.f));
	m_hook_joint = m_physics->create_joint_6dof(m_truck->get_actor(), m_trailer->get_actor(), frame_in_A, frame_in_B);
	m_hook_joint->insert(m_scene);
	
	joint_limit_m limit;
	limit.angular_y_max = -1.0f;
	limit.angular_x_max = 0.1f;
	limit.angular_x_min = -0.1f;
	limit.angular_z_max = 0.1f;
	limit.angular_z_min = -0.1f;
	m_hook_joint->set_limits(limit);
}


void coreApp::physics_update(void)
{
}

void coreApp::physics_post_update(void)
{
	if (m_truck && m_truck->getSuspension()->getNumWheels()) {
		switch (vehicleMotionFlag) {
		case LEFT:
			m_truck->getSuspension()->setSteeringValue(0.4f, 0);
			m_truck->getSuspension()->setSteeringValue(0.4f, 1);
			break;
		case RIGHT:
			m_truck->getSuspension()->setSteeringValue(-0.4f, 0);
			m_truck->getSuspension()->setSteeringValue(-0.4f, 1);
			break;
		case UP:
			m_truck->getSuspension()->applyEngineForce(1.0f, 2);
			m_truck->getSuspension()->applyEngineForce(1.0f, 3);
			break;
		case DOWN:
			m_truck->getSuspension()->applyEngineForce(-1.0f, 2);
			m_truck->getSuspension()->applyEngineForce(-1.0f, 3);
			break;
		case C:
			m_truck->getSuspension()->setBrake(10.0f, 2);
			m_truck->getSuspension()->setBrake(10.0f, 3);
			break;
		default:
			m_truck->getSuspension()->setSteeringValue(0.0f, 0);
			m_truck->getSuspension()->setSteeringValue(0.0f, 1);
			break;
		}
	}

	if (m_truck) {
		m_truck->physics_post_update();
	}
	if (m_trailer) {
		m_trailer->physics_post_update();
	}
}


int main(int argc, char *argv[])
{
	coreApp app;
	app.mainLoop();
}




