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
	VulkanRegistrar::recordCommandBuffer(*VK_COMMAND_BUFFER, pipeline->vkPipeline, imageIndex);

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
		aa::VulkanRegistrar::recreateSwapChain();
	}
}
