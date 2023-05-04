#include "SkyRectangle.h"

#include "constants/window_constants.hpp"

#include "GLShaders.h"
#include "GLRectangle.h"

#include "domain/RootObject.h"

using namespace aa;



SkyRectangle::SkyRectangle(LogicObject*	parent) : LogicObject(parent),
	height(WINDOW_HEIGHT), width(WINDOW_WIDTH), lifespan(0),
	shader(0), rectangle(0)
{

}

SkyRectangle::~SkyRectangle()
{
	if (rectangle) {
		delete rectangle;
	}
	if (shader) {
		delete shader;
	}

}


void SkyRectangle::kill()
{
	if (rectangle) {
		rectangle->kill();
	}
}


void SkyRectangle::init()
{
	shader = GLShaderFileBuilder("D:/licenta/dev/app/res/opengl/shaders")
		.setVertexShader("v_basic_tex_position")
		.setFragmentShader("f_perlin_sky")
		.build();

	rectangle = new GLRectangle(
		this,
		Vector3d(width * 0.5f, height * 0.5f, 0),
		height, width,
		shader
	);

	rectangle->init();

	shader->addUniform1iRef("u_Time", &this->lifespan);

}


void SkyRectangle::loop(float lap)
{
	LogicObject::loop(lap);

	lifespan += lap;

	rectangle->loop(lap);

}


void SkyRectangle::draw()
{
	rectangle->draw();

}
