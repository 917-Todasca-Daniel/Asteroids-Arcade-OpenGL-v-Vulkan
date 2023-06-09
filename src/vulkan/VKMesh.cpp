#include "VKMesh.h"

#include <iostream>

//  assimp
#include <assimp/postprocess.h>

#include "VulkanRegistrar.h"
#include "VKPipeline.h"

//	dev includes
#include "util/UMaths.h"
#include "util/UFile.h"

#include "data/Matrix4d.hpp"

#include "constants/window_constants.hpp"

using namespace aa;


#define ASSIMP_LOAD_FLAGS ( \
	aiProcess_Triangulate		| aiProcess_FlipUVs |				\
	aiProcess_GenSmoothNormals	| aiProcess_JoinIdenticalVertices	\
)



VKMesh::VKMesh(LogicObject* parent, Vector3d position, VKPipeline* pipeline) :  
	Mesh				(parent, position), 
	pipeline			(pipeline),
	vertexBuffer		(VkBuffer{}), 
	vertexBufferMemory	(VkDeviceMemory{}),
	indexBuffer			(VkBuffer{}), 
	indexBufferMemory	(VkDeviceMemory{}),
	lifespan			(0.0)
{

}

VKMesh::~VKMesh()
{
	vkDestroyBuffer	(*VK_DEVICE, vertexBuffer,		 nullptr);
	vkFreeMemory	(*VK_DEVICE, vertexBufferMemory, nullptr);

	vkDestroyBuffer	(*VK_DEVICE, indexBuffer,		 nullptr);
	vkFreeMemory	(*VK_DEVICE, indexBufferMemory,  nullptr);

	// delete pipeline in a module above
}


void VKMesh::loadFromFbx(const char* filepath, float scale) {
	Mesh::loadFromFbx(filepath, ASSIMP_LOAD_FLAGS, scale);

	lifespan = 0;
}


void VKMesh::init()
{
	Object3d::init();

	if (vertices.empty()) return;

	createVertexBuffer();
	createIndexBuffer();
}

void VKMesh::createVertexBuffer() {
	VkBuffer		stagingBuffer;
	VkDeviceMemory	stagingBufferMemory;
	void*			dataMapper;

	VkDeviceSize bufferSize = sizeof(float) * vertices.size();

	VulkanRegistrar::createBuffer(
		bufferSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer,
		stagingBufferMemory
	);

	vkMapMemory(*VK_DEVICE, stagingBufferMemory, 0, bufferSize, 0, &dataMapper);
	memcpy(dataMapper, vertices.data(), bufferSize);
	vkUnmapMemory(*VK_DEVICE, stagingBufferMemory);

	VulkanRegistrar::createBuffer(
		bufferSize,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		vertexBuffer,
		vertexBufferMemory
	);

	VulkanRegistrar::copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

	vkDestroyBuffer(*VK_DEVICE, stagingBuffer, nullptr);
	vkFreeMemory(*VK_DEVICE, stagingBufferMemory, nullptr);

}

void VKMesh::createIndexBuffer() {
	VkBuffer		stagingBuffer;
	VkDeviceMemory	stagingBufferMemory;
	void*			dataMapper;

	VkDeviceSize bufferSize = sizeof(uint32_t) * indices.size();

	VulkanRegistrar::createBuffer(
		bufferSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
		stagingBuffer, stagingBufferMemory
	);

	vkMapMemory(*VK_DEVICE, stagingBufferMemory, 0, bufferSize, 0, &dataMapper);
	memcpy(dataMapper, indices.data(), bufferSize);
	vkUnmapMemory(*VK_DEVICE, stagingBufferMemory);

	VulkanRegistrar::createBuffer(
		bufferSize,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, 
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
		indexBuffer, 
		indexBufferMemory
	);

	VulkanRegistrar::copyBuffer(stagingBuffer, indexBuffer, bufferSize);

	vkDestroyBuffer(*VK_DEVICE, stagingBuffer, nullptr);
	vkFreeMemory(*VK_DEVICE, stagingBufferMemory, nullptr);
}


void VKMesh::loop(float lap) {
	Mesh::loop(lap);

	lifespan += lap;
}


void VKMesh::draw()
{
	Object3d::draw();

	if (rmPending) return;

	VulkanRegistrar::recordCommandBuffer(
		*VK_COMMAND_BUFFER,
		pipeline,
		VK_CURRENT_IMAGE,
		&vertexBuffer,
		&indexBuffer,
		(uint32_t) indices.size()
	);


	{	// translation, scaling and rotation geometry
		Matrix4d projection = Matrix4d::ViewportMatrix();
		projection *= Matrix4d::TranslationMatrix(position);

		projection *= Matrix4d::RotateAround(rotation, center);

		for (int i = 0, j; i < 4; i++) {
			for (j = 0; j < i; j++) {
				std::swap(projection.data()[4 * i + j], projection.data()[4 * j + i]);
			}
		}

		pipeline->updateUniform(projection.data(), 16 * sizeof(float));
	}

}



VKInstancedMesh::VKInstancedMesh(
	LogicObject* parent,
	VKPipeline*  pipeline,
	uint32_t     noInstances
) : VKMesh(parent, Vector3d(0, 0, 0), pipeline), 
	noInstances(noInstances), projectionData(16 * noInstances),
	instanceBuffer(0), instanceBufferMemory(0),
	instanceStagingBuffer(0), instanceStagingBufferMemory(0), instanceDataMapper(0)
{

}

VKInstancedMesh::~VKInstancedMesh()
{
	vkDestroyBuffer	(*VK_DEVICE, instanceBuffer,		nullptr);
	vkFreeMemory	(*VK_DEVICE, instanceBufferMemory,	nullptr);

	vkUnmapMemory	(*VK_DEVICE, instanceStagingBufferMemory);
	vkDestroyBuffer	(*VK_DEVICE, instanceStagingBuffer, nullptr);
	vkFreeMemory	(*VK_DEVICE, instanceStagingBufferMemory, nullptr);

}


void VKInstancedMesh::init()
{
	VKMesh::init();
	
	VkDeviceSize bufferSize = sizeof(float) * projectionData.size();

	VulkanRegistrar::createBuffer(
		bufferSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		instanceStagingBuffer,
		instanceStagingBufferMemory
	);

	vkMapMemory(*VK_DEVICE, instanceStagingBufferMemory, 0, bufferSize, 0, &instanceDataMapper);

	VulkanRegistrar::createBuffer(
		bufferSize,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		instanceBuffer,
		instanceBufferMemory
	);

}

void VKInstancedMesh::draw()
{
	Object3d::draw();

	if (rmPending) return;

	VkDeviceSize bufferSize = sizeof(float) * projectionData.size();

	memcpy(instanceDataMapper, projectionData.data(), bufferSize);
	VulkanRegistrar::copyBuffer(instanceStagingBuffer, instanceBuffer, bufferSize);

	VulkanRegistrar::recordCommandBuffer(
		*VK_COMMAND_BUFFER,
		pipeline,
		VK_CURRENT_IMAGE,
		&vertexBuffer,
		&indexBuffer,
		(uint32_t)indices.size(),
		noInstances,
		&instanceBuffer
	);

}


VKMeshInstance::VKMeshInstance(
	VKInstancedMesh* parent,
	Vector3d         position,
	uint32_t         instanceIndex,
	float			 scale
) : Mesh(parent, position), instanceIndex(instanceIndex), scale(scale)
{

}

VKMeshInstance::~VKMeshInstance()
{

}


void aa::VKMeshInstance::draw()
{
	Object3d::draw();

	if (rmPending) return;

	{	// translation, scaling and rotation geometry
		VKInstancedMesh* instancedMesh = dynamic_cast<VKInstancedMesh*> (parent);

		Matrix4d projection = Matrix4d::ViewportMatrix();
		projection *= Matrix4d::TranslationMatrix(position);
		projection *= Matrix4d::ScalingMatrix(scale);
		projection *= Matrix4d::RotateAround(rotation, center);

		for (int i = 0, j; i < 4; i++) {
			for (j = 0; j < 4; j++) {
				instancedMesh->projectionData[instanceIndex * 16 + 4*i + j]
					= projection.data()[4*j + i];
			}
		}
	}

}
