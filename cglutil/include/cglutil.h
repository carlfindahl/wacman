#pragma once

#include <array>
#include <vector>
#include <string>
#include <memory>
#include <type_traits>

struct GLFWwindow;

/*
 * IMPLEMENTATION DETAIL STUFF:
 *  - Contains various utilities for getting absolute file paths and other code that users should not care
 *  about.
 *  - Also conatins one public function -> native_absolute_path that takes a relative path and produces an absolute
 *  path on the native format. Assuming relative to the executable directory.
 */
namespace cgl
{
namespace detail
{
/*!
 * \brief get_executable_dir gets the path of the directory the executable is in, not the working directory.
 * Used to interpretrelative paths in various reading functions.
 * \return A string representing the path (not std::filesystem for compatibility)
 */
std::string get_executable_dir();

/*!
 * \brief convert_path_separator converts the path separator of the given string to the native platform's
 * separator. On Unix, this will convert \\ to / and on Windows, it will be the other way round.
 * \param str is the string to convert
 * \return A string with proper separators
 */
std::string convert_path_separator(std::string_view str);

}  // namespace detail

/*!
 * \brief native_absolute_path takes a relative path and returns an absolute path on the native format (Windows / Linux). It will
 * check for relativity both in the working directory and relative to where the executable file is. It is designed to always take
 * a relative path, however.
 * \param relative_path is a relative path to the working directory or to the executable file.
 * \throws std::runtime_error if the given path does not exist in either of the relative locations, the input is wrong and the
 * function can not proceed, so it throws
 * \return An absolute path formatted in the native way. IE: On Windows (C:\Something\Something\s.img) Linux (/home/user/s.img)
 */
std::string native_absolute_path(std::string_view relative_path);
}  // namespace cgl

/*
 * DATA TYPES:
 *  - ShaderStage represents the path and type of stage, used with some shader utilites
 *  - LoadedTexture contains image info and pixel data from a loaded texture
 */
namespace cgl
{
/*!
 * \brief The ShaderStage struct is used to specify the type and location of a specific shader stage when
 *  calling makeShaderProgram.
 */
struct ShaderStage
{
    unsigned stage = 0u;
    const char* sourcePath = nullptr;
};

/*!
 * \brief The LoadedTexture struct represents a texture that has been loaded and contains relevant information about it.
 */
struct LoadedTexture
{
    int width = 0u;
    int height = 0u;
    std::vector<uint8_t> pixels = {};
};

/*!
 * \brief The LoadedTexture struct represents a 16-bit R-only heightmap that has been loaded and contains relevant information
 * about it.
 */
struct LoadedHeightmap
{
    int width = 0u;
    int height = 0u;
    std::vector<uint16_t> pixels = {};
};

/*!
 * \brief The ModelData struct represents vertices in a loaded model based on the obj format
 */
struct ModelData
{
    std::array<float, 3> position{};
    std::array<float, 3> normal{};
    std::array<float, 2> uv{};
};
}  // namespace cgl

/*
 * SHADER UTILITIES:
 *  - read_entire_file is good for quickly reading in a text file (not binary)
 *  - compile_shader will compile a single shader
 *  - fcompile_shader combines readEntireFile and compileShader into one useful call to compile a fileshader
 *  - create_program links together an arbitrary number of DIFFERENT shader stages
 *  - make_shader_program uses a vector of ShaderStages and compiles + links a shader program in one call
 */
namespace cgl
{
/*!
 * \brief read_entire_file reads the entire file at the provided file path
 * \param fp is the filepath to read from
 * \return A string containing the contents of the entire file
 */
std::string read_entire_file(std::string_view fp);

/*!
 * \brief compile_shader compiles a shader from the provided shader source code
 * \param source is the source code of the shader
 * \param type is the type of the shader
 * \return OpenGL name of the newly created shader
 */
unsigned compile_shader(const char* source, unsigned type);

/*!
 * \brief fcompile_shader compiles a shader from the given file
 * \param filepath is the path to the file which contains the shader source code
 * \param type is the type of the shader
 * \return OpenGL name of the newly created shader
 */
unsigned fcompile_shader(const char* filepath, unsigned type);

/*!
 * \brief create_program will create and link a program from the two supplied shaders
 * \param shaders contains all the compiled shaders that should be linked in the program
 * \return OpenGL name of the newly created program
 */
unsigned create_program(const std::vector<unsigned>& shaders);

/*!
 * \brief make_shader_program will create a linked and ready to use shader program using all the stages
 * provided as arguments. It will return 0 if any errors occured.
 * \param stages is a vector of cgl::ShaderStage that contains information about all the stages that should
 * make up the program. This vector should be at least size 2, as you need a vertex and fragment shader.
 * Only when doing a compute shader should this not be size 2, but size 1.
 * \return It will return an OpenGL Name of the program. All shaders compiled in the process will be flagged
 * for deletion. It is the callers responsibility to delete the OpenGL Program after creation.
 */
unsigned make_shader_program(const std::vector<ShaderStage>& stages);
}  // namespace cgl

/* RESOURCE UTILITIES:
 *  - load_texture loads a texture file from disk and returns a vector of R8G8B8A8 pixels
 *  - load_texture_partitioned extracts multiple textures (from an atlas) as separate textures based on parameters
 *  - load_gl_texture loads a texture and copies it to OpenGL returning a texture handle instead
 *  - load_obj loads an obj file as vertices only - non indexed
 *  - size_bytes returns the size of a standard container's contents in bytes
 */
namespace cgl
{
/*!
 * \brief load_texture loads the texture at the given relative filepath.
 * \param fp is the filepath of the texture
 * \return a Texture resource with information and a vector of pixel data laid out like: [R0,G0,B0,A0, R1,G1,B1,A1 ...]
 */
LoadedTexture load_texture(const char* fp);

/*!
 * \brief load_texture_partitioned loads a texture in similar partitions starting at {xoffset,yoffset}, then moving {w,h} pixels
 * out from that. It does this count times and every time it has done {cols} number of tiles it moves to the next row and
 * continues there
 * \param fp is the relative file path
 * \param xoffset is the starting x coordinate to sample from
 * \param yoffset is the starting y coordinate to sample from
 * \param w is the width of each texture to extract
 * \param h is the height of each texture to extract
 * \param cols is the number of columns
 * \param count is the total number of sprites/textures to extract
 * \return a vector of data about the loaded textures, in order
 * \note this function is great for loading animations and storing them as a 2D Array Texture for example
 */
std::vector<LoadedTexture> load_texture_partitioned(const char* fp, int xoffset, int yoffset, int w, int h, int cols, int count);

/*!
 * \brief load_gl_texture is a shortcut helper function to quickly load an entire texture with sensible defaults with OpenGL and
 * get out a handle to that texture
 * \param fp is the relative filepath of the texture
 * \note it is the callers responsibility to destroy the OpenGL texture handle
 * \return an OpenGL handle to the texture
 */
unsigned load_gl_texture(const char* fp);

/*!
 * \brief load_gl_texture_partitioned does the same as {@load_texture_partitioned}, except it creates a default 2D_ARRAY_TEXTURE
 * where each of the partitions are stored on their own layer.
 * \note it is the callers responsibility to destroy the OpenGL texture handle
 * \return an OpenGL handle to the texture
 */
unsigned load_gl_texture_partitioned(const char* fp, int xoffset, int yoffset, int w, int h, int cols, int count);

/*!
 * \brief load_texture loads the heightmap texture at the given relative filepath.
 * \param fp is the filepath of the heightmap
 * \return a heightmap resource with information and a vector of pixel data laid out like: [R0,R1,R2,R3,R4 ...]
 * \note The heigmap must be 16-bits per channel
 */
LoadedHeightmap load_heightmap(const char* fp);

/*!
 * \brief load_gl_heightmap is a shortcut helper function to quickly load an entire heightmap with sensible defaults with OpenGL
 * and get out a handle to that texture
 * \param fp is the relative filepath of the heightmap
 * \note it is the callers responsibility to destroy the OpenGL texture handle
 * \return an OpenGL handle to the heightmap
 */
unsigned load_gl_heightmap(const char* fp);

/*!
 * \brief load_obj loads a single obj file, ignoring indices (vertices only)
 * \param fp is the relative file path of the model
 * \return a vector of vertex data objects that contain positions, normals and texture coordinates
 */
std::vector<ModelData> load_obj(const char* fp);

/*!
 * \brief size_bytes returns the size in bytes of the elements of the given containers
 * \tparam Container is the required container to compute the size
 * \tparam Containers is a parameter pack of containers, for example a couple of std::vector<int>
 * \param c is the required container compute the size of
 * \param cs are the rest of the containers that contribute to the sum of the size
 * \return the sum of the sizes of the given container's contents in bytes
 */
template<typename Container, typename... Containers>
constexpr std::size_t size_bytes(const Container& c, const Containers&... cs) noexcept
{
    if constexpr (sizeof...(Containers) == 0)
    {
        return c.size() * sizeof(typename Container::value_type);
    }
    else
    {
        return (size_bytes(c) + size_bytes(cs...));
    }
}
}  // namespace cgl

#ifdef CGL_USE_GLFW
/*
 * GLFW UTILITIES:
 *  - create_window_and_context sets up a window and active context using OpenGL 4.5 Core Profile
 */
namespace cgl
{
/*!
 * \brief create_window_and_context create a window that is w*h large with the given title. An OpenGL 4.5 Core
 * Context is automatically created and made active. Currently there are no configurable options, but it's a
 * quick way to create a window. Make sure GLFW is initialized before calling this.
 * \param w - width of the window
 * \param h - height of the window
 * \param title - title of the window
 * \return The created window, or nullptr if it somehow failed
 */
GLFWwindow* create_window_and_context(unsigned w, unsigned h, const char* title);
}  // namespace cgl
#endif

/*
 * DEBUG UTILITIES:
 *  - create_debug_callback sets up a default OpenGL debug callback that enables everything by default
 */
namespace cgl
{
/*!
 * \brief create_debug_callback sets the OpenGL debug callback to the default callback.
 */
void create_debug_callback();
}  // namespace cgl

/*
 * DECLARATIVE SCOPE UTILITIES:
 *
 */
namespace cgl
{
namespace detail
{
/*!
 * \brief The UncaughtExceptionCounter class is able to track if any new exceptions have occured since this object was created.
 * This is useful since, for example, a scope guard can use this information to declaratively do operations when things fail or
 * succeed. Inspired by [https://www.youtube.com/watch?v=WjTrfoiB0MQ](This Talk).
 */
class UncaughtExceptionCounter
{
private:
    /* Number of unhandled exceptsion on creation */
    int m_exception_count = 0;

public:
    UncaughtExceptionCounter();

    /*!
     * \brief new_uncaught_exception returns true if there is a new exception since this class was constructed
     * \return true if a new exception has occured
     */
    bool new_uncaught_exception() noexcept;
};

/* Types just to have types for the various operator+'s */
enum class ScopeGuardOnFail
{
};
enum class ScopeGuardOnSuccess
{
};
enum class ScopeGuardOnExit
{
};

/*!
 * \brief ScopeGuard is an unconditional scope guard that unconditionally calls it's clean up function
 */
template<typename Func>
class ScopeGuard
{
private:
    Func m_function{};

public:
    /* Allow lvalue and rvalue construction */
    explicit ScopeGuard(const Func& f) : m_function(f) {}
    explicit ScopeGuard(Func&& f) : m_function(std::move(f)) {}

    /* No copying or moving of this class */
    ScopeGuard(const ScopeGuard&) = delete;
    ScopeGuard(ScopeGuard&&) = delete;
    ScopeGuard& operator=(const ScopeGuard&) = delete;
    ScopeGuard& operator=(ScopeGuard&&) = delete;

    /* Conditionally noexcept based on the fail state because if we have already thrown, then there is no point for us to do so,
     * and if we do it would only call std::terminate anyway so there we go. */
    ~ScopeGuard() { m_function(); }
};

/*!
 * \brief ScopeGuardWithExceptions deals with the Success and Fail conditions of a scope by counting exceptions
 */
template<typename Func, bool ExecuteOnFail>
class ScopeGuardWithExceptions
{
private:
    /* Function to call either on fail or on success */
    Func m_function{};

    /* This counter will determine if we will call the function or not */
    detail::UncaughtExceptionCounter m_counter{};

public:
    /* Allow lvalue and rvalue construction */
    explicit ScopeGuardWithExceptions(const Func& f) : m_function(f) {}
    explicit ScopeGuardWithExceptions(Func&& f) : m_function(std::move(f)) {}

    /* No copying or moving of this class */
    ScopeGuardWithExceptions(const ScopeGuardWithExceptions&) = delete;
    ScopeGuardWithExceptions(ScopeGuardWithExceptions&&) = delete;
    ScopeGuardWithExceptions& operator=(const ScopeGuardWithExceptions&) = delete;
    ScopeGuardWithExceptions& operator=(ScopeGuardWithExceptions&&) = delete;

    /* Conditionally noexcept based on the fail state because if we have already thrown, then there is no point for us to do so,
     * and if we do it would only call std::terminate anyway so there we go. */
    ~ScopeGuardWithExceptions() noexcept(ExecuteOnFail)
    {
        if (ExecuteOnFail == m_counter.new_uncaught_exception())
        {
            m_function();
        }
    }
};

/* Scope Guard Operator+'s to create the correct scope guard based on the empty tag type */

template<typename Func>
ScopeGuard<typename std::decay_t<Func>> operator+(detail::ScopeGuardOnExit, Func&& f)
{
    return ScopeGuard<typename std::decay_t<Func>>(std::forward<Func>(f));
}

template<typename Func>
ScopeGuardWithExceptions<typename std::decay_t<Func>, true> operator+(detail::ScopeGuardOnFail, Func&& f)
{
    return ScopeGuardWithExceptions<typename std::decay_t<Func>, true>(std::forward<Func>(f));
}

template<typename Func>
ScopeGuardWithExceptions<typename std::decay_t<Func>, false> operator+(detail::ScopeGuardOnSuccess, Func&& f)
{
    return ScopeGuardWithExceptions<typename std::decay_t<Func>, false>(std::forward<Func>(f));
}
}  // namespace detail
}  // namespace cgl

/*
 * MEMORY UTILITIES:
 *
 */
namespace cgl
{
}  // namespace cgl

/*
 * MACROS:
 *  Contains macros related to CGL functionality and other useful macros to be used in the program
 */

#define CGL_DETAIL_CONCAT_IMPL(s1, s2) s1##s2
#define CGL_DETAIL_CONCAT(s1, s2) CGL_DETAIL_CONCAT_IMPL(s1, s2)
#define CGL_DETAIL_ANON_VAR(name) CGL_DETAIL_CONCAT(name, __COUNTER__)

#define SCOPE_EXIT auto CGL_DETAIL_ANON_VAR(SCOPE_EXIT_STATE) = cgl::detail::ScopeGuardOnExit() + [&]()
#define SCOPE_FAIL auto CGL_DETAIL_ANON_VAR(SCOPE_FAIL_STATE) = cgl::detail::ScopeGuardOnFail() + [&]() noexcept
#define SCOPE_SUCCESS auto CGL_DETAIL_ANON_VAR(SCOPE_SUCCESS_STATE) = cgl::detail::ScopeGuardOnSuccess() + [&]()

#ifdef _MSC_VER
#define CGL_ALWAYS_INLINE __forceinline
#define CGL_NEVER_INLINE __declspec(noinline)
#define CGL_LIKELY(x) (x)
#define CGL_UNLIKELY(x) (x)
#else
#define CGL_ALWAYS_INLINE inline __attribute__((__always_inline__))
#define CGL_NEVER_INLINE __attribute__((__noinline__))
#define CGL_LIKELY(x) __builtin_expect((bool)(x), 1)
#define CGL_UNLIKELY(x) __builtin_expect((bool)(x), 0)
#endif
