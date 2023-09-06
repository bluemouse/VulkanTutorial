#include "Fence.h"
#include "Device.h"

#include "helpers.h"

using namespace Vulkan;

Fence::Fence(const Device& device) {
  create(device);
}

Fence::~Fence() {
  if (_fence != VK_NULL_HANDLE) {
    destroy();
  }
}

void Fence::create(const Device& device) {
  if (_fence != VK_NULL_HANDLE) {
    throw std::runtime_error("Vulkan fence has been initialized already!");
  }
  _device = &device;

  VkFenceCreateInfo fenceInfo{};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  VK_VERIFY(vkCreateFence(device, &fenceInfo, nullptr, &_fence));
}

void Fence::destroy() {
  if (_fence == VK_NULL_HANDLE) {
    throw std::runtime_error("Vulkan null fence cannot be released!");
  }
  vkDestroyFence(*_device, _fence, nullptr);

  _device = nullptr;
  _fence = VK_NULL_HANDLE;
}
