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
		6 * sizeof(uint16_t),
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
		stagingBuffer, stagingBufferMemory
	);

	vkMapMemory(*VK_DEVICE, stagingBufferMemory, 0, 6 * sizeof(uint16_t), 0, &dataMapper);
	memcpy(dataMapper, indices2d, 6 * sizeof(uint16_t));
	vkUnmapMemory(*VK_DEVICE, stagingBufferMemory);

	VulkanRegistrar::createBuffer(
		6 * sizeof(uint16_t),
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, 
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
		indexBuffer, 
		indexBufferMemory
	);

	VulkanRegistrar::copyBuffer(stagingBuffer, indexBuffer, 6 * sizeof(uint16_t));

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

	vkWaitForFences	(*VK_DEVICE, 1, VK_FLIGHT_FENCE, VK_TRUE, UINT64_MAX);

	uint32_t imageIndex;
	VkResult ret = vkAcquireNextImageKHR(
		*VK_DEVICE, *VK_SWAPCHAIN, UINT64_MAX, 
		*VK_IMAGE_SEMAPHORE, VK_NULL_HANDLE, &imageIndex
	);
	if (ret == VK_ERROR_OUT_OF_DATE_KHR) {
		aa::VulkanRegistrar::recreateSwapChain();
		return;
	}

	vkResetFences(*VK_DEVICE, 1, VK_FLIGHT_FENCE);

	vkResetCommandBuffer(*VK_COMMAND_BUFFER, 0);
	VulkanRegistrar::recordCommandBuffer(
		*VK_COMMAND_BUFFER,
		pipeline,
		imageIndex,
		&vertexBuffer,
		&indexBuffer
	);

	pipeline->updateUniform(&uniformValue, sizeof(float));

	VkSubmitInfo			submitInfo			{ };
	VkSemaphore				waitSemaphores[]	= { *VK_IMAGE_SEMAPHORE };
	VkSemaphore				signalSemaphores[]	= { *VK_RENDER_SEMAPHORE };
	VkPipelineStageFlags	waitStages[]		= { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

	submitInfo.sType				= VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount	= 1;
	submitInfo.pWaitSemaphores		= waitSemaphores;
	submitInfo.pWaitDstStageMask	= waitStages;
	submitInfo.commandBufferCount	= 1;
	submitInfo.pCommandBuffers		= VK_COMMAND_BUFFER;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores	= signalSemaphores;

	if (vkQueueSubmit(*VK_GRAPHICS_QUEUE, 1, &submitInfo, *VK_FLIGHT_FENCE) != VK_SUCCESS) {
		std::cout << "Failed to submit draw command buffer!\n";
	}

	VkPresentInfoKHR	presentInfo{ };
	VkSwapchainKHR		swapChains [ ] = { *VK_SWAPCHAIN };

	presentInfo.sType				= VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount	= 1;
	presentInfo.pWaitSemaphores		= signalSemaphores;

	presentInfo.swapchainCount		= 1;
	presentInfo.pSwapchains			= swapChains;
	presentInfo.pImageIndices		= &imageIndex;	
	presentInfo.pResults			= nullptr; 

	ret = vkQueuePresentKHR(*VK_PRESENTATION_QUEUE, &presentInfo);

	if (ret == VK_ERROR_OUT_OF_DATE_KHR || ret == VK_SUBOPTIMAL_KHR) {
		VulkanRegistrar::recreateSwapChain();
	}
}
