#include "DescriptorSet.h"

#include "DescriptorPool.h"
#include "DescriptorSetLayout.h"
#include "Device.h"
#include "helpers_vulkan.h"

NAMESPACE_VULKAN_BEGIN

DescriptorSet::DescriptorSet(const DescriptorPool& pool, const DescriptorSetLayout& layout) {
  allocate(pool, layout);
}

DescriptorSet::~DescriptorSet() {
  if (_set != VK_NULL_HANDLE) {
    free();
  }
}

DescriptorSet::DescriptorSet(DescriptorSet&& rhs) noexcept {
  moveFrom(rhs);
}

DescriptorSet& DescriptorSet::operator=(DescriptorSet&& rhs) noexcept(false) {
  if (this != &rhs) {
    moveFrom(rhs);
  }
  return *this;
}

void DescriptorSet::allocate(const DescriptorPool& pool, const DescriptorSetLayout& layout) {
  MI_VERIFY(_set == VK_NULL_HANDLE);
  _pool = &pool;

  VkDescriptorSetAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = pool;
  allocInfo.descriptorSetCount = 1;
  allocInfo.pSetLayouts = layout;

  MI_VERIFY_VKCMD(vkAllocateDescriptorSets(pool.device(), &allocInfo, &_set));
}

void DescriptorSet::free() {
  MI_VERIFY(_set != VK_NULL_HANDLE);

  // TODO: Can only call free() if _pool is created with
  // VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT.
  //  vkFreeDescriptorSets(_pool->device(), *_pool, 1, &_set);

  _set = VK_NULL_HANDLE;
  _pool = nullptr;
}

void DescriptorSet::moveFrom(DescriptorSet& rhs) {
  MI_VERIFY(_set == VK_NULL_HANDLE);
  _set = rhs._set;
  _pool = rhs._pool;

  rhs._set = VK_NULL_HANDLE;
  rhs._pool = nullptr;
}

NAMESPACE_VULKAN_END
