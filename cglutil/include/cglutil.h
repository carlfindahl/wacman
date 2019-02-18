#pragma once

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

/*!
 * \brief gl_debug_callback is the OpenGL debug message callback function
 */
void gl_debug_callback(unsigned source, unsigned type, unsigned id, unsigned severity, int length, const char* message,
                       const void* userParam);
}  // namespace detail


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
 *  - load_texture_partitioned
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

std::vector<LoadedTexture> load_texture_partitioned(const char* fp, int xoffset, int yoffset, int w, int h, int cols, int count);

/*!
 * \brief size_bytes returns the size in bytes of the elements of the given container.
 * \tparam Container is the type of the container, for example std::vector<int>
 * \param c is the container to check the size of
 * \return the size of the container's contents in bytes
 */
template<typename Container>
constexpr typename Container::size_type size_bytes(const Container& c) noexcept
{
    return c.size() * sizeof(typename Container::value_type);
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

#define CGL_DETAIL_CONCAT_IMPL(s1, s2) s1##s2
#define CGL_DETAIL_CONCAT(s1, s2) CGL_DETAIL_CONCAT_IMPL(s1, s2)
#define CGL_DETAIL_ANON_VAR(name) CGL_DETAIL_CONCAT(name, __COUNTER__)

#define SCOPE_EXIT auto CGL_DETAIL_ANON_VAR(SCOPE_EXIT_STATE) = cgl::detail::ScopeGuardOnExit() + [&]()
#define SCOPE_FAIL auto CGL_DETAIL_ANON_VAR(SCOPE_FAIL_STATE) = cgl::detail::ScopeGuardOnFail() + [&]() noexcept
#define SCOPE_SUCCESS auto CGL_DETAIL_ANON_VAR(SCOPE_SUCCESS_STATE) = cgl::detail::ScopeGuardOnSuccess() + [&]()
}  // namespace cgl

/*
 * MEMORY UTILITIES:
 *
 */
namespace cgl
{
}  // namespace cgl
