#include "CommandBuffer.h"
#include "Device.h"
#include "CommandPool.h"
#include "Framebuffer.h"
#include "Pipeline.h"
#include "RenderPass.h"

#include "helpers.h"

using namespace Vulkan;

void CommandBuffer::allocate(const Device& device, const CommandPool& commandPool) {
  if (_buffer != VK_NULL_HANDLE) {
    throw std::runtime_error("Vulkan command pool has been initialized already!");
  }

  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = commandPool;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = 1;

  VK_VERIFY(vkAllocateCommandBuffers(device, &allocInfo, &_buffer));
}

void CommandBuffer::reset() {
  vkResetCommandBuffer(_buffer, /*VkCommandBufferResetFlagBits*/ 0);
}

void CommandBuffer::record(const Framebuffer& framebuffer, const Pipeline& pipeline, const RenderPass& renderPass,
                           VkBuffer vertexBuffer, VkBuffer indexBuffer, uint32_t numIndices,
                           VkDescriptorSet descriptorSet) {
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  VK_VERIFY(vkBeginCommandBuffer(_buffer, &beginInfo));

  auto extent = framebuffer.extent();

  VkRenderPassBeginInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = renderPass;
  renderPassInfo.framebuffer = framebuffer;
  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent = extent;

  VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
  renderPassInfo.clearValueCount = 1;
  renderPassInfo.pClearValues = &clearColor;

  vkCmdBeginRenderPass(_buffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
  vkCmdBindPipeline(_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = (float)extent.width;
  viewport.height = (float)extent.height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  vkCmdSetViewport(_buffer, 0, 1, &viewport);

  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = extent;
  vkCmdSetScissor(_buffer, 0, 1, &scissor);

  VkBuffer vertexBuffers[] = {vertexBuffer};
  VkDeviceSize offsets[] = {0};
  vkCmdBindVertexBuffers(_buffer, 0, 1, vertexBuffers, offsets);
  vkCmdBindIndexBuffer(_buffer, indexBuffer, 0, VK_INDEX_TYPE_UINT16);
  vkCmdBindDescriptorSets(_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          pipeline.layout(), 0, 1, &descriptorSet,
                          0, nullptr);

  vkCmdDrawIndexed(_buffer, numIndices, 1, 0, 0, 0);

  vkCmdEndRenderPass(_buffer);

  VK_VERIFY(vkEndCommandBuffer(_buffer));
}
