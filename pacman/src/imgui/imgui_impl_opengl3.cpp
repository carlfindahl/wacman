// dear imgui: Renderer for OpenGL3 / OpenGL ES2 / OpenGL ES3 (modern OpenGL with shaders / programmatic
// pipeline) This needs to be used along with a Platform Binding (e.g. GLFW, SDL, Win32, custom..) (Note: We
// are using GL3W as a helper library to access OpenGL functions since there is no standard header to access
// modern OpenGL functions easily. Alternatives are GLEW, Glad, etc..)

// Implemented features:
//  [X] Renderer: User texture binding. Use 'GLuint' OpenGL texture identifier as void*/ImTextureID. Read the
//  FAQ about ImTextureID in imgui.cpp.

// You can copy and use unmodified imgui_impl_* files in your project. See main.cpp for an example of using
// this. If you are new to dear imgui, read examples/README.txt and read the documentation at the top of
// imgui.cpp. https://github.com/ocornut/imgui

// CHANGELOG
// (minor and older changes stripped away, please see git history for details)
//  2018-11-30: Misc: Setting up io.BackendRendererName so it can be displayed in the About Window.
//  2018-11-13: OpenGL: Support for GL 4.5's glClipControl(GL_UPPER_LEFT).
//  2018-08-29: OpenGL: Added support for more OpenGL loaders: glew and glad, with comments indicative that
//  any loader can be used. 2018-08-09: OpenGL: Default to OpenGL ES 3 on iOS and Android. GLSL version
//  default to "#version 300 ES". 2018-07-30: OpenGL: Support for GLSL 300 ES and 410 core. Fixes for
//  Emscripten compilation. 2018-07-10: OpenGL: Support for more GLSL versions (based on the GLSL version
//  string). Added error output when shaders fail to compile/link. 2018-06-08: Misc: Extracted
//  imgui_impl_opengl3.cpp/.h away from the old combined GLFW/SDL+OpenGL3 examples. 2018-06-08: OpenGL: Use
//  draw_data->DisplayPos and draw_data->DisplaySize to setup projection matrix and clipping rectangle.
//  2018-05-25: OpenGL: Removed unnecessary backup/restore of GL_ELEMENT_ARRAY_BUFFER_BINDING since this is
//  part of the VAO state. 2018-05-14: OpenGL: Making the call to glBindSampler() optional so 3.2 context
//  won't fail if the function is a NULL pointer. 2018-03-06: OpenGL: Added const char* glsl_version parameter
//  to ImGui_ImplOpenGL3_Init() so user can override the GLSL version e.g. "#version 150". 2018-02-23: OpenGL:
//  Create the VAO in the render function so the setup can more easily be used with multiple shared GL
//  context. 2018-02-16: Misc: Obsoleted the io.RenderDrawListsFn callback and exposed
//  ImGui_ImplSdlGL3_RenderDrawData() in the .h file so you can call it yourself. 2018-01-07: OpenGL: Changed
//  GLSL shader version from 330 to 150. 2017-09-01: OpenGL: Save and restore current bound sampler. Save and
//  restore current polygon mode. 2017-05-01: OpenGL: Fixed save and restore of current blend func state.
//  2017-05-01: OpenGL: Fixed save and restore of current GL_ACTIVE_TEXTURE.
//  2016-09-05: OpenGL: Fixed save and restore of current scissor rectangle.
//  2016-07-29: OpenGL: Explicitly setting GL_UNPACK_ROW_LENGTH to reduce issues because SDL changes it.
//  (#752)

//----------------------------------------
// OpenGL    GLSL      GLSL
// version   version   string
//----------------------------------------
//  2.0       110       "#version 110"
//  2.1       120
//  3.0       130
//  3.1       140
//  3.2       150       "#version 150"
//  3.3       330
//  4.0       400
//  4.1       410       "#version 410 core"
//  4.2       420
//  4.3       430
//  ES 2.0    100       "#version 100"
//  ES 3.0    300       "#version 300 es"
//----------------------------------------

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include <cstdio>
#include <cglutil.h>
#if defined(_MSC_VER) && _MSC_VER <= 1500  // MSVC 2008 or earlier
#include <stddef.h>                        // intptr_t
#else
#include <cstdint>  // intptr_t
#endif
#if defined(__APPLE__)
#include "TargetConditionals.h"
#endif

// iOS, Android and Emscripten can use GL ES 3
// Call ImGui_ImplOpenGL3_Init() with "#version 300 es"
#if (defined(__APPLE__) && TARGET_OS_IOS) || (defined(__ANDROID__)) || (defined(__EMSCRIPTEN__))
#define USE_GL_ES3
#endif

#ifdef USE_GL_ES3
// OpenGL ES 3
#include <GLES3/gl3.h>  // Use GL ES 3
#else
// Regular OpenGL
// About OpenGL function loaders: modern OpenGL doesn't have a standard header file and requires individual
// function pointers to be loaded manually. Helper libraries are often used for this purpose! Here we are
// supporting a few common ones: gl3w, glew, glad. You may use another loader/header of your choice (glext,
// glLoadGen, etc.), or chose to manually implement your own.
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <glad/glad.h>
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h>
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h>
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif
#endif

// OpenGL Data
static GLuint g_FontTexture = 0;
static GLuint g_ShaderHandle = 0;
static GLuint g_VaoHandle = 0;
static int g_AttribLocationTex = 0, g_AttribLocationProjMtx = 0;
static int g_AttribLocationPosition = 0, g_AttribLocationUV = 0, g_AttribLocationColor = 0;
static unsigned int g_VboHandle = 0, g_ElementsHandle = 0;

// Functions
bool ImGui_ImplOpenGL3_Init()
{
    ImGuiIO& io = ImGui::GetIO();
    io.BackendRendererName = "imgui_impl_opengl3";
    return true;
}

void ImGui_ImplOpenGL3_Shutdown() { ImGui_ImplOpenGL3_DestroyDeviceObjects(); }

void ImGui_ImplOpenGL3_NewFrame()
{
    if (!g_FontTexture)
    {
        ImGui_ImplOpenGL3_CreateDeviceObjects();
    }
}

// OpenGL3 Render function.
// (this used to be set in io.RenderDrawListsFn and called by ImGui::Render(), but you can now call this
// directly from your main loop) Note that this implementation is little overcomplicated because we are
// saving/setting up/restoring every OpenGL state explicitly, in order to be able to run within any OpenGL
// engine that doesn't do so.
void ImGui_ImplOpenGL3_RenderDrawData(ImDrawData* draw_data)
{
    // Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates !=
    // framebuffer coordinates)
    ImGuiIO& io = ImGui::GetIO();
    int fb_width = (int)(draw_data->DisplaySize.x * io.DisplayFramebufferScale.x);
    int fb_height = (int)(draw_data->DisplaySize.y * io.DisplayFramebufferScale.y);
    if (fb_width <= 0 || fb_height <= 0)
    {
        return;
    }
    draw_data->ScaleClipRects(io.DisplayFramebufferScale);

    // Backup GL state
#ifdef GL_POLYGON_MODE
    GLint last_polygon_mode[2];
    glGetIntegerv(GL_POLYGON_MODE, last_polygon_mode);
#endif
    GLint last_viewport[4];
    glGetIntegerv(GL_VIEWPORT, last_viewport);
    GLint last_scissor_box[4];
    glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box);
    GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
    bool clip_origin_lower_left = true;
#ifdef GL_CLIP_ORIGIN
    GLenum last_clip_origin = 0;
    glGetIntegerv(GL_CLIP_ORIGIN, (GLint*)&last_clip_origin);  // Support for GL 4.5's glClipControl(GL_UPPER_LEFT)
    if (last_clip_origin == GL_UPPER_LEFT)
    {
        clip_origin_lower_left = false;
    }
#endif

    // Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled, polygon
    // fill
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
#ifdef GL_POLYGON_MODE
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif

    // Setup viewport, orthographic projection matrix
    // Our visible imgui space lies from draw_data->DisplayPos (top left) to
    // draw_data->DisplayPos+data_data->DisplaySize (bottom right). DisplayMin is typically (0,0) for single
    // viewport apps.
    glViewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height);
    float L = draw_data->DisplayPos.x;
    float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
    float T = draw_data->DisplayPos.y;
    float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;
    const float ortho_projection[4][4] = {
        {2.0f / (R - L), 0.0f, 0.0f, 0.0f},
        {0.0f, 2.0f / (T - B), 0.0f, 0.0f},
        {0.0f, 0.0f, -1.0f, 0.0f},
        {(R + L) / (L - R), (T + B) / (B - T), 0.0f, 1.0f},
    };
    glUseProgram(g_ShaderHandle);
    glUniform1i(g_AttribLocationTex, 0);
    glUniformMatrix4fv(g_AttribLocationProjMtx, 1, GL_FALSE, &ortho_projection[0][0]);

    // Draw
    glBindVertexArray(g_VaoHandle);
    ImVec2 pos = draw_data->DisplayPos;
    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        const ImDrawIdx* idx_buffer_offset = nullptr;

        glNamedBufferData(g_VboHandle, (GLsizeiptr)cmd_list->VtxBuffer.Size * sizeof(ImDrawVert),
                          (const GLvoid*)cmd_list->VtxBuffer.Data, GL_STREAM_DRAW);

        glNamedBufferData(g_ElementsHandle, (GLsizeiptr)cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx),
                          (const GLvoid*)cmd_list->IdxBuffer.Data, GL_STREAM_DRAW);

        glVertexArrayElementBuffer(g_VaoHandle, g_ElementsHandle);
        glVertexArrayVertexBuffer(g_VaoHandle, 0, g_VboHandle, 0u, sizeof(ImDrawVert));

        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
        {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
            ImVec4 clip_rect =
                ImVec4(pcmd->ClipRect.x - pos.x, pcmd->ClipRect.y - pos.y, pcmd->ClipRect.z - pos.x, pcmd->ClipRect.w - pos.y);
            if (clip_rect.x < fb_width && clip_rect.y < fb_height && clip_rect.z >= 0.0f && clip_rect.w >= 0.0f)
            {
                // Apply scissor/clipping rectangle
                if (clip_origin_lower_left)
                {
                    glScissor((int)clip_rect.x, (int)(fb_height - clip_rect.w), (int)(clip_rect.z - clip_rect.x),
                              (int)(clip_rect.w - clip_rect.y));
                }
                else
                {
                    glScissor((int)clip_rect.x, (int)clip_rect.y, (int)clip_rect.z,
                              (int)clip_rect.w);  // Support for GL 4.5's glClipControl(GL_UPPER_LEFT)
                }

                // Bind texture, Draw
                glBindTextureUnit(0, reinterpret_cast<uintptr_t>(pcmd->TextureId));
                glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount,
                               sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer_offset);
            }
            idx_buffer_offset += pcmd->ElemCount;
        }
    }

    if (last_enable_cull_face)
    {
        glEnable(GL_CULL_FACE);
    }
    else
    {
        glDisable(GL_CULL_FACE);
    }
    glDisable(GL_SCISSOR_TEST);
#ifdef GL_POLYGON_MODE
    glPolygonMode(GL_FRONT_AND_BACK, (GLenum)last_polygon_mode[0]);
#endif
    glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
    glScissor(last_scissor_box[0], last_scissor_box[1], (GLsizei)last_scissor_box[2], (GLsizei)last_scissor_box[3]);
}

bool ImGui_ImplOpenGL3_CreateFontsTexture()
{
    // Build texture atlas
    ImGuiIO& io = ImGui::GetIO();
    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

    // Upload texture to graphics system
    glCreateTextures(GL_TEXTURE_2D, 1, &g_FontTexture);
    glTextureParameteri(g_FontTexture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(g_FontTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glTextureStorage2D(g_FontTexture, 1, GL_RGBA8, width, height);
    glTextureSubImage2D(g_FontTexture, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    // Store our identifier
    io.Fonts->TexID = (ImTextureID)(intptr_t)g_FontTexture;
    return true;
}

void ImGui_ImplOpenGL3_DestroyFontsTexture()
{
    if (g_FontTexture)
    {
        ImGuiIO& io = ImGui::GetIO();
        glDeleteTextures(1, &g_FontTexture);
        io.Fonts->TexID = 0;
        g_FontTexture = 0;
    }
}

bool ImGui_ImplOpenGL3_CreateDeviceObjects()
{
    // Shader code
    const GLchar* vertex_shader_glsl_450_core = "#version 450 core\n"
                                                "layout (location = 0) in vec2 Position;\n"
                                                "layout (location = 1) in vec2 UV;\n"
                                                "layout (location = 2) in vec4 Color;\n"
                                                "layout (location = 1) uniform mat4 ProjMtx;\n"
                                                "layout (location = 0) out vec2 Frag_UV;\n"
                                                "layout (location = 1) out vec4 Frag_Color;\n"
                                                "void main()\n"
                                                "{\n"
                                                "    Frag_UV = UV;\n"
                                                "    Frag_Color = Color;\n"
                                                "    gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
                                                "}\n";

    const GLchar* fragment_shader_glsl_450_core = "#version 450 core\n"
                                                  "layout (location = 0) in vec2 Frag_UV;\n"
                                                  "layout (location = 1) in vec4 Frag_Color;\n"
                                                  "layout (location = 0) uniform sampler2D Texture;\n"
                                                  "layout (location = 0) out vec4 Out_Color;\n"
                                                  "void main()\n"
                                                  "{\n"
                                                  "    Out_Color = Frag_Color * texture(Texture, Frag_UV.st);\n"
                                                  "}\n";

    // Create shaders
    GLuint vertex_shader = cgl::compile_shader(vertex_shader_glsl_450_core, GL_VERTEX_SHADER);
    GLuint fragment_shader = cgl::compile_shader(fragment_shader_glsl_450_core, GL_FRAGMENT_SHADER);
    g_ShaderHandle = cgl::create_program({vertex_shader, fragment_shader});

    // Assign shader locations
    g_AttribLocationTex = 0;
    g_AttribLocationProjMtx = 1;
    g_AttribLocationPosition = 0;
    g_AttribLocationUV = 1;
    g_AttribLocationColor = 2;

    // Create buffers
    glCreateBuffers(1, &g_VboHandle);
    glCreateBuffers(1, &g_ElementsHandle);

    // Create VAO
    glCreateVertexArrays(1, &g_VaoHandle);
    glVertexArrayVertexBuffer(g_VaoHandle, 0, g_VboHandle, 0u, sizeof(ImDrawVert));
    glVertexArrayElementBuffer(g_VaoHandle, g_ElementsHandle);

    glVertexArrayAttribBinding(g_VaoHandle, g_AttribLocationPosition, 0);
    glVertexArrayAttribBinding(g_VaoHandle, g_AttribLocationUV, 0);
    glVertexArrayAttribBinding(g_VaoHandle, g_AttribLocationColor, 0);

    glVertexArrayAttribFormat(g_VaoHandle, g_AttribLocationPosition, 2, GL_FLOAT, GL_FALSE, IM_OFFSETOF(ImDrawVert, pos));
    glVertexArrayAttribFormat(g_VaoHandle, g_AttribLocationUV, 2, GL_FLOAT, GL_FALSE, IM_OFFSETOF(ImDrawVert, uv));
    glVertexArrayAttribFormat(g_VaoHandle, g_AttribLocationColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, IM_OFFSETOF(ImDrawVert, col));

    glEnableVertexArrayAttrib(g_VaoHandle, 0);
    glEnableVertexArrayAttrib(g_VaoHandle, 1);
    glEnableVertexArrayAttrib(g_VaoHandle, 2);

    ImGui_ImplOpenGL3_CreateFontsTexture();
    return true;
}

void ImGui_ImplOpenGL3_DestroyDeviceObjects()
{
    if (g_VboHandle)
    {
        glDeleteBuffers(1, &g_VboHandle);
    }
    if (g_ElementsHandle)
    {
        glDeleteBuffers(1, &g_ElementsHandle);
    }
    g_VboHandle = g_ElementsHandle = 0;

    if (g_ShaderHandle)
    {
        glDeleteProgram(g_ShaderHandle);
    }
    g_ShaderHandle = 0;
    if (g_VaoHandle)
    {
        glDeleteVertexArrays(1, &g_VaoHandle);
    }
    g_VaoHandle = 0;
    ImGui_ImplOpenGL3_DestroyFontsTexture();
}
