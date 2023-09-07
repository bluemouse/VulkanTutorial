#include "DescriptorSet.h"

#include "Device.h"
#include "DescriptorPool.h"
#include "DescriptorSetLayout.h"

#include "helpers.h"

using namespace Vulkan;

DescriptorSet::DescriptorSet(const DescriptorPool& pool, const DescriptorSetLayout& layout) {
  allocate(pool, layout);
}

DescriptorSet::~DescriptorSet() {
  if (_set != VK_NULL_HANDLE) {
    free();
  }
}

void DescriptorSet::allocate(const DescriptorPool& pool, const DescriptorSetLayout& layout) {
  if (_set != VK_NULL_HANDLE) {
    throw std::runtime_error("Vulkan descriptor set has been allocated already!");
  }
  _pool = &pool;

  VkDescriptorSetAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = pool;
  allocInfo.descriptorSetCount = 1;
  allocInfo.pSetLayouts = layout;

  VK_VERIFY(vkAllocateDescriptorSets(pool.device(), &allocInfo, &_set));
}

void DescriptorSet::free() {
  if (_set == VK_NULL_HANDLE) {
    throw std::runtime_error("Vulkan null descriptor set cannot be freed!");
  }

  //TODO: Can only call free() if _pool is created with VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT.
  // vkFreeDescriptorSets(_pool->device(), *_pool, 1, &_set);

  _set = VK_NULL_HANDLE;
  _pool = nullptr;
}
