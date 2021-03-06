#include "Ressources/GraphicRessource.h"
#include "Utils.h"

void Rendering::Ressource::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(G_LOGICAL_DEVICE, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
		LOG_ASSERT("Failed to create buffer");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(G_LOGICAL_DEVICE, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(G_LOGICAL_DEVICE, &allocInfo, G_ALLOCATION_CALLBACK, &bufferMemory) != VK_SUCCESS) {
		LOG_ASSERT("Failled to allocate buffer memory");
	}

	vkBindBufferMemory(G_LOGICAL_DEVICE, buffer, bufferMemory, 0);
}

void Rendering::Ressource::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

	VkBufferCopy copyRegion{};
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	EndSingleTimeCommands(commandBuffer);
}

Rendering::Ressource::Ressource() : bShouldDestroy(false) 
{
	ressourceAccessorMutex.lock();
	ressources.push_back(this);
	ressourceAccessorMutex.unlock();
}

Rendering::Ressource::~Ressource() {}

void Rendering::Ressource::FlushRessources()
{
	ressourceAccessorMutex.lock();
	for (int64_t i = ressources.size() - 1; i >= 0; --i) {
		if (ressources[i]->bShouldDestroy) {
			delete ressources[i];
			ressources.erase(ressources.begin() + i);
		}
	}
	ressourceAccessorMutex.unlock();
}

void Rendering::Ressource::FreeRessources()
{
	ressourceAccessorMutex.lock();
	for (int64_t i = ressources.size() - 1; i >= 0; --i) {
		delete ressources[i];
	}
	ressources.clear();
	ressourceAccessorMutex.unlock();
}

void Rendering::Ressource::UpdateAllRessources()
{
	LOG("Update all ressources");
	vkDeviceWaitIdle(G_LOGICAL_DEVICE);
	for (auto& ressource : ressources) {
		ressource->CreateOrUpdateRessource();
	}
	LOG("Updated all ressources");
}
