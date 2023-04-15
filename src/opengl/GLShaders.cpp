#include "GLShaders.h"

//	cpp includes
#include <iostream>

//	OpenGL related includes
#include <GL/glew.h>
#include <GLFW/glfw3.h>

//	dev includes
#include "util/UFile.hpp"

using namespace aa;


unsigned int GLShaders::compileShader(ShaderType type, const std::string& shader)
{
	unsigned int glType = 0;
	if (type == GLSVertex) {
		glType = GL_VERTEX_SHADER;
	}
	else if (type == GLSFragment) {
		glType = GL_FRAGMENT_SHADER;
	}

	unsigned int id = glCreateShader(glType);

	const char* src = shader.c_str();
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	
	if (result == GL_FALSE) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

		char* msg = new char[length];
		glGetShaderInfoLog(id, length, &length, msg);

		std::cout << msg << std::endl;

		delete msg;

		glDeleteShader(id);
		return 0;
	}

	return id;
}


unsigned int GLShaders::createShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	unsigned int program = glCreateProgram();
	unsigned int vs = compileShader(GLSVertex, vertexShader);
	unsigned int fs = compileShader(GLSFragment, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}


unsigned int GLShaders::createShaderFromFiles(const std::string& vertexName, const std::string& fragmentName)
{
	std::string vertexPath = U_GLShaderPath(vertexName);
	std::string fragmentPath = U_GLShaderPath(fragmentName);

	std::string vertexShader = U_readFileContent(vertexPath);
	std::string fragmentShader = U_readFileContent(fragmentPath);

	return createShader(vertexShader, fragmentShader);
}
