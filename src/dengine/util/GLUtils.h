#pragma once

#include "dengine/platform/CommonGL.h"
#include "dengine/platform/Logger.h"

namespace GLUtils
{
/** helper macro involving the checkGLError function
 *
 * The macro uses __FUNCTION__ and __LINE__ as arguments to the checkGLError function.
 * Use it anywhere you like to catch opengl errors.
 */
#define CHECK_GL_ERROR()                                                                                                         \
	do                                                                                                                           \
	{                                                                                                                            \
		GLUtils::checkGLError(__FUNCTION__, __LINE__);                                                                           \
	} while (0)

void checkGLError(const char* where, int line);

bool loadTexImage2D(const std::string& fileName, GLenum target);

GLuint createTexture(const std::string& fileName, bool mipmap = true);

/** helper function for shader compiling
 *
 * The function internaly uses glCreateShader, glShaderSource and glCompileShader sequence.
 * glGetShaderiv is used to detect compile errors, the function prints GLSL errors and warnings to stderr.
 *
 * \param eShaderType we support GL_VERTEX_SHADER, GL_FRAGMENT_SHADER and GL_GEOMETRY_SHADER values
 * \param strShaderText GLSL code to be used as a glShaderSource argument
 * \return opengl shader id or 0 on failure
 */
GLuint createShaderFromSource(GLenum eShaderType, const std::string& strShaderText);

/** helper function for shader compiling and loading
 *
 * This funcstion loads shader source from file named filename and calls createShaderFromSource().
 *
 * \param eShaderType we support GL_VERTEX_SHADER, GL_FRAGMENT_SHADER and GL_GEOMETRY_SHADER values
 * \param filename file to use as a shader source
 * \return opengl shader id or 0 on failure
 */
GLuint createShaderFromFile(GLenum eShaderType, const std::string& filename);

static bool linkProgram(GLuint program);

/** deletes the shader program and all attached shaders
 *
 * You should delete all shader programs before your application terminates.
 */
void deleteProgramAndShaders(GLuint program);

/** helper function for shader linking - overloaded
 *
 * This is the overloaded version of createProgram().
 *
 * \see createProgram
 * \param shaders zero ended array of shader ids - for example {vsid, fsid, 0}
 * \return opengl shader program id or 0 on failure
 */
GLuint createProgram(const GLuint* shaders);

/** helper function for shader linking
 *
 * The function internaly uses glCreateProgram, glAttachShader to attach all supplied shaders and finally glLinkProgram.
 * Any GLSL errors and warnings are printed to stderr.
 *
 * \param shaderList array of compiled shaders to attach, you can use createShaderFromSource() for example to fill the array
 * \return opengl shader program id or 0 on failure
 */
GLuint createProgram(const std::vector<GLuint>& shaderList);

namespace Debug
{
/// Returns human readable representation of debug source src
const char* sourceToString(GLenum src);
/// Returns human readable representation of debug type
const char* typeToString(GLenum type);
/// Returns human readable representation of debug severity
const char* severityToString(GLenum severity);
/**
 * Tries to print as nice as possible backtrace of the current call stack.
 *
 * \warning The function will chop out parts of the stack. For example the function itself will not appear in the backtrace.
 * \warning The format or verbosity may be platform dependent.
 * \author Tomas Barak, pgr-framework DCGI CVUT FEL
 */
void printBacktrace();

} // namespace Debug

} // namespace GLUtils
