#ifndef GUI_H_
#define GUI_H_

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <imgui.h>
#include <debugDrawer.h>

struct ImDrawData;
struct GLFWcursor;
struct GLFWwindow;

class GUI
{
public:
	GUI(GLFWwindow *window);
	~GUI();

	void ImGui_Shutdown(void);
	void new_frame	(void);
	void render		(void);

	// Use if you want to reset your rendering device without losing ImGui state.
	void ImGui_InvalidateDeviceObjects();
	bool ImGui_CreateDeviceObjects();
	bool ImGui_CreateFontsTexture();
protected:
	// GLFW data
	GLFWwindow	 *m_Window;
	double       m_Time;
	bool         m_MouseJustPressed[3] = { false, false, false };
	GLFWcursor*  m_MouseCursors[ImGuiMouseCursor_COUNT];

	// OpenGL3 data
	char         m_GlslVersion[32];
	GLuint       m_FontTexture;
	int          m_ShaderHandle, m_VertHandle, m_FragHandle;
	int          m_AttribLocationTex , m_AttribLocationProjMtx;
	int          m_AttribLocationPosition, m_AttribLocationUV, m_AttribLocationColor;
	unsigned	 m_VboHandle, m_ElementsHandle;
};


#endif /* GUI_H_ */
