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

	int isCompiled;
	glGetShaderiv(id, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

		char* msg = new char[length + 1];
		glGetShaderInfoLog(id, length, &length, msg);
		msg[length] = '\0';
		std::cout << msg << std::endl;

		delete[] msg;

		glDeleteShader(id);
		id = 0;
	}

	return id;
}

void GLShader::bind()
{
	glUseProgram(shaderProgram);

	for (auto& i1ref : uniforms1iRef) {
		glUniform1f(
			glGetUniformLocation(shaderProgram, i1ref.uniformKey.c_str()),
			*(float*)i1ref.value
		);
	}

	for (auto& uTex : uniformsTex) {
		uTex.value->bindToSlot(0);
		glUniform1i(
			glGetUniformLocation(shaderProgram, uTex.uniformKey.c_str()),
			0
		);
	}

	for (auto& u4f : uniforms4f) {
		glUniform4f(
			glGetUniformLocation(shaderProgram, u4f.uniformKey.c_str()),
			u4f.value.x, u4f.value.y, u4f.value.z, u4f.value.w
		);
	}

	for (auto& u4f : uniforms3f) {
		glUniform3f(
			glGetUniformLocation(shaderProgram, u4f.uniformKey.c_str()),
			u4f.value.x, u4f.value.y, u4f.value.z
		);
	}

	for (auto& mat4f : uniformsMat4f) {
		glUniformMatrix4fv(
			glGetUniformLocation(shaderProgram, mat4f.uniformKey.c_str()),
			1, 
			GL_TRUE,
			mat4f.value.data()
		);
	}
}


template <typename wrapperT>
void GLShader::updateOrPutIfExists(
	std::vector <wrapperT>& container,
	const wrapperT& lookUp
) {
	for (auto& obj : container) {
		if (obj.uniformKey == lookUp.uniformKey) {
			obj.value = lookUp.value;
			return;
		}
	}
	container.push_back(lookUp);
}

void GLShader::addUniformTex(
	const std::string& uniformKey,
	GLTexture*		   tex
) {
	this->updateOrPutIfExists <UniformTex>(
		this->uniformsTex,
		{ uniformKey, tex }
	);
}

void GLShader::addUniform3f(
	const std::string& uniformKey,
	Vector3d           value
) {
	this->updateOrPutIfExists <Uniform3f>(
		this->uniforms3f,
		{ uniformKey, value }
	);
}

void GLShader::addUniform4f(
	const std::string& uniformKey,
	Vector4d           value
) {
	this->updateOrPutIfExists <Uniform4f>(
		this->uniforms4f,
		{ uniformKey, value }
	);
}

void GLShader::addUniformMat4f(
	const std::string& uniformKey,
	Matrix4d           value
) {
	this->updateOrPutIfExists <UniformMat4f>(
		this->uniformsMat4f,
		{ uniformKey, value }
	);
}

void GLShader::addUniform1iRef(
	const std::string& uniformKey,
	const void*		   value
) {
	this->updateOrPutIfExists <Uniform1iRef>(
		this->uniforms1iRef,
		{ uniformKey, value }
	);
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
	GLTexture*			tex
) {
	if (!shaderOutput) {
		shaderOutput = new GLShader();
	}
	shaderOutput->addUniformTex(uniformKey, tex);
	return *this;
}

GLShaderFileBuilder& GLShaderFileBuilder::addUniform4f(
	const std::string&	uniformKey, 
	Vector4d 			value
) {
	if (!shaderOutput) {
		shaderOutput = new GLShader();
	}
	shaderOutput->addUniform4f(uniformKey, value);
	return *this;
}

GLShaderFileBuilder& GLShaderFileBuilder::addUniform3f(
	const std::string& uniformKey,
	Vector3d 		   value
) {
	if (!shaderOutput) {
		shaderOutput = new GLShader();
	}
	shaderOutput->addUniform3f(uniformKey, value);
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

	if (!shaderOutput) {
		shaderOutput = new GLShader();
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
