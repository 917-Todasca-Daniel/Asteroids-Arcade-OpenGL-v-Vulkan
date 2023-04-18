#include "GLShaders.h"

//	cpp includes
#include <iostream>

//	OpenGL related includes
#include <GL/glew.h>
#include <GLFW/glfw3.h>

//	dev includes
#include "util/UFile.h"

#include "GLTexture.h"

using namespace aa;



GLShader::GLShader() : shaderProgram(0)
{

}

GLShader::~GLShader() {
	glDeleteProgram(shaderProgram);
}


unsigned int GLShader::compileShader(
	unsigned int glType,
	const std::string& shader
)
{
	unsigned int id = glCreateShader(glType);

	const char* src = shader.c_str();
	glShaderSource(id, 1, &src, 0);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);

	if (result == GL_FALSE) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

		char* msg = new char[length];
		glGetShaderInfoLog(id, length, &length, msg);

		std::cout << msg << std::endl;

		delete[] msg;

		glDeleteShader(id);
		id = 0;
	}

	return id;
}

void GLShader::bind() const
{
	glUseProgram(shaderProgram);

	for (auto& uTex : uniformsTex) {
		uTex.tex->bindToSlot(uTex.slot);
	}

	for (auto& u4f : uniforms4f) {
		glUniform4f(
			glGetUniformLocation(shaderProgram, u4f.uniformKey.c_str()),
			u4f.value.x, u4f.value.y, u4f.value.z, u4f.value.w
		);
	}
}



IGLShaderBuilder::IGLShaderBuilder()
{

}



GLShaderFileBuilder::GLShaderFileBuilder(const std::string& shadersDirectory)
	: shadersDirectory(UFile::normalizedDirectory(shadersDirectory)), 
	  vertexFilepath(), fragmentFilepath(), shaderOutput(nullptr)
{

}

GLShaderFileBuilder::~GLShaderFileBuilder()
{
	if (shaderOutput) {
		delete shaderOutput;
	}
}


GLShaderFileBuilder& GLShaderFileBuilder::setVertexShader(const std::string& filename)
{
	std::string filepath = shadersDirectory;
	filepath += filename;
	filepath += ".glsl";
	this->vertexFilepath = filepath;
	return *this;
}

GLShaderFileBuilder& GLShaderFileBuilder::setFragmentShader(const std::string& filename)
{
	std::string filepath = shadersDirectory;
	filepath += filename;
	filepath += ".glsl";
	this->fragmentFilepath = filepath;
	return *this;
}

GLShaderFileBuilder& GLShaderFileBuilder::addUniformTex(
	const std::string&	uniformKey, 
	GLTexture*			tex,
	unsigned int		slot
) {
	if (!shaderOutput) {
		shaderOutput = new GLShader();
	}
	shaderOutput->uniformsTex.push_back({ uniformKey, tex, slot });
	return *this;
}

GLShaderFileBuilder& GLShaderFileBuilder::addUniform4f(
	const std::string&	uniformKey, 
	Vector4d 			value
) {
	if (!shaderOutput) {
		shaderOutput = new GLShader();
	}
	shaderOutput->uniforms4f.push_back({ uniformKey, value });
	return *this;
}


GLShader* GLShaderFileBuilder::build()
{
	if (vertexFilepath.empty()) {
		std::cout << "GLShaderFileBuilder::vertexFilepath is empty!\n";
		return nullptr;
	}
	if (fragmentFilepath.empty()) {
		std::cout << "GLShaderFileBuilder::vertexFilepath is empty!\n";
		return nullptr;
	}

	GLShader* output = shaderOutput;
	shaderOutput = nullptr;

	std::string vertexShader	= UFile::readFileContent(vertexFilepath);
	std::string fragmentShader	= UFile::readFileContent(fragmentFilepath);

	unsigned int program = glCreateProgram();
	unsigned int vs = GLShader::compileShader(GL_VERTEX_SHADER,   vertexShader);
	unsigned int fs = GLShader::compileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	output->shaderProgram = program;
	
	return output;
}
