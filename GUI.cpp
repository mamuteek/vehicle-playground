#include <GUI.h>
#include <imgui.h>

static const char* ImGui_ImplGlfwGL3_GetClipboardText(void* user_data)
{
	return glfwGetClipboardString((GLFWwindow*)user_data);
}

static void ImGui_ImplGlfwGL3_SetClipboardText(void* user_data, const char* text)
{
	glfwSetClipboardString((GLFWwindow*)user_data, text);
}


GUI::GUI(GLFWwindow *window)
	: m_Window(window),
	  m_Time(0.0f),
	  m_MouseJustPressed{false, false, false},
	  m_MouseCursors{ 0 },
	  m_GlslVersion("#version 150"),
	  m_FontTexture(0),
      m_ShaderHandle(0),
	  m_VertHandle(0),
	  m_FragHandle(0),
	  m_AttribLocationTex(0),
	  m_AttribLocationProjMtx(0),
	  m_AttribLocationPosition(0),
	  m_AttribLocationUV(0),
	  m_AttribLocationColor(0),
	  m_VboHandle(0),
	  m_ElementsHandle(0)
{
	// Custom config
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	io.MouseDrawCursor = true;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	// Store GL version string so we can refer to it later in case we recreate shaders.
	char *glsl_version = "#version 150";
	IM_ASSERT((int)strlen(glsl_version) + 2 < IM_ARRAYSIZE(m_GlslVersion));
	strcpy(m_GlslVersion, glsl_version);
	strcat(m_GlslVersion, "\n");

	// Setup back-end capabilities flags
	io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;   // We can honor GetMouseCursor() values (optional)
	io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;    // We can honor io.WantSetMousePos requests (optional, rarely used)

	// Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
	io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
	io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
	io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
	io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
	io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
	io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
	io.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
	io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
	io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
	io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
	io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
	io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
	io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
	io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
	io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
	io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
	io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

	io.SetClipboardTextFn = ImGui_ImplGlfwGL3_SetClipboardText;
	io.GetClipboardTextFn = ImGui_ImplGlfwGL3_GetClipboardText;
	io.ClipboardUserData = m_Window;

#ifdef _WIN32
	//io.ImeWindowHandle = glfwGetWin32Window(g_Window);
#endif

	// Load cursors
	// FIXME: GLFW doesn't expose suitable cursors for ResizeAll, ResizeNESW, ResizeNWSE. We revert to arrow cursor for those.
	m_MouseCursors[ImGuiMouseCursor_Arrow] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
	m_MouseCursors[ImGuiMouseCursor_TextInput] = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
	m_MouseCursors[ImGuiMouseCursor_ResizeAll] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
	m_MouseCursors[ImGuiMouseCursor_ResizeNS] = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
	m_MouseCursors[ImGuiMouseCursor_ResizeEW] = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
	m_MouseCursors[ImGuiMouseCursor_ResizeNESW] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
	m_MouseCursors[ImGuiMouseCursor_ResizeNWSE] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);

	ImGui::StyleColorsDark();
}


GUI::~GUI()
{
}


// OpenGL3 Render function.
// (this used to be set in io.RenderDrawListsFn and called by ImGui::Render(), but you can now call this directly from your main loop)
// Note that this implementation is little overcomplicated because we are saving/setting up/restoring every OpenGL state explicitly, in order to be able to run within any OpenGL engine that doesn't do so.
void GUI::render(void)
{
	ImDrawData* draw_data = ImGui::GetDrawData();

    // Avoid rendering when minimized, scale coordinates for retina displays
	// (screen coordinates != framebuffer coordinates)
    ImGuiIO& io = ImGui::GetIO();
    int fb_width = (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
    int fb_height = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);
    if (fb_width == 0 || fb_height == 0)
        return;
    draw_data->ScaleClipRects(io.DisplayFramebufferScale);

    // Backup GL state
    GLenum last_active_texture; glGetIntegerv(GL_ACTIVE_TEXTURE, (GLint*)&last_active_texture);
    glActiveTexture(GL_TEXTURE0);
    GLint last_program; glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
    GLint last_texture; glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
    GLint last_sampler; glGetIntegerv(GL_SAMPLER_BINDING, &last_sampler);
    GLint last_array_buffer; glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
    GLint last_element_array_buffer; glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &last_element_array_buffer);
    GLint last_vertex_array; glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);
    GLint last_polygon_mode[2]; glGetIntegerv(GL_POLYGON_MODE, last_polygon_mode);
    GLint last_viewport[4]; glGetIntegerv(GL_VIEWPORT, last_viewport);
    GLint last_scissor_box[4]; glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box);
    GLenum last_blend_src_rgb; glGetIntegerv(GL_BLEND_SRC_RGB, (GLint*)&last_blend_src_rgb);
    GLenum last_blend_dst_rgb; glGetIntegerv(GL_BLEND_DST_RGB, (GLint*)&last_blend_dst_rgb);
    GLenum last_blend_src_alpha; glGetIntegerv(GL_BLEND_SRC_ALPHA, (GLint*)&last_blend_src_alpha);
    GLenum last_blend_dst_alpha; glGetIntegerv(GL_BLEND_DST_ALPHA, (GLint*)&last_blend_dst_alpha);
    GLenum last_blend_equation_rgb; glGetIntegerv(GL_BLEND_EQUATION_RGB, (GLint*)&last_blend_equation_rgb);
    GLenum last_blend_equation_alpha; glGetIntegerv(GL_BLEND_EQUATION_ALPHA, (GLint*)&last_blend_equation_alpha);
    GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
    GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
    GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
    GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);

    // Setup render state: alpha-blending enabled, no face culling,
    // no depth testing, scissor enabled, polygon fill.
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Setup viewport, orthographic projection matrix
    glViewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height);
    const float ortho_projection[4][4] =
    {
        { 2.0f/io.DisplaySize.x, 0.0f,                   0.0f, 0.0f },
        { 0.0f,                  2.0f/-io.DisplaySize.y, 0.0f, 0.0f },
        { 0.0f,                  0.0f,                  -1.0f, 0.0f },
        {-1.0f,                  1.0f,                   0.0f, 1.0f },
    };
    glUseProgram(m_ShaderHandle);
    glUniform1i(m_AttribLocationTex, 0);
    glUniformMatrix4fv(m_AttribLocationProjMtx, 1, GL_FALSE, &ortho_projection[0][0]);
    glBindSampler(0, 0); // Rely on combined texture/sampler state.

    // Recreate the VAO every time. This is to easily allow multiple GL contexts.
    // VAO are not shared among GL contexts and we don't track creation/deletion of windows,
    // so we don't have an obvious key to use to cache them.
    GLuint vao_handle = 0;
    glGenVertexArrays(1, &vao_handle);
    glBindVertexArray(vao_handle);
    glBindBuffer(GL_ARRAY_BUFFER, m_VboHandle);
    glEnableVertexAttribArray(m_AttribLocationPosition);
    glEnableVertexAttribArray(m_AttribLocationUV);
    glEnableVertexAttribArray(m_AttribLocationColor);
    glVertexAttribPointer(m_AttribLocationPosition, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)IM_OFFSETOF(ImDrawVert, pos));
    glVertexAttribPointer(m_AttribLocationUV, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)IM_OFFSETOF(ImDrawVert, uv));
    glVertexAttribPointer(m_AttribLocationColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), (GLvoid*)IM_OFFSETOF(ImDrawVert, col));

    // Draw
    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        const ImDrawIdx* idx_buffer_offset = 0;

        glBindBuffer(GL_ARRAY_BUFFER, m_VboHandle);
        glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmd_list->VtxBuffer.Size * sizeof(ImDrawVert), (const GLvoid*)cmd_list->VtxBuffer.Data, GL_STREAM_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ElementsHandle);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx), (const GLvoid*)cmd_list->IdxBuffer.Data, GL_STREAM_DRAW);

        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++) {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
            if (pcmd->UserCallback) {
                pcmd->UserCallback(cmd_list, pcmd);
            }
            else {
                glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
                glScissor((int)pcmd->ClipRect.x, (int)(fb_height - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
                glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer_offset);
            }
            idx_buffer_offset += pcmd->ElemCount;
        }
    }
    glDeleteVertexArrays(1, &vao_handle);

    // Restore modified GL state
    glUseProgram(last_program);
    glBindTexture(GL_TEXTURE_2D, last_texture);
    glBindSampler(0, last_sampler);
    glActiveTexture(last_active_texture);
    glBindVertexArray(last_vertex_array);
    glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, last_element_array_buffer);
    glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
    glBlendFuncSeparate(last_blend_src_rgb, last_blend_dst_rgb, last_blend_src_alpha, last_blend_dst_alpha);
    if (last_enable_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
    if (last_enable_cull_face) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
    if (last_enable_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
    if (last_enable_scissor_test) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, (GLenum)last_polygon_mode[0]);
    glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
    glScissor(last_scissor_box[0], last_scissor_box[1], (GLsizei)last_scissor_box[2], (GLsizei)last_scissor_box[3]);
}


bool GUI::ImGui_CreateFontsTexture()
{
    // Build texture atlas
    ImGuiIO& io = ImGui::GetIO();
    unsigned char* pixels;
    int width, height;

    // Load as RGBA 32-bits (75% of the memory is wasted, but default font is so small)
    // because it is more likely to be compatible with user's existing shaders.
    // If your ImTextureId represent a higher-level concept than just a GL texture id,
    // consider calling GetTexDataAsAlpha8() instead to save on GPU memory.
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

    // Upload texture to graphics system
    GLint last_texture;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
    glGenTextures(1, &m_FontTexture);
    glBindTexture(GL_TEXTURE_2D, m_FontTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    // Store our identifier
    io.Fonts->TexID = (void *)(intptr_t)m_FontTexture;


    // Restore state
    glBindTexture(GL_TEXTURE_2D, last_texture);

    return true;
}

bool GUI::ImGui_CreateDeviceObjects()
{
    // Backup GL state
    GLint last_texture, last_array_buffer, last_vertex_array;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);

    const GLchar* vertex_shader =
        "uniform mat4 ProjMtx;\n"
        "in vec2 Position;\n"
        "in vec2 UV;\n"
        "in vec4 Color;\n"
        "out vec2 Frag_UV;\n"
        "out vec4 Frag_Color;\n"
        "void main()\n"
        "{\n"
        "	Frag_UV = UV;\n"
        "	Frag_Color = Color;\n"
        "	gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
        "}\n";

    const GLchar* fragment_shader =
        "uniform sampler2D Texture;\n"
        "in vec2 Frag_UV;\n"
        "in vec4 Frag_Color;\n"
        "out vec4 Out_Color;\n"
        "void main()\n"
        "{\n"
        "	Out_Color = Frag_Color * texture( Texture, Frag_UV.st);\n"
        "}\n";

    const GLchar* vertex_shader_with_version[2] = { m_GlslVersion, vertex_shader };
    const GLchar* fragment_shader_with_version[2] = { m_GlslVersion, fragment_shader };

    m_ShaderHandle = glCreateProgram();
    m_VertHandle = glCreateShader(GL_VERTEX_SHADER);
    m_FragHandle = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(m_VertHandle, 2, vertex_shader_with_version, NULL);
    glShaderSource(m_FragHandle, 2, fragment_shader_with_version, NULL);
    glCompileShader(m_VertHandle);
    glCompileShader(m_FragHandle);
    glAttachShader(m_ShaderHandle, m_VertHandle);
    glAttachShader(m_ShaderHandle, m_FragHandle);
    glLinkProgram(m_ShaderHandle);

    m_AttribLocationTex = glGetUniformLocation(m_ShaderHandle, "Texture");
    m_AttribLocationProjMtx = glGetUniformLocation(m_ShaderHandle, "ProjMtx");
    m_AttribLocationPosition = glGetAttribLocation(m_ShaderHandle, "Position");
    m_AttribLocationUV = glGetAttribLocation(m_ShaderHandle, "UV");
    m_AttribLocationColor = glGetAttribLocation(m_ShaderHandle, "Color");

    glGenBuffers(1, &m_VboHandle);
    glGenBuffers(1, &m_ElementsHandle);

    ImGui_CreateFontsTexture();

    // Restore modified GL state
    glBindTexture(GL_TEXTURE_2D, last_texture);
    glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
    glBindVertexArray(last_vertex_array);

    return true;
}

void GUI::ImGui_InvalidateDeviceObjects()
{
    if (m_VboHandle) glDeleteBuffers(1, &m_VboHandle);
    if (m_ElementsHandle) glDeleteBuffers(1, &m_ElementsHandle);
    m_VboHandle = m_ElementsHandle = 0;

    if (m_ShaderHandle && m_VertHandle) glDetachShader(m_ShaderHandle, m_VertHandle);
    if (m_VertHandle) glDeleteShader(m_VertHandle);
    m_VertHandle = 0;

    if (m_ShaderHandle && m_FragHandle) glDetachShader(m_ShaderHandle, m_FragHandle);
    if (m_FragHandle) glDeleteShader(m_FragHandle);
    m_FragHandle = 0;

    if (m_ShaderHandle) glDeleteProgram(m_ShaderHandle);
    m_ShaderHandle = 0;

    if (m_FontTexture) {
        glDeleteTextures(1, &m_FontTexture);
        ImGui::GetIO().Fonts->TexID = 0;
        m_FontTexture = 0;
    }
}


void GUI::ImGui_Shutdown()
{
    // Destroy GLFW mouse cursors
    for (ImGuiMouseCursor cursor_n = 0; cursor_n < ImGuiMouseCursor_COUNT; cursor_n++)
        glfwDestroyCursor(m_MouseCursors[cursor_n]);
    memset(m_MouseCursors, 0, sizeof(m_MouseCursors));

    // Destroy OpenGL objects
    ImGui_InvalidateDeviceObjects();
}

void GUI::new_frame(void)
{
    if (!m_FontTexture)
        ImGui_CreateDeviceObjects();

    ImGuiIO& io = ImGui::GetIO();

    // Setup display size (every frame to accommodate for window resizing)
    int w, h;
    int display_w, display_h;
    glfwGetWindowSize(m_Window, &w, &h);
    glfwGetFramebufferSize(m_Window, &display_w, &display_h);
    io.DisplaySize = ImVec2((float)w, (float)h);
    io.DisplayFramebufferScale = ImVec2(w > 0 ? ((float)display_w / w) : 0, h > 0 ? ((float)display_h / h) : 0);

    // Setup time step
    double current_time =  glfwGetTime();
    io.DeltaTime = m_Time > 0.0 ? (float)(current_time - m_Time) : (float)(1.0f/60.0f);
    m_Time = current_time;

    // Setup inputs
    // (we already got mouse wheel, keyboard keys & characters from glfw callbacks polled in glfwPollEvents())
    if (glfwGetWindowAttrib(m_Window, GLFW_FOCUSED)) {
        // Set OS mouse position if requested Keyboard(only used when
    	// ImGuiConfigFlags_NavEnableSetMousePos is enabled by user).
        if (io.WantSetMousePos) {
            glfwSetCursorPos(m_Window, (double)io.MousePos.x, (double)io.MousePos.y);
        }
        else {
            double mouse_x, mouse_y;
            glfwGetCursorPos(m_Window, &mouse_x, &mouse_y);
            io.MousePos = ImVec2((float)mouse_x, (float)mouse_y);
        }
    }
    else {
        io.MousePos = ImVec2(-FLT_MAX,-FLT_MAX);
    }

    for (int i = 0; i < 3; i++) {
        // If a mouse press event came, always pass it as "mouse held this frame",
    	// so we don't miss click-release events that are shorter than 1 frame.
        io.MouseDown[i] = m_MouseJustPressed[i] || glfwGetMouseButton(m_Window, i) != 0;
        m_MouseJustPressed[i] = false;
    }

    // Update OS/hardware mouse cursor if imgui isn't drawing a software cursor
    if (glfwGetInputMode(m_Window, GLFW_CURSOR) != GLFW_CURSOR_DISABLED)
    {
        ImGuiMouseCursor cursor = ImGui::GetMouseCursor();
        if (io.MouseDrawCursor || cursor == ImGuiMouseCursor_None) {
            glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        }
        else {
            glfwSetCursor(m_Window, m_MouseCursors[cursor] ? m_MouseCursors[cursor] : m_MouseCursors[ImGuiMouseCursor_Arrow]);
            glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }

    // Start the frame. This call will update the io.WantCaptureMouse and
    // io.WantCaptureKeyboard flags that you can use to dispatch inputs (or not) to your application.
    ImGui::NewFrame();
}
