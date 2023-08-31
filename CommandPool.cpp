#include "CommandPool.h"
#include "Device.h"

#include "helpers.h"

using namespace Vulkan;

CommandPool::CommandPool(const Device& device, uint32_t queueFamilyIndex) {
  init(device, queueFamilyIndex);
}

CommandPool::~CommandPool() {
  if (_pool != VK_NULL_HANDLE) {
    release();
  }
}

void CommandPool::init(const Device& device, uint32_t queueFamilyIndex) {
  if (_pool != VK_NULL_HANDLE) {
    throw std::runtime_error("Vulkan command pool has been initialized already!");
  }

  _device = &device;

  VkCommandPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  poolInfo.queueFamilyIndex = queueFamilyIndex;

  VK_VERIFY(vkCreateCommandPool(device, &poolInfo, nullptr, &_pool));
}

void CommandPool::release() {
  if (_pool == VK_NULL_HANDLE) {
    throw std::runtime_error("Vulkan null command pool cannot be released!");
  }

  vkDestroyCommandPool(*_device, _pool, nullptr);
  _pool = VK_NULL_HANDLE;
  _device = nullptr;
}

