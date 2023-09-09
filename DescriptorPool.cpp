#include "DescriptorPool.h"
#include "Device.h"

#include "helpers_vkdebug.h"

using namespace Vulkan;

DescriptorPool::DescriptorPool(const Device& device,
                               std::vector<VkDescriptorPoolSize> poolSizes, uint32_t maxSets) {
  create(device, poolSizes, maxSets);
}

DescriptorPool::~DescriptorPool() {
  if (_pool != VK_NULL_HANDLE) {
    destroy();
  }
}

void DescriptorPool::create(const Device& device,
                            std::vector<VkDescriptorPoolSize> poolSizes, uint32_t maxSets) {
  if (_pool != VK_NULL_HANDLE) {
    throw std::runtime_error("Vulkan descriptor pool has been initialized already!");
  }
  _device = &device;

  VkDescriptorPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
  poolInfo.pPoolSizes = poolSizes.data();
  poolInfo.maxSets = maxSets;

  MG_VERIFY_VKCMD(vkCreateDescriptorPool(device, &poolInfo, nullptr, &_pool));
}

void DescriptorPool::destroy() {
  if (_pool == VK_NULL_HANDLE) {
    throw std::runtime_error("Vulkan null descriptor pool cannot be destroyed!");
  }

  vkDestroyDescriptorPool(*_device, _pool, nullptr);
  _pool = VK_NULL_HANDLE;
  _device = nullptr;
}
