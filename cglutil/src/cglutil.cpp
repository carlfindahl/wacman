#include "cglutil.h"

#include <regex>
#include <cstdio>
#include <cstring>
#include <sstream>
#include <memory>
#include <fstream>
#include <exception>
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

std::string convert_path_separator(std::string_view str)
{
#ifndef WIN32
    const char* native_separator = "/";
    const char* foreign_separator = R"(\)";
#else
    const char* native_separator = R"(\)";
    constexpr char foreign_separator = '/';
#endif
    std::string str_copy{str};

    /* Scan string until no more occurences of the foreign_separator exists, and replace with native one */
    do
    {
        auto pos = str_copy.find_first_of(foreign_separator);
        if (pos != std::string::npos)
        {
            str_copy.replace(pos, 1, native_separator);
        }
    } while (str_copy.find(foreign_separator) != str_copy.npos);

    return str_copy;
}

/* Define this as nothing for non-Windows platforms so the function declaration still works */
#ifndef WIN32
#define APIENTRY
#endif

static void APIENTRY gl_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                                       const GLchar* message, const void* userParam)
{
    switch (severity)
    {
    case GL_DEBUG_SEVERITY_LOW: /* fprintf(stdout, "GL_MESSAGE: %s\n", message); */ break;
    case GL_DEBUG_SEVERITY_MEDIUM: fprintf(stderr, "GL_WARNING: %s\n", message); break;
    default: fprintf(stderr, "GL_ERROR: %s\n", message); break;
    }
}

}  // namespace detail

std::string native_absolute_path(std::string_view relative_path)
{
    /* Try the path relative to the executable file (this should work most of the time) */
    const auto out_path = detail::convert_path_separator(detail::get_executable_dir() + "/" + relative_path.data());
    if (CGL_LIKELY(std::filesystem::exists(out_path)))
    {
        return out_path;
    }

    /* Otherwise try the path relative to the working directory */
    const auto alt_out_path = std::filesystem::current_path() / detail::convert_path_separator(relative_path.data());
    if (std::filesystem::exists(alt_out_path))
    {
        return alt_out_path.string();
    }
    /* We have failed to find a file that corresponds to this format, throw an exception here */
    else
    {
        /* Write a sensible error message to the buffer before throwing */
        char buffer[255];
        sprintf(buffer, "No path (%s) exists!", out_path.c_str());
        throw std::runtime_error(buffer);
    }
}

std::string read_entire_file(std::string_view fp)
{
    const auto path = native_absolute_path(fp);

#ifndef WIN32
    /* Attempt to open the file provided with absolute path */
    FILE* file = fopen(path.c_str(), "r");
    SCOPE_EXIT { fclose(file); };

    /* On failure, return an invalid GL name */
    if (file == nullptr)
    {
        fprintf(stderr, "read_entire_file failed to open input file:\n%s\n", path.c_str());
        return {};
    }

    /* Get buffer size */
    fseek(file, 0, SEEK_END);
    const auto bufsz = ftell(file);
    fseek(file, 0, SEEK_SET);

    /* Read the file into the buffer */
    auto* buf = new GLchar[bufsz + 1];
    SCOPE_EXIT { delete[] buf; };

    fread(buf, sizeof(GLchar), bufsz, file);

    /* Important to terminate string */
    buf[bufsz] = '\0';
    std::string out{buf};
#else
    /* Also, fopen is weird with fs::path on windows since it uses wchar_t for me, so we use ifstream */
    std::ifstream file{path};
    file.sync_with_stdio(false);

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
        SCOPE_EXIT { delete[] log; };

        /* Get the log text and print it */
        const char* typeString = (type == GL_VERTEX_SHADER ? "Vertex" : "Fragment");

        glGetShaderInfoLog(shader, len, &len, log);
        fprintf(stderr, "%s Shader Compile Error:\n%s\n", typeString, log);

        /* Cleanup and return an invalid GL Name */
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
        SCOPE_EXIT { delete[] log; };

        /* Get the log text and print it */
        glGetProgramInfoLog(program, len, &len, log);
        fprintf(stderr, "Program Link Error:\n%s\n", log);

        /* Cleanup and return an invalid GL Name */
        glDeleteProgram(program);
        return 0;
    }

    return program;
}

GLuint make_shader_program(const std::vector<ShaderStage>& stages)
{
    /* Create arrays of stage definitions and compiled shaders */
    std::vector<unsigned> shaders = {};

    /* Compile all shaders and put them in the shader array */
    for (const auto& stage : stages)
    {
        shaders.emplace_back(fcompile_shader(stage.sourcePath, stage.stage));
    }

    return create_program(std::vector<GLuint>(std::begin(shaders), std::end(shaders)));
}

LoadedTexture load_texture(const char* fp)
{
    const auto abs_path = native_absolute_path(fp);
    if (std::filesystem::exists(abs_path))
    {
        int w, h, c;
        auto* raw_pixels = stbi_load(abs_path.c_str(), &w, &h, &c, STBI_rgb_alpha);
        SCOPE_EXIT { stbi_image_free(raw_pixels); };

        /* Copy pixels into vector and return vector */
        std::vector<uint8_t> pixels(w * h * 4);
        memcpy(pixels.data(), raw_pixels, w * h * 4);
        return LoadedTexture{w, h, std::move(pixels)};
    }
    else
    {
        fprintf(stderr, "Failed to load texture (%s)\n", fp);
        return {};
    }
}

std::vector<LoadedTexture> load_texture_partitioned(const char* fp, int xoffset, int yoffset, int w, int h, int cols, int count)
{
    auto whole_texture = load_texture(fp);

    /* Number of channels (RGBA) */
    constexpr int ch = 4;

    int current_row = 0;
    int current_col = 0;

    /* We have a total of count textures where there are cols sprites per row. We will read rows until we hit the count.
     * For each of those rows, read in a block of pixels encoded as RGBA with dimensions WxH and store that data in a new
     * loaded texture struct.
     */
    std::vector<LoadedTexture> out_textures(count);
    for (int i = 0; i < count; ++i)
    {
        /* Reads a block of pixels into the appropriate loaded texture */
        for (int y = yoffset + h * current_row; y < (yoffset + h * current_row) + h; ++y)
        {
            for (int x = xoffset * ch + w * ch * current_col; x < (xoffset * ch + w * ch * current_col) + w * ch; x += ch)
            {
                /* Each pixel is 4 values, so read R, G, B, A */
                out_textures[i].pixels.push_back(whole_texture.pixels[y * ch * whole_texture.width + x]);
                out_textures[i].pixels.push_back(whole_texture.pixels[y * ch * whole_texture.width + x + 1]);
                out_textures[i].pixels.push_back(whole_texture.pixels[y * ch * whole_texture.width + x + 2]);
                out_textures[i].pixels.push_back(whole_texture.pixels[y * ch * whole_texture.width + x + 3]);
            }
        }

        out_textures[i].width = w;
        out_textures[i].height = h;

        /* Increment the current row and col */
        if (!(++current_col % cols))
        {
            current_col = 0;
            ++current_row;
        }
    }

    return out_textures;
}

unsigned load_gl_texture(const char* fp)
{
    auto texture = load_texture(fp);

    /* Create a default texture from the loaded texture */
    unsigned name = 0u;
    glCreateTextures(GL_TEXTURE_2D, 1, &name);
    glTextureStorage2D(name, 1, GL_RGBA8, texture.width, texture.height);
    glTextureSubImage2D(name, 0, 0, 0, texture.width, texture.height, GL_RGBA, GL_UNSIGNED_BYTE, texture.pixels.data());
    glTextureParameteri(name, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(name, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(name, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(name, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    return name;
}

unsigned load_gl_texture_partitioned(const char* fp, int xoffset, int yoffset, int w, int h, int cols, int count)
{
    const auto tex_data = load_texture_partitioned(fp, xoffset, yoffset, w, h, cols, count);

    GLuint tex_id = 0u;
    glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &tex_id);

    /* Set up sensible default parameters */
    glTextureParameteri(tex_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(tex_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(tex_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTextureParameteri(tex_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    /* Allocate and transfer data to Texture */
    glTextureStorage3D(tex_id, 1, GL_RGBA8, tex_data[0].width, tex_data[0].height, tex_data.size());
    for (int i = 0; i < static_cast<int>(tex_data.size()); ++i)
    {
        /* One texture per array layer */
        glTextureSubImage3D(tex_id, 0, 0, 0, i, tex_data[i].width, tex_data[i].height, 1, GL_RGBA, GL_UNSIGNED_BYTE,
                            tex_data[i].pixels.data());
    }

    return tex_id;
}

LoadedHeightmap load_heightmap(const char* fp)
{
    const auto abs_path = native_absolute_path(fp);
    if (std::filesystem::exists(abs_path))
    {
        int w, h, c;
        auto* raw_pixels = stbi_load_16(abs_path.c_str(), &w, &h, &c, STBI_rgb_alpha);
        SCOPE_EXIT { stbi_image_free(raw_pixels); };

        /* Copy pixels into vector and return vector */
        std::vector<uint16_t> pixels(w * h * 4);
        memcpy(pixels.data(), raw_pixels, size_bytes(pixels));
        return LoadedHeightmap{w, h, std::move(pixels)};
    }
    else
    {
        fprintf(stderr, "Failed to load texture (%s)\n", fp);
        return {};
    }
}

unsigned load_gl_heightmap(const char* fp)
{
    auto heightmap = load_heightmap(fp);

    /* Create a default heightmap from the loaded heightmap */
    unsigned name = 0u;
    glCreateTextures(GL_TEXTURE_2D, 1, &name);
    glTextureStorage2D(name, 1, GL_RGBA16, heightmap.width, heightmap.height);
    glTextureSubImage2D(name, 0, 0, 0, heightmap.width, heightmap.height, GL_RGBA, GL_UNSIGNED_SHORT, heightmap.pixels.data());
    glTextureParameteri(name, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(name, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(name, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(name, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    return name;
}

std::vector<ModelData> load_obj(const char* path)
{
    /* Open file and turn off stdio sync */
    std::ifstream model_file(cgl::native_absolute_path(path));
    model_file.sync_with_stdio(false);

    /* Temporary storage */
    std::vector<std::array<float, 3>> positions{};
    std::vector<std::array<float, 3>> normals{};
    std::vector<std::array<float, 2>> uvs{};

    /* Output Data */
    std::vector<ModelData> out_verts{};

    std::string line;
    const std::regex idx_regex(R"((\d+)\/(\d+)\/(\d+))");
    while (std::getline(model_file, line))
    {
        std::stringstream line_stream(line);
        std::string qualifier{};
        line_stream >> qualifier;

        /* Read a position */
        if (qualifier == "v")
        {
            positions.emplace_back();
            line_stream >> positions.back()[0];
            line_stream >> positions.back()[1];
            line_stream >> positions.back()[2];
            line_stream.ignore();
        }
        /* Read a tex coord */
        else if (qualifier == "vt")
        {
            uvs.emplace_back();
            line_stream >> uvs.back()[0];
            line_stream >> uvs.back()[1];
            uvs.back()[1] = 1.f - uvs.back()[1];
            line_stream.ignore();
        }
        /* Read a normal */
        else if (qualifier == "vn")
        {
            normals.emplace_back();
            line_stream >> normals.back()[0];
            line_stream >> normals.back()[1];
            line_stream >> normals.back()[2];
            line_stream.ignore();
        }
        /* Add a face (triangle) */
        else if (qualifier == "f")
        {
            std::string triplet;
            for (int i = 0; i < 3; ++i)
            {
                line_stream >> triplet;

                std::smatch match{};
                if (!std::regex_match(triplet, match, idx_regex))
                {
                    fprintf(stderr, "Failed to match regex with obj pattern! Fix obj format.");
                }

                /* Read indices from match */
                unsigned pos_idx = std::stoi(match.str(1));
                unsigned uv_idx = std::stoi(match.str(2));
                unsigned nrm_idx = std::stoi(match.str(3));

                /* Construct vertex from indices */
                ModelData vtx = {positions[pos_idx - 1], normals[nrm_idx - 1], uvs[uv_idx - 1]};

                out_verts.push_back(vtx);
            }
            line_stream.ignore();
        }
    }

    return out_verts;
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

    /* Only enable LOW -> HIGH priority debug messages. Ignore Notifications */
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_HIGH, 0, nullptr, true);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_MEDIUM, 0, nullptr, true);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_LOW, 0, nullptr, true);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, false);
}

namespace detail
{
UncaughtExceptionCounter::UncaughtExceptionCounter() : m_exception_count(std::uncaught_exceptions()) {}

bool UncaughtExceptionCounter::new_uncaught_exception() noexcept { return std::uncaught_exceptions() > m_exception_count; }
}  // namespace detail
// namespace detail
}  // namespace cgl
