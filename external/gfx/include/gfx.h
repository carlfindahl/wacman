///////////////////////////////////////////////////////////////////////////////////////////////////
/// gfx.h
///
/// Index:
/// - Version History
/// - Instructions
///     - Include
/// - API & Macros
///     - Logging
///         - Overview
///         - Disabling color output
///         - Example of usage
///     - Assert
///         - Overview
///         - Example of usage
///     - GLEnumToString
///         - Overview
///         - Example of usage
///     - GL_CALL
///         - Overview
///         - Disabling termination on error
///         - Example of usage
///     - Introspection (in development)
///         - Overview
///         - Enabling introspection
/// - Implementation
///     - Logging
///     - GL_CALL
///     - Introspection
///     - GLEnumToString (at the end, due to its size)
///
/// Version History
/// - 2019.02.23 (1.14): - Add asserts to introspection functions
///                      - Add support for some non-square matrices
/// - 2019.02.19 (1.13): - Add support for using colorpicker when editing vec3 and vec4
///                        (Happens on an all or nothing basis as Dear ImGui don't store checkbox state)
///                      - Add support for 2x2 matrix uniforms
///                      - Update documentation for turning on and off gfx assert, debug, and introspection
/// - 2019.01.29 (1.12): - Make all UI modifications to uniforms drag based (ctrl+left click to input manually)
///                      - Add support for 3x3 and 4x4 matrix uniforms
/// - 2019.01.21 (1.11): - #undef min max, and scope disable warnings of sprintf on windows
/// - 2019.01.21 (1.10): - Add support for printing GLenum value if unknown type
/// - 2019.01.21 (1.09): - Fix bug as result of CollapsingHeader not pushing ID on stack
/// - 2019.01.12 (1.08): - Add Introspection of Vertex Array Objects
/// - 2019.01.12 (1.07): - Use Collapsing Headers in Introspection of Shader Program,
///                      - Add function to generate scrollable height
/// - 2019.01.10 (1.06): - Add Introspection of Shader Program
/// - 2019.01.09 (1.05): - Terminate on OpenGL error on default & redo indexing of documentation
/// - 2019.01.09 (1.04): - Add GL_CALL
/// - 2019.01.09 (1.03): - Fix missing () in GFX_ASSERT
/// - 2019.01.09 (1.02): - Rename GlEnumToString to GLEnumToString
/// - 2019.01.08 (1.01): - Add GlEnumToString function
/// - 2019.01.08 (1.00): - Initial "release" GFX_ assert and logs.
///
/// Instructions - Include
/// This library is contained within this single header file to make it more convenient for
/// students to include it into their projects.
/// To avoid introducing unnecessary includes and compile time overhead, this library does not
/// inline everything, but rather requires being implemented in 1 single file.
/// To do this, in 1 (ONE) .cpp file write: #define GFX_IMPLEMENTATION before including gfx.h
/// For example, in main.cpp do
///     #include // some file
///     #include // some other file
///     #define GFX_IMPLEMENTATION
///     #include "gfx.h"
///
/// Failing to do this in any file will lead to errors complaining about
/// undefined references.
/// Defining the macro in multiple files will lead to errors complaining about
/// multiple definitions
///
///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////
/// API - Logging
///////////////////////////////////////////////////////////////////////////////////////////////////
/// OVERVIEW
///
/// Basic functionality for categorized logging to console.
/// The logging statements will either print to stdout or to stderr depending on
/// the category of the message.
///
/// Logging should be done through the different macros,
/// NOT through the Gfx::Detail::Log function!
///
/// All messages will be following the specified format:
/// <category>: <file>: <function>: <line_number>: message.
///
/// Example:
/// [INFO ]: main.cpp  : main      :  7: Initialized all OpenGL components
///
/// The logs must be written with printf syntax, which can be found here:
/// http://en.cppreference.com/w/cpp/io/c/fprintf
///
/// note, you must use .c_str() or .data() when sending a std::string to "%s".
///
///////////////////////////////////////////////////////////////////////////////////////////////////
/// DISABLING COLOR OUTPUT
///
/// Color output has been confirmed to work in the Windows terminal, and in the raw terminal
/// on Ubuntu (tested Ubuntu 16.04 & 18.04).
/// However, I did notice issues when logging to terminal in Visual Studio Code,
/// and have therefore added an option for disabling the colors.
/// To do this define GFX_NO_TERMINAL_COLOR before including gfx.h in the file where gfx.h is
/// defined (the one containing #define GFX_IMPLEMENTATION)
/// For example:
///     #include // some file
///     #include // some other file
///     #define GFX_IMPLEMENTATION
///     #define GFX_NO_TERMINAL_COLOR
///     #include "gfx.h"
///
///////////////////////////////////////////////////////////////////////////////////////////////////
/// EXAMPLE OF USAGE
///
/// #include <string>
/// #include "gfx.h"
///
/// int main()
/// {
///     std::string str = "hey";
///     GFX_DEBUG("Logging debug: %s yas", str.c_str());
///     return 0;
/// }
///
/// Will result in something like:
/// [DEBUG]: main.cpp  : main      :  7: Logging debug: hey yas
///
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <cstdio>
#include <cstdlib>

///////////////////////////////////////////////////////////
/// \brief
///     Prints the specified error to stderr and
///     terminates the program.
///
/// \detailed
///     This error is supposed to be used for
///     unrecoverable errors.
///////////////////////////////////////////////////////////
#define GFX_ERROR(fmt, ...)                                                                                                     \
{                                                                                                                               \
    Gfx::Detail::Log(stderr, "ERROR", Gfx::Detail::TERMINAL_COLOR_FG_RED, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__);    \
    Gfx::Detail::PauseTerminal();                                                                                               \
    std::exit(EXIT_FAILURE);                                                                                                    \
}

///////////////////////////////////////////////////////////
/// \brief
///     Prints the specified warning to stderr.
///     Use for non breaking situations.
///////////////////////////////////////////////////////////
#define GFX_WARN(fmt, ...) \
Gfx::Detail::Log(stderr, "WARN", Gfx::Detail::TERMINAL_COLOR_FG_YELLOW, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__);

///////////////////////////////////////////////////////////
/// \brief
///     Prints regular info to stdout.
///////////////////////////////////////////////////////////
#define GFX_INFO(fmt, ...) \
Gfx::Detail::Log(stdout, "INFO", Gfx::Detail::TERMINAL_COLOR_FG_WHITE, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__);

///////////////////////////////////////////////////////////
/// \brief
///     Prints debug information to stdout.
///
/// \detailed
///     Use this for debug related information,
///     can be turned off by defining GFX_NO_DEBUG
///     on a project wide basis (i.e. in CMake)
///////////////////////////////////////////////////////////
#ifdef NDEBUG
#define GFX_NO_DEBUG
#endif

#ifndef GFX_NO_DEBUG
#define GFX_DEBUG(fmt, ...) \
Gfx::Detail::Log(stdout, "DEBUG", Gfx::Detail::TERMINAL_COLOR_FG_CYAN, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__);

#else
#define GFX_DEBUG(fmt, ...)
#endif

namespace Gfx
{
    namespace Detail
    {
        extern const char* TERMINAL_COLOR_RESET;
        extern const char* TERMINAL_COLOR_FG_BLACK;
        extern const char* TERMINAL_COLOR_FG_RED;
        extern const char* TERMINAL_COLOR_FG_GREEN;
        extern const char* TERMINAL_COLOR_FG_YELLOW;
        extern const char* TERMINAL_COLOR_FG_BLUE;
        extern const char* TERMINAL_COLOR_FG_MAGENTA;
        extern const char* TERMINAL_COLOR_FG_CYAN;
        extern const char* TERMINAL_COLOR_FG_GREY;
        extern const char* TERMINAL_COLOR_FG_WHITE;

        extern const char* TERMINAL_COLOR_BG_BLACK;
        extern const char* TERMINAL_COLOR_BG_RED;
        extern const char* TERMINAL_COLOR_BG_GREEN;
        extern const char* TERMINAL_COLOR_BG_YELLOW;
        extern const char* TERMINAL_COLOR_BG_BLUE;
        extern const char* TERMINAL_COLOR_BG_MAGENTA;
        extern const char* TERMINAL_COLOR_BG_CYAN;
        extern const char* TERMINAL_COLOR_BG_GREY;
        extern const char* TERMINAL_COLOR_BG_WHITE;

        ///////////////////////////////////////////////////////////
        /// \brief
        ///     Actual function which is called by log
        ///     macros.
        ///
        /// \note
        ///     ATTENTION!
        ///     Do not use this function!
        ///     You are supposed to use the logger macros!
        ///////////////////////////////////////////////////////////
        void
        Log(std::FILE* file,
            const char* type,
            const char* color,
            const char* filepath,
            const char* func,
            const int line,
            const char* fmt,
            ...);

        ///////////////////////////////////////////////////////////
        /// Convenience for letting the console "hang"
        /// on windows systems.
        ///////////////////////////////////////////////////////////
        void
        PauseTerminal();
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// API - Assert
///////////////////////////////////////////////////////////////////////////////////////////////////
/// OVERVIEW
///
/// Asserts are "landmines" you place for yourself to avoid making mistakes.
/// An assert is given an expression that is expected to be true, in the case where it is not
/// the program crashes.
///
/// An assertation failure will lead to a message in the terminal with the following format.
/// <category>: <file>: <function>: <line_number>: Assertation failure: <expr>: message.
///
/// The messages must be written with printf syntax, which can be found here:
/// http://en.cppreference.com/w/cpp/io/c/fprintf
///
/// note, you must use .c_str() or .data() when sending a std::string to "%s".
///
///////////////////////////////////////////////////////////////////////////////////////////////////
/// EXAMPLE OF USAGE
///
/// #include <string>
/// #include "gfx.h"
///
/// void func(char* ptr)
/// {
///     GFX_ASSERT(ptr != nullptr, "ptr must not be nullptr");
///     // Do something with ptr
/// }
///
/// int main()
/// {
///     func(nullptr);
///     return 0;
/// }
///
/// Will result in something like:
/// [ERROR]: main.cpp  : func      : 6: Assertion failure: ptr != nullptr: ptr must not be nullptr
///
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
/// \brief
///     Assertation macro, send expr that is
///     expected to be true and also a formatted message.
///
/// \detailed
///     Can be turned off by defining GFX_ASSERT_OFF
///     on a project wide basis (i.e. in CMake)
///////////////////////////////////////////////////////////
#ifndef GFX_ASSERT_OFF
#define GFX_ASSERT(expr, fmt, ...)                                          \
if ((expr)) {}                                                              \
else { GFX_ERROR("Assertion failure: " #expr ": " fmt, ##__VA_ARGS__); }    \

#else
#define GFX_ASSERT(expr, fmt, ...)
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
/// API - GLEnumToString
///////////////////////////////////////////////////////////////////////////////////////////////////
/// OVERVIEW
///
/// OpenGL has a large amount defined hex enum values, which can be heard to read when
/// you don't have access to the name.
/// GLEnumToString is a utility function that will turn those enum values into constant strings.
/// Enums will also include the errors that you get back from OpenGL.
/// Important: This function does not allocate any memory, so DO NOT call delete[]
///            on the strings you get back from this function.
///
/// Note: Since OpenGL contains so many enums, some are obviously missing.
///       I have mainly included those needed for gfx.h to function.
///       If you miss any, please request it in an issue or something :)
///
///////////////////////////////////////////////////////////////////////////////////////////////////
/// EXAMPLE OF USAGE
///
/// #include <string>
/// #include "gfx.h"
///
/// int main()
/// {
///     std::string str = "hey";
///     GFX_DEBUG("Logging debug: %s", Gfx::GLEnumToString(GL_FLOAT_VEC2));
///     return 0;
/// }
///
/// Will result in something like:
/// [DEBUG]: main.cpp  : main      :  7: Logging debug: GL_FLOAT_VEC2
///
///////////////////////////////////////////////////////////////////////////////////////////////////
namespace Gfx
{
    const char* GLEnumToString(unsigned e);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// API - GL_Call
///////////////////////////////////////////////////////////////////////////////////////////////////
/// OVERVIEW
///
/// Error handling is important in OpenGL, but it can be easy to forget.
/// If you make an OpenGL call with the GLCall macro any errors detected from that call
/// and where the call happened will be reported to you through the Gfx logging system.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
/// DISABLING COLOR OUTPUT
///
/// By default we log and terminate the application if we detect an OpenGL error,
/// however, if you do not want that functionality you can remove it.
/// To do this define GFX_NO_TERMINATE_ON_GL_ERROR before including gfx.h in the file where
/// gfx.h is defined (the one containing #define GFX_IMPLEMENTATION).
/// For example:
///     #include // some file
///     #include // some other file
///     #define GFX_IMPLEMENTATION
///     #define GFX_NO_TERMINATION_ON_GL_ERROR
///     #include "gfx.h"
///
///////////////////////////////////////////////////////////////////////////////////////////////////
/// EXAMPLE OF USAGE
///
/// #include "gfx.h"
///
/// int main()
/// {
///     GLuint invalid_program_id = 15;
///     GFX_GL_CALL(glUseProgram(invalid_program_id))
///     return 0;
/// }
///
/// Will result in something like:
/// [GL_ERROR]: main.cpp  : main      :  7: (0x0501): GL_INVALID_VALUE
///
///////////////////////////////////////////////////////////////////////////////////////////////////
#define GFX_GL_CALL(glfunc)                                                                                                                                             \
{                                                                                                                                                                       \
    while (glGetError() != GL_NO_ERROR) {}                                                                                                                              \
    glfunc;                                                                                                                                                             \
    while (GLenum errc = glGetError())                                                                                                                                  \
    {                                                                                                                                                                   \
        Gfx::Detail::Log(stderr, "GL_ERROR", Gfx::Detail::TERMINAL_COLOR_FG_RED, __FILE__, __func__, __LINE__, "(0x%04x): %s", errc, Gfx::GLEnumToString(errc));        \
        Gfx::Detail::GLCallTerminateImpl();                                                                                                                             \
    }                                                                                                                                                                   \
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// API - Introspection (In development)
///////////////////////////////////////////////////////////////////////////////////////////////////
/// OVERVIEW
///
/// The introspection API is supposed to help you get a better view of what is currently
/// going on in the OpenGL world, by being allowed to inspect (and sometimes manipulate)
/// different values.
///
/// To use these utility tools you need to have Dear ImGui (https://github.com/ocornut/imgui)
/// integrated into your projects, and I as far as possible try to follow the same interface
/// style as that used in Dear ImGui.
///
/// These functions will respect the current OpenGL pipeline, and turn back any modifications
/// made.
/// They do not require setting up the pipeline in a specific manner,
/// unless otherwise noted.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
/// ENABLING INTROSPECTION
///
/// Since the introspection functionality requires Dear ImGui as a dependency
/// (and since it is under development) it needs to be enabled before it can be used.
/// To do this GFX_ENABLE_INTROSPECTION must be defined on a project wide bases (i.e. in CMake)
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef GFX_ENABLE_INTROSPECTION
namespace Gfx
{
    ///////////////////////////////////////////////////////
    /// \brief
    ///     Outputs information related to the shader-
    ///     program <program>.
    ///     Information is presented using ImGui, in the
    ///     collapsing header labeled <label>.
    ///
    /// \param label
    ///     The label used to identify the program in
    ///     ImGui. Must be unique.
    ///
    /// \param program
    ///     The program to output information about.
    ///     The program needs to be a valid and linked
    ///     OpenGL program.
    ///     To get source output of the different shaders
    ///     they must not have been deleted or detached.
    ///
    ///////////////////////////////////////////////////////
    void IntrospectShader(const char* label, GLuint program);

    ///////////////////////////////////////////////////////
    /// \brief
    ///     Outputs information related to the vertex-
    ///     array <vao>.
    ///     Information is presented using ImGui, in the
    ///     collapsing header labeled <label>.
    ///
    /// \param label
    ///     The label used to identify the vao in
    ///     ImGui. Must be unique.
    ///
    /// \param vao
    ///     The vertex array to output information about.
    ///     The vao needs to be a valid vertex array
    ///     object.
    ///     The element buffer object belonging to the vao
    ///     must be of type GL_UNSIGNED_INT.
    ///
    ///////////////////////////////////////////////////////
    void IntrospectVertexArray(const char* label, GLuint vao);
}
#endif // GFX_ENABLE_INTROSPECTION



///////////////////////////////////////////////////////////////////////////////////////////////////
/// Implementation
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef GFX_IMPLEMENTATION

#include <cstdarg>
#include <cstring>
#include <vector>

#ifdef _WIN32
#include <Windows.h>
#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

#pragma warning (push)
#pragma warning (disable: 4996)
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Implementation - Logging
///////////////////////////////////////////////////////////////////////////////////////////////////
namespace Gfx
{
    namespace Detail
    {
        #ifdef GFX_NO_TERMINAL_COLOR

            const char* TERMINAL_COLOR_RESET = "";
            const char* TERMINAL_COLOR_FG_BLACK = "";
            const char* TERMINAL_COLOR_FG_RED = "";
            const char* TERMINAL_COLOR_FG_GREEN = "";
            const char* TERMINAL_COLOR_FG_YELLOW = "";
            const char* TERMINAL_COLOR_FG_BLUE = "";
            const char* TERMINAL_COLOR_FG_MAGENTA = "";
            const char* TERMINAL_COLOR_FG_CYAN = "";
            const char* TERMINAL_COLOR_FG_GREY = "";
            const char* TERMINAL_COLOR_FG_WHITE = "";

            const char* TERMINAL_COLOR_BG_BLACK = "";
            const char* TERMINAL_COLOR_BG_RED = "";
            const char* TERMINAL_COLOR_BG_GREEN = "";
            const char* TERMINAL_COLOR_BG_YELLOW = "";
            const char* TERMINAL_COLOR_BG_BLUE = "";
            const char* TERMINAL_COLOR_BG_MAGENTA = "";
            const char* TERMINAL_COLOR_BG_CYAN = "";
            const char* TERMINAL_COLOR_BG_GREY = "";
            const char* TERMINAL_COLOR_BG_WHITE = "";

        #else

            const char* TERMINAL_COLOR_RESET = "\033[0m";
            const char* TERMINAL_COLOR_FG_BLACK = "\033[0;30m";
            const char* TERMINAL_COLOR_FG_RED = "\033[0;31m";
            const char* TERMINAL_COLOR_FG_GREEN = "\033[0;32m";
            const char* TERMINAL_COLOR_FG_YELLOW = "\033[0;33m";
            const char* TERMINAL_COLOR_FG_BLUE = "\033[0;34m";
            const char* TERMINAL_COLOR_FG_MAGENTA = "\033[0;35m";
            const char* TERMINAL_COLOR_FG_CYAN = "\033[0;36m";
            const char* TERMINAL_COLOR_FG_GREY = "\033[0;37m";
            const char* TERMINAL_COLOR_FG_WHITE = "\033[0m";

            const char* TERMINAL_COLOR_BG_BLACK = "\033[0;40m";
            const char* TERMINAL_COLOR_BG_RED = "\033[0;41m";
            const char* TERMINAL_COLOR_BG_GREEN = "\033[0;42m";
            const char* TERMINAL_COLOR_BG_YELLOW = "\033[0;43m";
            const char* TERMINAL_COLOR_BG_BLUE = "\033[0;44m";
            const char* TERMINAL_COLOR_BG_MAGENTA = "\033[0;45m";
            const char* TERMINAL_COLOR_BG_CYAN = "\033[0;46m";
            const char* TERMINAL_COLOR_BG_GREY = "\033[0;47m";
            const char* TERMINAL_COLOR_BG_WHITE = "\033[0m";

        #endif

        void
        Log(std::FILE* file,
            const char* type,
            const char* color,
            const char* filepath,
            const char* func,
            const int line,
            const char* fmt,
            ...)
        {
            // Enable virtual terminal in windows for text color
            #ifdef _WIN32
            static bool initialized = false;
            if (!initialized)
            {
                const auto init_windows = []()
                {
                    const auto outputs = { STD_OUTPUT_HANDLE, STD_ERROR_HANDLE };
                    for (const auto& item : outputs)
                    {
                        HANDLE handle = GetStdHandle(item);
                        if (handle == INVALID_HANDLE_VALUE)
                            return false;

                        DWORD mode;
                        if (!GetConsoleMode(handle, &mode))
                            return false;

                        mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
                        if (!SetConsoleMode(handle, mode))
                            return false;
                    }

                    return true;
                };

                if (!init_windows())
                {
                    std::fprintf(stderr, "Could not enable virtual terminal processing, you will not get any terminal colors\n");
                }
                initialized = true;
            }
            #endif

            // Logging logic
            va_list args1;
            va_start(args1, fmt);
            va_list args2;
            va_copy(args2, args1);

            std::size_t size = 1 + std::vsnprintf(nullptr, 0, fmt, args1);
            std::vector<char> buffer(size);

            va_end(args1);
            std::vsnprintf(buffer.data(), size, fmt, args2);
            va_end(args2);

            #ifdef _WIN32
            const char* filename = std::strrchr(filepath, '\\');
            #else
            const char* filename = std::strrchr(filepath, '/');
            #endif

            filename = (filename)
                     ? filename + 1 // Increment past '/' or '\\'
                     : filepath;

            std::fprintf(file, "%s[%-5s]%s: %-10s: %-10s:%3d: %s\n",
                         color, type, TERMINAL_COLOR_RESET,
                         filename, func,
                         line, buffer.data());

            std::fflush(file);
        }

        void
        PauseTerminal()
        {
            #ifdef _WIN32
                system("pause");
            #endif
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Implementation - GL_Call
///////////////////////////////////////////////////////////////////////////////////////////////////
namespace Gfx
{
    namespace Detail
    {
        void
        GLCallTerminateImpl()
        {
            #ifndef GFX_NO_TERMINATION_ON_GL_ERROR
                std::exit(EXIT_FAILURE);
            #endif
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Implementation - Introspection (In development)
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef GFX_ENABLE_INTROSPECTION
#include "imgui.h"
#include <algorithm>
#include <vector>
#include <cinttypes>

namespace Gfx
{
    namespace Detail
    {
        // Generator macro to avoid duplicating code all the time.
        #define GFX_INTROSPECTION_GENERATE_VARIABLE_RENDER(cputype, count, gltype, glread, glwrite, imguifunc)          \
        {                                                                                                               \
            ImGui::Text(#gltype" %s:", name);                                                                           \
            cputype value[count];                                                                                       \
            glread(program, location, &value[0]);                                                                       \
            if (imguifunc("", &value[0], 0.25f))                                                                        \
                glwrite(program, location, 1, &value[0]);                                                               \
        }

        #define GFX_INTROSPECTION_GENERATE_MATRIX_RENDER(cputype, rows, columns, gltype, glread, glwrite, imguifunc)    \
        {                                                                                                               \
            ImGui::Text(#gltype" %s:", name);                                                                           \
            cputype value[rows * columns];                                                                              \
            int size = rows * columns;                                                                                  \
            glread(program, location, &value[0]);                                                                       \
            int modified = 0;                                                                                           \
            for (int i = 0; i < size; i += rows)                                                                        \
            {                                                                                                           \
                ImGui::PushID(i);                                                                                       \
                modified += imguifunc("", &value[i], 0.25f);                                                            \
                ImGui::PopID();                                                                                         \
            }                                                                                                           \
            if (modified)                                                                                               \
                glwrite(program, location, 1, GL_FALSE, value);                                                         \
        }

        void
        RenderUniformVariable(GLuint program, GLenum type, const char* name, GLint location)
        {
            static bool is_color = false;
            switch (type)
            {
                case GL_FLOAT:
                    GFX_INTROSPECTION_GENERATE_VARIABLE_RENDER(GLfloat, 1, GL_FLOAT, glGetUniformfv, glProgramUniform1fv, ImGui::DragFloat);
                    break;

                case GL_FLOAT_VEC2:
                    GFX_INTROSPECTION_GENERATE_VARIABLE_RENDER(GLfloat, 2, GL_FLOAT_VEC2, glGetUniformfv, glProgramUniform2fv, ImGui::DragFloat2);
                    break;

                case GL_FLOAT_VEC3:
                {
                    ImGui::Checkbox("##is_color", &is_color); ImGui::SameLine();
                    ImGui::Text("GL_FLOAT_VEC3 %s", name); ImGui::SameLine();
                    float value[3];
                    glGetUniformfv(program, location, &value[0]);
                    if ((!is_color && ImGui::DragFloat3("", &value[0])) || (is_color && ImGui::ColorEdit3("Color", &value[0], ImGuiColorEditFlags_NoLabel)))
                        glProgramUniform3fv(program, location, 1, &value[0]);
                }
                    break;

                case GL_FLOAT_VEC4:
                {
                    ImGui::Checkbox("##is_color", &is_color); ImGui::SameLine();
                    ImGui::Text("GL_FLOAT_VEC4 %s", name); ImGui::SameLine();
                    float value[4];
                    glGetUniformfv(program, location, &value[0]);
                    if ((!is_color && ImGui::DragFloat4("", &value[0])) || (is_color && ImGui::ColorEdit4("Color", &value[0], ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreviewHalf)))
                        glProgramUniform4fv(program, location, 1, &value[0]);
                }
                    break;


                case GL_INT:
                    GFX_INTROSPECTION_GENERATE_VARIABLE_RENDER(GLint, 1, GL_INT, glGetUniformiv, glProgramUniform1iv, ImGui::DragInt);
                    break;

                case GL_INT_VEC2:
                    GFX_INTROSPECTION_GENERATE_VARIABLE_RENDER(GLint, 2, GL_INT, glGetUniformiv, glProgramUniform2iv, ImGui::DragInt2);
                    break;

                case GL_INT_VEC3:
                    GFX_INTROSPECTION_GENERATE_VARIABLE_RENDER(GLint, 3, GL_INT, glGetUniformiv, glProgramUniform3iv, ImGui::DragInt3);
                    break;

                case GL_INT_VEC4:
                    GFX_INTROSPECTION_GENERATE_VARIABLE_RENDER(GLint, 4, GL_INT, glGetUniformiv, glProgramUniform4iv, ImGui::DragInt4);
                    break;

                case GL_SAMPLER_2D:
                    GFX_INTROSPECTION_GENERATE_VARIABLE_RENDER(GLint, 1, GL_SAMPLER_2D, glGetUniformiv, glProgramUniform1iv, ImGui::DragInt);
                    break;

                case GL_FLOAT_MAT2:
                    GFX_INTROSPECTION_GENERATE_MATRIX_RENDER(GLfloat, 2, 2, GL_FLOAT_MAT2, glGetUniformfv, glProgramUniformMatrix2fv, ImGui::DragFloat2);
                    break;

                case GL_FLOAT_MAT3:
                    GFX_INTROSPECTION_GENERATE_MATRIX_RENDER(GLfloat, 3, 3, GL_FLOAT_MAT3, glGetUniformfv, glProgramUniformMatrix3fv, ImGui::DragFloat3);
                    break;

                case GL_FLOAT_MAT4:
                    GFX_INTROSPECTION_GENERATE_MATRIX_RENDER(GLfloat, 4, 4, GL_FLOAT_MAT4, glGetUniformfv, glProgramUniformMatrix4fv, ImGui::DragFloat4);
                    break;

                case GL_FLOAT_MAT2x3:
                    GFX_INTROSPECTION_GENERATE_MATRIX_RENDER(GLfloat, 3, 2, GL_FLOAT_MAT2x3, glGetUniformfv, glProgramUniformMatrix2x3fv, ImGui::DragFloat3);
                    break;

                case GL_FLOAT_MAT2x4:
                    GFX_INTROSPECTION_GENERATE_MATRIX_RENDER(GLfloat, 4, 2, GL_FLOAT_MAT2x4, glGetUniformfv, glProgramUniformMatrix2x4fv, ImGui::DragFloat4);
                    break;

                case GL_FLOAT_MAT3x2:
                    GFX_INTROSPECTION_GENERATE_MATRIX_RENDER(GLfloat, 2, 3, GL_FLOAT_MAT3x2, glGetUniformfv, glProgramUniformMatrix3x2fv, ImGui::DragFloat2);
                    break;

                case GL_FLOAT_MAT3x4:
                    GFX_INTROSPECTION_GENERATE_MATRIX_RENDER(GLfloat, 4, 3, GL_FLOAT_MAT3x4, glGetUniformfv, glProgramUniformMatrix3x2fv, ImGui::DragFloat4);
                    break;

                default:
                    ImGui::Text("%s has type %s, which isn't supported yet!", name, GLEnumToString(type));
                    break;
            }
        }

        #undef GFX_INTROSPECTION_GENERATE_VARIABLE_RENDER
        #undef GFX_INTROSPECTION_GENERATE_MATRIX_RENDER

        float
        GetScrollableHeight()
        {
            return ImGui::GetTextLineHeight() * 16;
        }
    }

    void
    IntrospectShader(const char* label, GLuint program)
    {
        GFX_ASSERT(label != nullptr, "The label supplied with program: %u is nullptr", program);
        GFX_ASSERT(glIsProgram(program), "The program: %u is not a valid shader program", program);

        ImGui::PushID(label);
        if (ImGui::CollapsingHeader(label))
        {
            // Uniforms
            ImGui::Indent();
            if (ImGui::CollapsingHeader("Uniforms", ImGuiTreeNodeFlags_DefaultOpen))
            {
                GLint uniform_count;
                glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &uniform_count);

                // Read the length of the longest active uniform.
                GLint max_name_length;
                glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_name_length);

                static std::vector<char> name;
                name.resize(max_name_length);

                for (int i = 0; i < uniform_count; i++)
                {
                    GLint ignored;
                    GLenum type;
                    glGetActiveUniform(program, i, max_name_length, nullptr, &ignored, &type, name.data());

                    const auto location = glGetUniformLocation(program, name.data());
                    ImGui::Indent();
                    ImGui::PushID(i);
                    ImGui::PushItemWidth(-1.0f);
                    Detail::RenderUniformVariable(program, type, name.data(), location);
                    ImGui::PopItemWidth();
                    ImGui::PopID();
                    ImGui::Unindent();
                }
            }
            ImGui::Unindent();

            // Shaders
            ImGui::Indent();
            if (ImGui::CollapsingHeader("Shaders"))
            {
                GLint shader_count;
                glGetProgramiv(program, GL_ATTACHED_SHADERS, &shader_count);

                static std::vector<GLuint> attached_shaders;
                attached_shaders.resize(shader_count);
                glGetAttachedShaders(program, shader_count, nullptr, attached_shaders.data());

                for (const auto& shader : attached_shaders)
                {
                    GLint source_length = 0;
                    glGetShaderiv(shader, GL_SHADER_SOURCE_LENGTH, &source_length);
                    static std::vector<char> source;
                    source.resize(source_length);
                    glGetShaderSource(shader, source_length, nullptr, source.data());

                    GLint type = 0;
                    glGetShaderiv(shader, GL_SHADER_TYPE, &type);

                    ImGui::Indent();
                    auto string_type = GLEnumToString(type);
                    ImGui::PushID(string_type);
                    if (ImGui::CollapsingHeader(string_type))
                    {
                        auto y_size = std::min(ImGui::CalcTextSize(source.data()).y, Detail::GetScrollableHeight());
                        ImGui::InputTextMultiline("", source.data(), source.size(), ImVec2(-1.0f, y_size), ImGuiInputTextFlags_ReadOnly);
                    }
                    ImGui::PopID();
                    ImGui::Unindent();

                }
            }
            ImGui::Unindent();
        }
        ImGui::PopID();
    }

    void
    IntrospectVertexArray(const char* label, GLuint vao)
    {
        GFX_ASSERT(label != nullptr, "The label supplied with VAO: %u is nullptr", vao);
        GFX_ASSERT(glIsVertexArray(vao), "The VAO: %u is not a valid vertex array object", vao);

        ImGui::PushID(label);
        if (ImGui::CollapsingHeader(label))
        {
            ImGui::Indent();

            // Get current bound vertex buffer object so we can reset it back once we are finished.
            GLint current_vbo = 0;
            glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &current_vbo);

            // Get current bound vertex array object so we can reset it back once we are finished.
            GLint current_vao = 0;
            glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &current_vao);
            glBindVertexArray(vao);

            // Get the maximum number of vertex attributes,
            // minimum is 4, I have 16, means that whatever number of attributes is here, it should be reasonable to iterate over.
            GLint max_vertex_attribs = 0;
            glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_vertex_attribs);

            GLint ebo = 0;
            glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &ebo);

            // EBO Visualization
            char buffer[128];
            std::sprintf(buffer, "Element Array Buffer: %d", ebo);
            ImGui::PushID(buffer);
            if (ImGui::CollapsingHeader(buffer))
            {
                ImGui::Indent();
                // Assuming unsigned int atm, as I have not found a way to get out the type of the element array buffer.
                int size = 0;
                glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
                size /= sizeof(GLuint);
                ImGui::Text("Size: %d", size);

                if (ImGui::TreeNode("Buffer Contents"))
                {
                    // TODO: Find a better way to put this out on screen, because this solution will probably not scale good when we get a lot of indices.
                    //       Possible solution: Make it into columns, like the VBO's, and present the indices as triangles.
                    auto ptr = (GLuint*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_READ_ONLY);
                    for (int i = 0; i < size; i++)
                    {
                        ImGui::Text("%u", ptr[i]);
                        ImGui::SameLine();
                        if ((i + 1) % 3 == 0)
                            ImGui::NewLine();
                    }

                    glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

                    ImGui::TreePop();
                }

                ImGui::Unindent();
            }
            ImGui::PopID();

            // VBO Visualization
            for (intptr_t i = 0; i < max_vertex_attribs; i++)
            {
                GLint enabled = 0;
                glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &enabled);

                if (!enabled)
                    continue;

                std::sprintf(buffer, "Attribute: %" PRIdPTR "", i);
                ImGui::PushID(buffer);
                if (ImGui::CollapsingHeader(buffer))
                {
                    ImGui::Indent();
                    // Display meta data
                    GLint buffer = 0;
                    glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, &buffer);
                    ImGui::Text("Buffer: %d", buffer);

                    GLint type = 0;
                    glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_TYPE, &type);
                    ImGui::Text("Type: %s", GLEnumToString(type));

                    GLint dimensions = 0;
                    glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_SIZE, &dimensions);
                    ImGui::Text("Dimensions: %d", dimensions);

                    // Need to bind buffer to get access to parameteriv, and for mapping later
                    glBindBuffer(GL_ARRAY_BUFFER, buffer);

                    GLint size = 0;
                    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
                    ImGui::Text("Size in bytes: %d", size);

                    GLint stride = 0;
                    glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_STRIDE, &stride);
                    ImGui::Text("Stride in bytes: %d", stride);

                    GLvoid* offset = nullptr;
                    glGetVertexAttribPointerv(i, GL_VERTEX_ATTRIB_ARRAY_POINTER, &offset);
                    ImGui::Text("Offset in bytes: %" PRIdPTR "", (intptr_t)offset);

                    GLint usage = 0;
                    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_USAGE, &usage);
                    ImGui::Text("Usage: %s", GLEnumToString(usage));

                    // Create table with indexes and actual contents
                    if (ImGui::TreeNode("Buffer Contents"))
                    {
                        ImGui::BeginChild(ImGui::GetID("vbo contents"), ImVec2(-1.0f, Detail::GetScrollableHeight()), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
                        ImGui::Columns(dimensions + 1);
                        const char* descriptors[] = {"index", "x", "y", "z", "w"};
                        for (int j = 0; j < dimensions + 1; j++)
                        {
                            ImGui::Text("%s", descriptors[j]);
                            ImGui::NextColumn();
                        }
                        ImGui::Separator();

                        auto ptr = (char*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY) + (intptr_t)offset;
                        for (int j = 0, c = 0; j < size; j += stride, c++)
                        {
                            ImGui::Text("%d", c);
                            ImGui::NextColumn();
                            for (int k = 0; k < dimensions; k++)
                            {
                                switch (type)
                                {
                                    case GL_BYTE:           ImGui::Text("% d", *(GLbyte*)   &ptr[j + k * sizeof(GLbyte)]);      break;
                                    case GL_UNSIGNED_BYTE:  ImGui::Text("%u",  *(GLubyte*)  &ptr[j + k * sizeof(GLubyte)]);     break;
                                    case GL_SHORT:          ImGui::Text("% d", *(GLshort*)  &ptr[j + k * sizeof(GLshort)]);     break;
                                    case GL_UNSIGNED_SHORT: ImGui::Text("%u",  *(GLushort*) &ptr[j + k * sizeof(GLushort)]);    break;
                                    case GL_INT:            ImGui::Text("% d", *(GLint*)    &ptr[j + k * sizeof(GLint)]);       break;
                                    case GL_UNSIGNED_INT:   ImGui::Text("%u",  *(GLuint*)   &ptr[j + k * sizeof(GLuint)]);      break;
                                    case GL_FLOAT:          ImGui::Text("% f", *(GLfloat*)  &ptr[j + k * sizeof(GLfloat)]);     break;
                                    case GL_DOUBLE:         ImGui::Text("% f", *(GLdouble*) &ptr[j + k * sizeof(GLdouble)]);    break;
                                }
                                ImGui::NextColumn();
                            }
                        }
                        glUnmapBuffer(GL_ARRAY_BUFFER);
                        ImGui::EndChild();
                        ImGui::TreePop();
                    }
                    ImGui::Unindent();
                }
                ImGui::PopID();
            }

            // Cleanup
            glBindVertexArray(current_vao);
            glBindBuffer(GL_ARRAY_BUFFER, current_vbo);

            ImGui::Unindent();
        }
        ImGui::PopID();
    }
}

#endif // GFX_ENABLE_INTROSPECTION

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Implementation - GLEnumToString
///////////////////////////////////////////////////////////////////////////////////////////////////
namespace Gfx
{
    const char* GLEnumToString(GLenum e)
    {
        #define GFX_TO_STRING_GENERATOR(x) case x: return #x; break;
        switch (e)
        {
            // shader:
            GFX_TO_STRING_GENERATOR(GL_VERTEX_SHADER);
            GFX_TO_STRING_GENERATOR(GL_GEOMETRY_SHADER);
            GFX_TO_STRING_GENERATOR(GL_FRAGMENT_SHADER);

            // buffer usage:
            GFX_TO_STRING_GENERATOR(GL_STREAM_DRAW);
            GFX_TO_STRING_GENERATOR(GL_STREAM_READ);
            GFX_TO_STRING_GENERATOR(GL_STREAM_COPY);
            GFX_TO_STRING_GENERATOR(GL_STATIC_DRAW);
            GFX_TO_STRING_GENERATOR(GL_STATIC_READ);
            GFX_TO_STRING_GENERATOR(GL_STATIC_COPY);
            GFX_TO_STRING_GENERATOR(GL_DYNAMIC_DRAW);
            GFX_TO_STRING_GENERATOR(GL_DYNAMIC_READ);
            GFX_TO_STRING_GENERATOR(GL_DYNAMIC_COPY);

            // errors:
            GFX_TO_STRING_GENERATOR(GL_NO_ERROR);
            GFX_TO_STRING_GENERATOR(GL_INVALID_ENUM);
            GFX_TO_STRING_GENERATOR(GL_INVALID_VALUE);
            GFX_TO_STRING_GENERATOR(GL_INVALID_OPERATION);
            GFX_TO_STRING_GENERATOR(GL_INVALID_FRAMEBUFFER_OPERATION);
            GFX_TO_STRING_GENERATOR(GL_OUT_OF_MEMORY);
            GFX_TO_STRING_GENERATOR(GL_STACK_UNDERFLOW);
            GFX_TO_STRING_GENERATOR(GL_STACK_OVERFLOW);

            // types:
            GFX_TO_STRING_GENERATOR(GL_BYTE);
            GFX_TO_STRING_GENERATOR(GL_UNSIGNED_BYTE);
            GFX_TO_STRING_GENERATOR(GL_SHORT);
            GFX_TO_STRING_GENERATOR(GL_UNSIGNED_SHORT);
            GFX_TO_STRING_GENERATOR(GL_FLOAT);
            GFX_TO_STRING_GENERATOR(GL_FLOAT_VEC2);
            GFX_TO_STRING_GENERATOR(GL_FLOAT_VEC3);
            GFX_TO_STRING_GENERATOR(GL_FLOAT_VEC4);
            GFX_TO_STRING_GENERATOR(GL_DOUBLE);
            GFX_TO_STRING_GENERATOR(GL_DOUBLE_VEC2);
            GFX_TO_STRING_GENERATOR(GL_DOUBLE_VEC3);
            GFX_TO_STRING_GENERATOR(GL_DOUBLE_VEC4);
            GFX_TO_STRING_GENERATOR(GL_INT);
            GFX_TO_STRING_GENERATOR(GL_INT_VEC2);
            GFX_TO_STRING_GENERATOR(GL_INT_VEC3);
            GFX_TO_STRING_GENERATOR(GL_INT_VEC4);
            GFX_TO_STRING_GENERATOR(GL_UNSIGNED_INT);
            GFX_TO_STRING_GENERATOR(GL_UNSIGNED_INT_VEC2);
            GFX_TO_STRING_GENERATOR(GL_UNSIGNED_INT_VEC3);
            GFX_TO_STRING_GENERATOR(GL_UNSIGNED_INT_VEC4);
            GFX_TO_STRING_GENERATOR(GL_BOOL);
            GFX_TO_STRING_GENERATOR(GL_BOOL_VEC2);
            GFX_TO_STRING_GENERATOR(GL_BOOL_VEC3);
            GFX_TO_STRING_GENERATOR(GL_BOOL_VEC4);
            GFX_TO_STRING_GENERATOR(GL_FLOAT_MAT2);
            GFX_TO_STRING_GENERATOR(GL_FLOAT_MAT3);
            GFX_TO_STRING_GENERATOR(GL_FLOAT_MAT4);
            GFX_TO_STRING_GENERATOR(GL_FLOAT_MAT2x3);
            GFX_TO_STRING_GENERATOR(GL_FLOAT_MAT2x4);
            GFX_TO_STRING_GENERATOR(GL_FLOAT_MAT3x2);
            GFX_TO_STRING_GENERATOR(GL_FLOAT_MAT3x4);
            GFX_TO_STRING_GENERATOR(GL_FLOAT_MAT4x2);
            GFX_TO_STRING_GENERATOR(GL_FLOAT_MAT4x3);
            GFX_TO_STRING_GENERATOR(GL_DOUBLE_MAT2);
            GFX_TO_STRING_GENERATOR(GL_DOUBLE_MAT3);
            GFX_TO_STRING_GENERATOR(GL_DOUBLE_MAT4);
            GFX_TO_STRING_GENERATOR(GL_DOUBLE_MAT2x3);
            GFX_TO_STRING_GENERATOR(GL_DOUBLE_MAT2x4);
            GFX_TO_STRING_GENERATOR(GL_DOUBLE_MAT3x2);
            GFX_TO_STRING_GENERATOR(GL_DOUBLE_MAT3x4);
            GFX_TO_STRING_GENERATOR(GL_DOUBLE_MAT4x2);
            GFX_TO_STRING_GENERATOR(GL_DOUBLE_MAT4x3);
            GFX_TO_STRING_GENERATOR(GL_SAMPLER_1D);
            GFX_TO_STRING_GENERATOR(GL_SAMPLER_2D);
            GFX_TO_STRING_GENERATOR(GL_SAMPLER_3D);
            GFX_TO_STRING_GENERATOR(GL_SAMPLER_CUBE);
            GFX_TO_STRING_GENERATOR(GL_SAMPLER_1D_SHADOW);
            GFX_TO_STRING_GENERATOR(GL_SAMPLER_2D_SHADOW);
            GFX_TO_STRING_GENERATOR(GL_SAMPLER_1D_ARRAY);
            GFX_TO_STRING_GENERATOR(GL_SAMPLER_2D_ARRAY);
            GFX_TO_STRING_GENERATOR(GL_SAMPLER_1D_ARRAY_SHADOW);
            GFX_TO_STRING_GENERATOR(GL_SAMPLER_2D_ARRAY_SHADOW);
            GFX_TO_STRING_GENERATOR(GL_SAMPLER_2D_MULTISAMPLE);
            GFX_TO_STRING_GENERATOR(GL_SAMPLER_2D_MULTISAMPLE_ARRAY);
            GFX_TO_STRING_GENERATOR(GL_SAMPLER_CUBE_SHADOW);
            GFX_TO_STRING_GENERATOR(GL_SAMPLER_BUFFER);
            GFX_TO_STRING_GENERATOR(GL_SAMPLER_2D_RECT);
            GFX_TO_STRING_GENERATOR(GL_SAMPLER_2D_RECT_SHADOW);
            GFX_TO_STRING_GENERATOR(GL_INT_SAMPLER_1D);
            GFX_TO_STRING_GENERATOR(GL_INT_SAMPLER_2D);
            GFX_TO_STRING_GENERATOR(GL_INT_SAMPLER_3D);
            GFX_TO_STRING_GENERATOR(GL_INT_SAMPLER_CUBE);
            GFX_TO_STRING_GENERATOR(GL_INT_SAMPLER_1D_ARRAY);
            GFX_TO_STRING_GENERATOR(GL_INT_SAMPLER_2D_ARRAY);
            GFX_TO_STRING_GENERATOR(GL_INT_SAMPLER_2D_MULTISAMPLE);
            GFX_TO_STRING_GENERATOR(GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY);
            GFX_TO_STRING_GENERATOR(GL_INT_SAMPLER_BUFFER);
            GFX_TO_STRING_GENERATOR(GL_INT_SAMPLER_2D_RECT);
            GFX_TO_STRING_GENERATOR(GL_UNSIGNED_INT_SAMPLER_1D);
            GFX_TO_STRING_GENERATOR(GL_UNSIGNED_INT_SAMPLER_2D);
            GFX_TO_STRING_GENERATOR(GL_UNSIGNED_INT_SAMPLER_3D);
            GFX_TO_STRING_GENERATOR(GL_UNSIGNED_INT_SAMPLER_CUBE);
            GFX_TO_STRING_GENERATOR(GL_UNSIGNED_INT_SAMPLER_1D_ARRAY);
            GFX_TO_STRING_GENERATOR(GL_UNSIGNED_INT_SAMPLER_2D_ARRAY);
            GFX_TO_STRING_GENERATOR(GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE);
            GFX_TO_STRING_GENERATOR(GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY);
            GFX_TO_STRING_GENERATOR(GL_UNSIGNED_INT_SAMPLER_BUFFER);
            GFX_TO_STRING_GENERATOR(GL_UNSIGNED_INT_SAMPLER_2D_RECT);
            GFX_TO_STRING_GENERATOR(GL_IMAGE_1D);
            GFX_TO_STRING_GENERATOR(GL_IMAGE_2D);
            GFX_TO_STRING_GENERATOR(GL_IMAGE_3D);
            GFX_TO_STRING_GENERATOR(GL_IMAGE_2D_RECT);
            GFX_TO_STRING_GENERATOR(GL_IMAGE_CUBE);
            GFX_TO_STRING_GENERATOR(GL_IMAGE_BUFFER);
            GFX_TO_STRING_GENERATOR(GL_IMAGE_1D_ARRAY);
            GFX_TO_STRING_GENERATOR(GL_IMAGE_2D_ARRAY);
            GFX_TO_STRING_GENERATOR(GL_IMAGE_2D_MULTISAMPLE);
            GFX_TO_STRING_GENERATOR(GL_IMAGE_2D_MULTISAMPLE_ARRAY);
            GFX_TO_STRING_GENERATOR(GL_INT_IMAGE_1D);
            GFX_TO_STRING_GENERATOR(GL_INT_IMAGE_2D);
            GFX_TO_STRING_GENERATOR(GL_INT_IMAGE_3D);
            GFX_TO_STRING_GENERATOR(GL_INT_IMAGE_2D_RECT);
            GFX_TO_STRING_GENERATOR(GL_INT_IMAGE_CUBE);
            GFX_TO_STRING_GENERATOR(GL_INT_IMAGE_BUFFER);
            GFX_TO_STRING_GENERATOR(GL_INT_IMAGE_1D_ARRAY);
            GFX_TO_STRING_GENERATOR(GL_INT_IMAGE_2D_ARRAY);
            GFX_TO_STRING_GENERATOR(GL_INT_IMAGE_2D_MULTISAMPLE);
            GFX_TO_STRING_GENERATOR(GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY);
            GFX_TO_STRING_GENERATOR(GL_UNSIGNED_INT_IMAGE_1D);
            GFX_TO_STRING_GENERATOR(GL_UNSIGNED_INT_IMAGE_2D);
            GFX_TO_STRING_GENERATOR(GL_UNSIGNED_INT_IMAGE_3D);
            GFX_TO_STRING_GENERATOR(GL_UNSIGNED_INT_IMAGE_2D_RECT);
            GFX_TO_STRING_GENERATOR(GL_UNSIGNED_INT_IMAGE_CUBE);
            GFX_TO_STRING_GENERATOR(GL_UNSIGNED_INT_IMAGE_BUFFER);
            GFX_TO_STRING_GENERATOR(GL_UNSIGNED_INT_IMAGE_1D_ARRAY);
            GFX_TO_STRING_GENERATOR(GL_UNSIGNED_INT_IMAGE_2D_ARRAY);
            GFX_TO_STRING_GENERATOR(GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE);
            GFX_TO_STRING_GENERATOR(GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY);
            GFX_TO_STRING_GENERATOR(GL_UNSIGNED_INT_ATOMIC_COUNTER);
        }

        static char buffer[32];
        std::sprintf(buffer, "Unknown GLenum: (0x%04x)", e);
        return buffer;

        #undef GFX_TO_STRING_GENERATOR
    }
}

#if _WIN32
    #pragma warning (pop)
#endif

#endif // GFX_IMPLEMENTATION
