#include "CommandBuffer.h"
#include "Device.h"
#include "CommandPool.h"
#include "Framebuffer.h"
#include "Pipeline.h"
#include "RenderPass.h"

#include "helpers.h"

using namespace Vulkan;

CommandBuffer::CommandBuffer(const CommandPool& commandPool) {
  allocate(commandPool);
}

CommandBuffer::~CommandBuffer() {
  if (_buffer != VK_NULL_HANDLE) {
    free();
  }
}

void CommandBuffer::allocate(const CommandPool& commandPool) {
  if (_buffer != VK_NULL_HANDLE) {
    throw std::runtime_error("Vulkan command buffer has been initialized already!");
  }
  _pool = &commandPool;

  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = commandPool;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = 1;

  VK_VERIFY(vkAllocateCommandBuffers(commandPool.device(), &allocInfo, &_buffer));
}

void CommandBuffer::reset() {
  vkResetCommandBuffer(_buffer, /*VkCommandBufferResetFlagBits*/ 0);
}

void CommandBuffer::free() {
  if (_buffer == VK_NULL_HANDLE) {
    throw std::runtime_error("Vulkan null command buffer cannot be freed!");
  }

  vkFreeCommandBuffers(_pool->device(), *_pool, 1, &_buffer);

  _buffer = VK_NULL_HANDLE;
  _pool = nullptr;
}

void CommandBuffer::recordCommand(const std::function<void(const CommandBuffer& buffer)>& command) const {
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  VK_VERIFY(vkBeginCommandBuffer(_buffer, &beginInfo));

  command(*this);

  VK_VERIFY(vkEndCommandBuffer(_buffer));
}

void CommandBuffer::executeCommand(const std::function<void(const CommandBuffer& buffer)>& command,
                                   std::vector<VkSemaphore> waits, std::vector<VkSemaphore> signals,
                                   VkFence fence) const {
  recordCommand(command);

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &_buffer;

  VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount = static_cast<uint32_t>(waits.size());
  submitInfo.pWaitSemaphores = waits.data();
  submitInfo.pWaitDstStageMask = waitStages;

  submitInfo.signalSemaphoreCount = static_cast<uint32_t>(signals.size());
  submitInfo.pSignalSemaphores = signals.data();

  auto queue = _pool->queue();
  VK_VERIFY(vkQueueSubmit(queue, 1, &submitInfo, fence));
}

void CommandBuffer::recordSingleTimeCommand(const std::function<void(const CommandBuffer&)>& command) const {
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  VK_VERIFY(vkBeginCommandBuffer(_buffer, &beginInfo));

  command(*this);

  VK_VERIFY(vkEndCommandBuffer(_buffer));
}

void CommandBuffer::executeSingleTimeCommand(const std::function<void(const CommandBuffer&)>& command, bool blocking) const {
  recordSingleTimeCommand(command);

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &_buffer;

  auto queue = _pool->queue();
  vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
  if (blocking) {
    vkQueueWaitIdle(queue);
  }
}
