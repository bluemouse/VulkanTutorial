#pragma once

#include <vulkan/vulkan.h>

#include <vector>

namespace Vulkan {

class Device;

class DescriptorPool {
public:
  DescriptorPool() = default;
  DescriptorPool(const Device& device, std::vector<VkDescriptorPoolSize> poolSizes, uint32_t maxSets);
  ~DescriptorPool();

  void create(const Device& device, std::vector<VkDescriptorPoolSize> poolSizes, uint32_t maxSets);
  void destroy();

  operator VkDescriptorPool() const { return _pool; }

  const Device& device() const { return *_device; }

private:
  VkDescriptorPool _pool = VK_NULL_HANDLE;

  const Device* _device = nullptr;
};

} //namespace Vulkan