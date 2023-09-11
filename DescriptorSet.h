#pragma once

#include <vulkan/vulkan.h>

namespace Vulkan {

class DescriptorPool;
class DescriptorSetLayout;

class DescriptorSet {
 public:
  DescriptorSet() = default;
  DescriptorSet(const DescriptorPool& pool, const DescriptorSetLayout& layout);
  ~DescriptorSet();

  void allocate(const DescriptorPool& pool, const DescriptorSetLayout& layout);
  void free();

  operator VkDescriptorSet() const { return _set; }
  operator const VkDescriptorSet*() const { return &_set; }

 private:
  VkDescriptorSet _set = VK_NULL_HANDLE;

  const DescriptorPool* _pool = nullptr;
};

} // namespace Vulkan