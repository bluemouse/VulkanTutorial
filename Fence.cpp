#include "Fence.h"

#include "Device.h"

NAMESPACE_VULKAN_BEGIN

Fence::Fence(const Device& device) {
  create(device);
}

Fence::~Fence() {
  if (isCreated()) {
    destroy();
  }
}

Fence::Fence(Fence&& rhs) noexcept {
  moveFrom(rhs);
}

Fence& Fence::operator=(Fence&& rhs) noexcept(false) {
  if (this != &rhs) {
    moveFrom(rhs);
  }
  return *this;
}

void Fence::moveFrom(Fence& rhs) {
  MI_VERIFY(!isCreated());
  _fence = rhs._fence;
  _device = rhs._device;

  rhs._fence = VK_NULL_HANDLE;
  rhs._device = nullptr;
}

void Fence::create(const Device& device) {
  MI_VERIFY(!isCreated());
  _device = &device;

  VkFenceCreateInfo fenceInfo{};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  MI_VERIFY_VKCMD(vkCreateFence(device, &fenceInfo, nullptr, &_fence));
}

void Fence::destroy() {
  MI_VERIFY(isCreated());
  vkDestroyFence(*_device, _fence, nullptr);

  _device = nullptr;
  _fence = VK_NULL_HANDLE;
}

NAMESPACE_VULKAN_END
