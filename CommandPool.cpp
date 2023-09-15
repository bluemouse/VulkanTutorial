#include "CommandPool.h"

#include "Device.h"
#include "helpers_vulkan.h"

NAMESPACE_VULKAN_BEGIN

CommandPool::CommandPool(const Device& device, uint32_t queueFamilyIndex) {
  create(device, queueFamilyIndex);
}

CommandPool::~CommandPool() {
  if (_pool != VK_NULL_HANDLE) {
    destroy();
  }
}

CommandPool::CommandPool(CommandPool&& rhs) noexcept {
  moveFrom(rhs);
}

CommandPool& CommandPool::operator=(CommandPool&& rhs) noexcept(false) {
  if (this != &rhs) {
    moveFrom(rhs);
  }
  return *this;
}

void CommandPool::create(const Device& device, uint32_t queueFamilyIndex) {
  MI_VERIFY(_pool == VK_NULL_HANDLE);

  _device = &device;

  VkCommandPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  poolInfo.queueFamilyIndex = queueFamilyIndex;

  MI_VERIFY_VKCMD(vkCreateCommandPool(device, &poolInfo, nullptr, &_pool));

  vkGetDeviceQueue(device, queueFamilyIndex, 0, &_queue);
}

void CommandPool::destroy() {
  MI_VERIFY(_pool != VK_NULL_HANDLE);

  vkDestroyCommandPool(*_device, _pool, nullptr);
  _pool = VK_NULL_HANDLE;
  _queue = VK_NULL_HANDLE;
  _device = nullptr;
}

void CommandPool::moveFrom(CommandPool& rhs) {
  MI_VERIFY(_pool == VK_NULL_HANDLE);
  _pool = rhs._pool;
  _queue = rhs._queue;
  _device = rhs._device;

  rhs._pool = VK_NULL_HANDLE;
  rhs._queue = VK_NULL_HANDLE;
  rhs._device = nullptr;
}

NAMESPACE_VULKAN_END