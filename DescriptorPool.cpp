#include "DescriptorPool.h"

#include <utility>

#include "Device.h"
#include "helpers_vulkan.h"

NAMESPACE_VULKAN_BEGIN

DescriptorPool::DescriptorPool(const Device& device,
                               std::vector<VkDescriptorPoolSize> poolSizes,
                               uint32_t maxSets) {
  create(device, std::move(poolSizes), maxSets);
}

DescriptorPool::~DescriptorPool() {
  if (_pool != VK_NULL_HANDLE) {
    destroy();
  }
}

DescriptorPool::DescriptorPool(DescriptorPool&& rhs) noexcept {
  moveFrom(rhs);
}

DescriptorPool& DescriptorPool::operator=(DescriptorPool&& rhs) noexcept(false) {
  if (this != &rhs) {
    moveFrom(rhs);
  }
  return *this;
}

void DescriptorPool::create(const Device& device,
                            std::vector<VkDescriptorPoolSize> poolSizes,
                            uint32_t maxSets) {
  MI_VERIFY(_pool == VK_NULL_HANDLE);
  _device = &device;

  VkDescriptorPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
  poolInfo.pPoolSizes = poolSizes.data();
  poolInfo.maxSets = maxSets;

  MI_VERIFY_VKCMD(vkCreateDescriptorPool(device, &poolInfo, nullptr, &_pool));
}

void DescriptorPool::destroy() {
  MI_VERIFY(_pool != VK_NULL_HANDLE);

  vkDestroyDescriptorPool(*_device, _pool, nullptr);
  _pool = VK_NULL_HANDLE;
  _device = nullptr;
}

void DescriptorPool::moveFrom(DescriptorPool& rhs) {
  MI_VERIFY(_pool == VK_NULL_HANDLE);
  _pool = rhs._pool;
  _device = rhs._device;

  rhs._pool = VK_NULL_HANDLE;
  rhs._device = nullptr;
}

NAMESPACE_VULKAN_END