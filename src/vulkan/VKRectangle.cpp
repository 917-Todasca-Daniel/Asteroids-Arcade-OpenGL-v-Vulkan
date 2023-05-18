#include "VKRectangle.h"

#include <iostream>

#include "VKPipeline.h"
#include "VKShader.h"

//	dev includes
#include "util/UMaths.h"

#include "constants/window_constants.hpp"

using namespace aa;



VKRectangle::VKRectangle(
	LogicObject* parent,
	Vector3d     position,
	float        height,
	float        width,
	VKPipeline*  pipeline
) :  Object3d(parent, position), 
	height				(height), 
	width				(width), 
	pipeline			(pipeline),
	vertexBuffer		(VkBuffer{}), 
	vertexBufferMemory	(VkDeviceMemory{}),
	indexBuffer			(VkBuffer{}), 
	indexBufferMemory	(VkDeviceMemory{}),
	uniformValue		(0.0f)
{

}

VKRectangle::~VKRectangle()
{
	vkDestroyBuffer	(*VK_DEVICE, vertexBuffer,		 nullptr);
	vkFreeMemory	(*VK_DEVICE, vertexBufferMemory, nullptr);

	vkDestroyBuffer	(*VK_DEVICE, indexBuffer,		 nullptr);
	vkFreeMemory	(*VK_DEVICE, indexBufferMemory,  nullptr);
}


void VKRectangle::init()
{
	Object3d::init();

	{	// optimise by using CPU instead of a shader for calculating coords
		vertices2d[0] = 1.0f * (position.x - width / 2) / WINDOW_UNIT;
		vertices2d[1] = 1.0f * (position.y - height / 2) / WINDOW_UNIT;

		aa::UMaths::worldRectangleBottomLeft(
			height, width,
			vertices2d[0], vertices2d[1],
			vertices2d[2], vertices2d[3],
			vertices2d[4], vertices2d[5],
			vertices2d[6], vertices2d[7]
		);

		aa::UMaths::worldToGLCoord(vertices2d[0], vertices2d[1]);
		aa::UMaths::worldToGLCoord(vertices2d[2], vertices2d[3]);
		aa::UMaths::worldToGLCoord(vertices2d[4], vertices2d[5]);
	}

	createVertexBuffer();
	createIndexBuffer ();
}

void VKRectangle::createVertexBuffer() {
	VkBuffer		stagingBuffer;
	VkDeviceMemory	stagingBufferMemory;
	void*			dataMapper;

	VulkanRegistrar::createBuffer(
		sizeof(float) * 8,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer,
		stagingBufferMemory
	);

	vkMapMemory(*VK_DEVICE, stagingBufferMemory, 0, sizeof(float) * 8, 0, &dataMapper);
	memcpy(dataMapper, &vertices2d, sizeof(float) * 8);
	vkUnmapMemory(*VK_DEVICE, stagingBufferMemory);

	VulkanRegistrar::createBuffer(
		sizeof(float) * 8,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		vertexBuffer,
		vertexBufferMemory
	);

	VulkanRegistrar::copyBuffer(stagingBuffer, vertexBuffer, sizeof(float) * 8);

	vkDestroyBuffer(*VK_DEVICE, stagingBuffer, nullptr);
	vkFreeMemory(*VK_DEVICE, stagingBufferMemory, nullptr);

}

void VKRectangle::createIndexBuffer() {
	VkBuffer		stagingBuffer;
	VkDeviceMemory	stagingBufferMemory;
	void*			dataMapper;

	VulkanRegistrar::createBuffer(
		6 * sizeof(uint32_t),
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
		stagingBuffer, stagingBufferMemory
	);

	vkMapMemory(*VK_DEVICE, stagingBufferMemory, 0, 6 * sizeof(uint32_t), 0, &dataMapper);
	memcpy(dataMapper, indices2d, 6 * sizeof(uint32_t));
	vkUnmapMemory(*VK_DEVICE, stagingBufferMemory);

	VulkanRegistrar::createBuffer(
		6 * sizeof(uint32_t),
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, 
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
		indexBuffer, 
		indexBufferMemory
	);

	VulkanRegistrar::copyBuffer(stagingBuffer, indexBuffer, 6 * sizeof(uint32_t));

	vkDestroyBuffer(*VK_DEVICE, stagingBuffer, nullptr);
	vkFreeMemory(*VK_DEVICE, stagingBufferMemory, nullptr);
}


void VKRectangle::loop(float lap) {
	Object3d::loop(lap);

	uniformValue += lap;
}


void VKRectangle::draw()
{
	Object3d::draw();

	if (rmPending) return;

	VulkanRegistrar::recordCommandBuffer(
		*VK_COMMAND_BUFFER,
		pipeline,
		VK_CURRENT_IMAGE,
		&vertexBuffer,
		&indexBuffer,
		6
	);

	pipeline->updateUniform(&uniformValue, sizeof(float));
}
