#include "GLUtils.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace GLUtils
{

void checkGLError(const char* where, int line)
{
	GLenum err = glGetError();
	if (err == GL_NONE)
		return;

	std::string errString = "<unknown>";
	switch (err)
	{
	case GL_INVALID_ENUM:
		errString = "GL_INVALID_ENUM";
		break;
	case GL_INVALID_VALUE:
		errString = "GL_INVALID_VALUE";
		break;
	case GL_INVALID_OPERATION:
		errString = "GL_INVALID_OPERATION";
		break;
	case GL_INVALID_FRAMEBUFFER_OPERATION:
		errString = "GL_INVALID_FRAMEBUFFER_OPERATION";
		break;
	case GL_OUT_OF_MEMORY:
		errString = "GL_OUT_OF_MEMORY";
		break;
	default:;
	}
	if (where == 0 || *where == 0)
		LOG_ERROR("GL error occurred: {}", errString)
	else
		LOG_ERROR("GL error occurred in {}:{}: {}", where, line, errString)
}

bool GLUtils::loadTexImage2D(const std::string& fileName, GLenum target)
{
	bool result = false;

	stbi_set_flip_vertically_on_load(false);

	int width, height, Bpp;
	unsigned char* data = stbi_load(fileName.c_str(), &width, &height, &Bpp, STBI_rgb_alpha);

	if (data)
	{
		// printf("%s,bpp %d\n",fileName.c_str(),Bpp);
		glTexImage2D(target, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		result = true;
	}
	else
	{
		LOG_ERROR("[STB_IMAGE] Failed to load texture '{}'!", fileName);
	}
	stbi_image_free(data);

	return result;
}

GLuint GLUtils::createTexture(const std::string& fileName, bool mipmap)
{
	// generate and bind one texture
	GLuint tex = 0;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	// set linear filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// upload our image data to OpenGL
	if (!loadTexImage2D(fileName, GL_TEXTURE_2D))
	{
		glBindTexture(GL_TEXTURE_2D, 0);
		glDeleteTextures(1, &tex);
		return 0;
	}
	// create mipmaps
	if (mipmap)
		glGenerateMipmap(GL_TEXTURE_2D);
	// unbind the texture (just in case someone will mess up with texture calls later)
	glBindTexture(GL_TEXTURE_2D, 0);
	CHECK_GL_ERROR();
	return tex;
}

GLuint createShaderFromSource(GLenum eShaderType, const std::string& strShaderText)
{
	GLuint shader = glCreateShader(eShaderType);
	const char* strFileData = strShaderText.c_str();
	glShaderSource(shader, 1, &strFileData, NULL);

	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar* strInfoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);

		const char* strShaderType = NULL;
		switch (eShaderType)
		{
		case GL_VERTEX_SHADER:
			strShaderType = "vertex";
			break;
		case GL_FRAGMENT_SHADER:
			strShaderType = "fragment";
			break;
		case GL_GEOMETRY_SHADER:
			strShaderType = "geometry";
			break;
		}

		LOG_ERROR("Compile failure in {} shader:\n{}", strShaderType, strInfoLog);

		delete[] strInfoLog;
		glDeleteShader(shader);
		return 0;
	}

	CHECK_GL_ERROR();
	return shader;
}

GLuint createShaderFromFile(GLenum eShaderType, const std::string& filename)
{
	FILE* f = fopen(filename.c_str(), "rb");
	if (!f)
	{
		LOG_ERROR("Unable to open file {} for reading!", filename);
		return 0;
	}
	else
	{
		LOG_INFO("Loading shader : {}", filename);
	}

	fseek(f, 0, SEEK_END);
	int length = ftell(f);
	fseek(f, 0, SEEK_SET);

	char* buffer = new char[length + 1];
	fread(buffer, 1, length, f);
	fclose(f);
	buffer[length] = '\0';

	GLuint sh = createShaderFromSource(eShaderType, buffer);
	delete[] buffer;
	return sh;
}

static bool linkProgram(GLuint program)
{
	glLinkProgram(program);

	GLint status = GL_FALSE;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	CHECK_GL_ERROR();

	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar* strInfoLog = new GLchar[infoLogLength + 1];
		glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
		fprintf(stderr, "Linker failure: %s\n", strInfoLog);
		delete[] strInfoLog;
		return false;
	}
	return true;
}

void deleteProgramAndShaders(GLuint program)
{
	const GLsizei MAX_COUNT = 10; // Pisvejc's constant
	GLsizei count = 0;
	GLuint shaders[MAX_COUNT];

	if (program != 0)
	{
		glGetAttachedShaders(program, MAX_COUNT, &count, shaders);
		for (int i = 0; i < count; i++)
			glDeleteShader(shaders[i]);

		glDeleteProgram(program);
	}
}

GLuint createProgram(const GLuint* shaders)
{
	GLuint program = glCreateProgram();

	while (shaders && *shaders)
		glAttachShader(program, *shaders++);
	CHECK_GL_ERROR();

	if (!linkProgram(program))
	{
		deleteProgramAndShaders(program);
		return 0;
	}

	return program;
}

GLuint createProgram(const std::vector<GLuint>& shaderList)
{
	GLuint program = glCreateProgram();

	for (size_t iLoop = 0; iLoop < shaderList.size(); iLoop++)
		glAttachShader(program, shaderList[iLoop]);
	CHECK_GL_ERROR();

	if (!linkProgram(program))
	{
		deleteProgramAndShaders(program);
		return 0;
	}

	return program;
}

} // namespace GLUtils
