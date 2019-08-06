#ifndef MATHFUNC_H_
#define MATHFUNC_H_

#include<debugDrawer.h>
//#include "imgui.h"

static bool keys_down[512];

static glm::vec3 cameraPos = glm::vec3(0.0f, 5.0f, 12.0f);
static glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
static glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
static float cameraSpeed = 0.5f;

static float deltaTime = 0.0f;
static float lastFrame = 0.0f;

static void Engine_KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
		keys_down[key] = true;
	else if (action == GLFW_RELEASE)
		keys_down[key] = false;
}


static double lastX = 400, lastY = 300;
static double pitch, yaw;
static bool gFirstMouse = true;
static double gSensitivity = 0.08f;
static void Engine_MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (gFirstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		gFirstMouse = false;
	}

	double xoffset = xpos - lastX;
	double yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;
	xoffset *= gSensitivity;
	yoffset *= gSensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	const float fpitch = static_cast<float>(pitch);
	const float fyaw = static_cast<float>(yaw);

	glm::vec3 front;
	front.x = cos(glm::radians(fpitch)) * cos(glm::radians(fyaw));
	front.y = sin(glm::radians(fpitch));
	front.z = cos(glm::radians(fpitch)) * sin(glm::radians(fyaw));
	cameraFront = glm::normalize(front);
}


static float fov = 45.f;
static void Engine_ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (fov >= 1.0f && fov <= 45.0f)
		fov -= static_cast<float>(yoffset);
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 45.0f)
		fov = 45.0f;
}

static void Engine_InstallCallbacks(GLFWwindow* window)
{
	glfwSetCursorPosCallback(window, Engine_MouseCallback);
    glfwSetScrollCallback(window, Engine_ScrollCallback);
    glfwSetKeyCallback(window, Engine_KeyCallback);
}



/** IMGUI callbacks */

/*
static void ImGui_MouseButtonCallback(GLFWwindow*, int button, int action, int)
{
    //if (action == GLFW_PRESS && button >= 0 && button < 3)
        //g_MouseJustPressed[button] = true;
}

static void ImGui_ScrollCallback(GLFWwindow*, double xoffset, double yoffset)
{
    ImGuiIO& io = ImGui::GetIO();
    io.MouseWheelH += (float)xoffset;
    io.MouseWheel += (float)yoffset;
}

static void ImGui_KeyCallback(GLFWwindow* window, int key, int, int action, int mods)
{
    ImGuiIO& io = ImGui::GetIO();
    if (action == GLFW_PRESS && key == GLFW_KEY_U) {
    	Engine_InstallCallbacks(window);
    }
    else if (action == GLFW_PRESS)
        io.KeysDown[key] = true;
    if (action == GLFW_RELEASE)
        io.KeysDown[key] = false;

    (void)mods; // Modifiers are not reliable across systems
    io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
    io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
    io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
    io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
}

static void ImGui_CharCallback(GLFWwindow*, unsigned int c)
{
    ImGuiIO& io = ImGui::GetIO();
    if (c > 0 && c < 0x10000)
        io.AddInputCharacter((unsigned short)c);
}

static void ImGui_InstallCallbacks(GLFWwindow* window)
{
    glfwSetMouseButtonCallback(window, ImGui_MouseButtonCallback);
    glfwSetScrollCallback(window, ImGui_ScrollCallback);
    glfwSetKeyCallback(window, ImGui_KeyCallback);
    glfwSetCharCallback(window, ImGui_CharCallback);
}*/


#endif /* MATHFUNC_H_ */
