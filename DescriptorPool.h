#pragma once

#include <vulkan/vulkan.h>

#include <vector>

#include "helpers_vulkan.h"

NAMESPACE_VULKAN_BEGIN

class Device;

class DescriptorPool {
 public:
  DescriptorPool() = default;
  DescriptorPool(const Device& device, std::vector<VkDescriptorPoolSize> poolSizes,
                 uint32_t maxSets);
  ~DescriptorPool();

  void create(const Device& device, std::vector<VkDescriptorPoolSize> poolSizes, uint32_t maxSets);
  void destroy();

  operator VkDescriptorPool() const { return _pool; }

  [[nodiscard]] const Device& device() const { return *_device; }

 private:
  VkDescriptorPool _pool = VK_NULL_HANDLE;

  const Device* _device = nullptr;
};

NAMESPACE_VULKAN_END