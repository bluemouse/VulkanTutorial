#pragma once

#include <vulkan/vulkan.h>

#include "helpers_vulkan.h"

NAMESPACE_VULKAN_BEGIN

class DescriptorPool;
class DescriptorSetLayout;

class DescriptorSet {
 public:
  DescriptorSet() = default;
  DescriptorSet(const DescriptorPool& pool, const DescriptorSetLayout& layout);
  ~DescriptorSet();

  // Transfer the ownership from `rhs` to `this`
  DescriptorSet(DescriptorSet&& rhs) noexcept;
  DescriptorSet& operator=(DescriptorSet&& rhs) noexcept(false);

  void allocate(const DescriptorPool& pool, const DescriptorSetLayout& layout);
  void free();

  operator VkDescriptorSet() const { return _set; }
  operator const VkDescriptorSet*() const { return &_set; }

 private:
  void moveFrom(DescriptorSet& rhs);

 private:
  VkDescriptorSet _set = VK_NULL_HANDLE;

  const DescriptorPool* _pool = nullptr;
};

NAMESPACE_VULKAN_END