#include "VKTriangle.h"

#include <iostream>

#include "VulkanRegistrar.h"

#include "VKPipeline.h"

//	dev includes
#include "util/UColors.h"
#include "util/UMaths.h"

#include "constants/window_constants.hpp"

using namespace aa;



VKTriangle::VKTriangle(
	LogicObject*	parent, 
	VKPipeline*		pipeline
)
	: Object3d(parent, position), pipeline(pipeline)
{

}

VKTriangle::~VKTriangle()
{
	
}


void VKTriangle::init()
{
	Object3d::init();

}


void VKTriangle::draw()
{
	Object3d::draw();

	if (rmPending) return;

	VulkanRegistrar::recordCommandBuffer(*VK_COMMAND_BUFFER, pipeline, VK_CURRENT_IMAGE);
}
