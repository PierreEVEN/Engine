#include "Vulkan/VulkanCommandBuffer.h"
#include "IO/Log.h"
#include "Vulkan/VulkanFramebuffser.h"
#include "Vulkan/VulkanCommandPool.h"
#include "Vulkan/VulkanLogicalDevice.h"
#include "Vulkan/VulkanRenderPass.h"
#include "Vulkan/VulkanSwapChain.h"
#include "Vulkan/VulkanGraphicPipeline.h"
#include "Vulkan/VulkanVertexBuffer.h"
#include "Vulkan/VulkanUniformBuffer.h"
#include "Vulkan/VulkanMesh.h"

std::vector<VkCommandBuffer> commandBuffers;

void Rendering::Vulkan::CommandBuffer::CreateCommandBuffers()
{
	LOG("Create command buffers");

	commandBuffers.resize(Framebuffer::GetSwapChainFramebuffers().size());

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = CommandPool::GetCommandPool();
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

	if (vkAllocateCommandBuffers(LogDevice::GetLogicalDevice(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
		LOG_ASSERT("Failed to allocate command buffer");
	}

	for (size_t i = 0; i < commandBuffers.size(); i++) {
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0; // Optional
		beginInfo.pInheritanceInfo = nullptr; // Optional

		if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
			LOG_ASSERT(String("Failed to create command buffer #") + String::ToString(i));
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = RenderPass::GetRenderPass();
		renderPassInfo.framebuffer = Framebuffer::GetSwapChainFramebuffers()[i];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = SwapChain::GetSwapchainExtend();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.5f, 0.2f, 0.1f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

			vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, GraphicPipeline::GetGraphicPipeline());

			VkBuffer vertexBuffers[] = { VertexBuffer::GetVertexBuffer() };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);
			vkCmdBindIndexBuffer(commandBuffers[i], VertexBuffer::GetIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);

			vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, GraphicPipeline::GetPipelineLayout(), 0, 1, &UniformBuffer::GetDescriptorSets()[i] , 0, nullptr);
			vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>(Mesh::GetIndexCount()), 1, 0, 0, 0);

		vkCmdEndRenderPass(commandBuffers[i]);

		if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
			LOG_ASSERT(String("Failed to register command buffer #") + String::ToString(i));
		}
	}
}

std::vector<VkCommandBuffer>& Rendering::Vulkan::CommandBuffer::GetCommandBuffers()
{
	return commandBuffers;
}

void Rendering::Vulkan::CommandBuffer::FreeCommandBuffers()
{
	vkFreeCommandBuffers(LogDevice::GetLogicalDevice(), CommandPool::GetCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
}