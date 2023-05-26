#include "SkyRectangle.h"

#include "constants/window_constants.hpp"

#include "GLShaders.h"
#include "GLRectangle.h"

#include "domain/RootObject.h"
#include "domain/GameFactory.h"

using namespace aa;



SkyRectangle::SkyRectangle(LogicObject*	parent) : LogicObject(parent),
	height(WINDOW_HEIGHT), width(WINDOW_WIDTH), lifespan(0),
	sky(0)
{

}

SkyRectangle::~SkyRectangle()
{
	if (sky) {
		delete sky;
	}
}


void SkyRectangle::kill()
{
	if (sky) {
		sky->kill();
	}
}

void SkyRectangle::init()
{
	sky = FACTORY->buildSky(height, width, &this->lifespan);
	sky->init();

}


void SkyRectangle::loop(float lap)
{
	LogicObject::loop(lap);

	lifespan += lap;
	sky->loop(lap);

}


void SkyRectangle::draw()
{
	sky->draw();

}
