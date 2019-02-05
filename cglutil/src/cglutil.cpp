#include "cglutil.h"

#include <cstdio>
#include <cstring>
#include <memory>
#include <fstream>
#include <filesystem>

#include <stb_image.h>
#include <glad/glad.h>

#ifdef CGL_USE_GLFW
#include <GLFW/glfw3.h>
#endif

#ifdef WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

namespace cgl
{
namespace detail
{
std::string get_executable_dir()
{
    constexpr int path_length = 256;
    char path_str[path_length];
#ifdef WIN32
    GetModuleFileNameA(nullptr, path_str, path_length);
#else
    readlink("/proc/self/exe", path_str, path_length);
#endif
    return std::filesystem::path(path_str).parent_path().string();
}

void gl_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                       const GLchar* message, const void* userParam)
{
    fprintf(stderr, "GL_ERROR: %s\n", message);
}

}  // namespace detail

std::string read_entire_file(const char* fp)
{
#ifndef WIN32
    /* Create absolute path */
    auto path = std::filesystem::path(detail::get_executable_dir()) / fp;

    /* Attempt to open the file provided with absolute path */
    FILE* file;
    file = fopen(path.c_str(), "r");

    /* On failure, return an invalid GL name */
    if (file == nullptr)
    {
        fprintf(stderr, "readEntireFile failed to open input file:\n%s\n", path.c_str());
        return {};
    }

    /* Get buffer size */
    fseek(file, 0, SEEK_END);
    const auto bufsz = ftell(file);
    fseek(file, 0, SEEK_SET);

    /* Read the file into the buffer */
    auto* buf = new GLchar[bufsz + 1];
    fread(buf, sizeof(GLchar), bufsz, file);
    fclose(file);

    /* Important to terminate string */
    buf[bufsz] = '\0';

    std::string out{buf};
    delete[] buf;
#else
    std::filesystem::path(detail::getExecutableDir()) / fp;

    /* Also, fopen is weird with fs::path on windows since it uses wchar_t for me, so we use ifstream */
    std::ifstream file{path.c_str()};

    std::string out = {};
    std::string line = {};
    while (std::getline(file, line))
    {
        out += line + '\n';
    }
#endif
    return out;
}

GLuint compile_shader(const char* source, GLenum type)
{
    /* Create shader, attach source code and compile it */
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    /* Check for erros */
    int err, len;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &err);

    if (!err)
    {
        /* Get log length */
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
        auto* log = new GLchar[len];

        /* Get the log text and print it */
        const char* typeString = (type == GL_VERTEX_SHADER ? "Vertex" : "Fragment");

        glGetShaderInfoLog(shader, len, &len, log);
        fprintf(stderr, "%s Shader Compile Error:\n%s\n", typeString, log);

        /* Cleanup and return an invalid GL Name */
        delete[] log;
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

GLuint fcompile_shader(const char* filepath, GLenum type)
{
    /* Read the file contents */
    auto source = read_entire_file(filepath);

    /* Use other function to compile the extracted source, then clean up and return */
    GLuint shader = compile_shader(source.c_str(), type);

    return shader;
}

GLuint create_program(const std::vector<GLuint>& shaders)
{
    /* Create program */
    GLuint program = glCreateProgram();

    /* Attach all shaders that should be part of the program */
    for (auto shader : shaders)
    {
        glAttachShader(program, shader);
    }

    /* Link the program */
    glLinkProgram(program);

    /* Check for errors */
    int err, len;
    glGetProgramiv(program, GL_LINK_STATUS, &err);

    if (!err)
    {
        /* Get log length */
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
        auto log = new GLchar[len];

        /* Get the log text and print it */
        glGetProgramInfoLog(program, len, &len, log);
        fprintf(stderr, "Program Link Error:\n%s\n", log);

        /* Cleanup and return an invalid GL Name */
        glDeleteProgram(program);
        delete[] log;
        return 0;
    }

    return program;
}

GLuint make_shader_program(const std::vector<ShaderStage>& stages)
{
    /* Create arrays of stage definitions and compiled shaders */
    std::vector<unsigned> shaders;

    /* Compile all shaders and put them in the shader array */
    for (const auto& stage : stages)
    {
        shaders.push_back(fcompile_shader(stage.sourcePath, stage.stage));
    }

    return create_program(std::vector<GLuint>(std::begin(shaders), std::end(shaders)));
}

std::vector<uint8_t> load_texture(const char* fp)
{
    const auto abs_path = std::filesystem::path(detail::get_executable_dir()) / fp;
    if (std::filesystem::exists(abs_path))
    {
        int w, h, c;

        auto* raw_pixels = stbi_load(abs_path.c_str(), &w, &h, &c, STBI_rgb_alpha);
        std::vector<uint8_t> pixels(w * h * 4);
        memcpy(pixels.data(), raw_pixels, w * h * 4);
        stbi_image_free(raw_pixels);
        return pixels;
    }
    else
    {
        fprintf(stderr, "Failed to load texture (%s)\n", fp);
        return {};
    }
}

#ifdef CGL_USE_GLFW
GLFWwindow* create_window_and_context(unsigned w, unsigned h, const char* title)
{
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifndef NDEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(w, h, title, nullptr, nullptr);
    glfwMakeContextCurrent(window);

    return window;
}
#endif

void create_debug_callback()
{
    glDebugMessageCallback(detail::gl_debug_callback, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, true);
}

}  // namespace cgl
