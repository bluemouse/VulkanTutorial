#pragma once

#include <vulkan/vulkan.h>

#include <vector>

#include "helpers_vulkan.h"

NAMESPACE_VULKAN_BEGIN

class Device;
class DescriptorSetLayout;

class DescriptorPool {
 public:
  DescriptorPool() = default;
  DescriptorPool(const DescriptorSetLayout& layout, uint32_t maxSets);
  DescriptorPool(const Device& device,
                 std::vector<VkDescriptorPoolSize> poolSizes,
                 uint32_t maxSets);
  ~DescriptorPool();

  // Transfer the ownership from `rhs` to `this`
  DescriptorPool(DescriptorPool&& rhs) noexcept;
  DescriptorPool& operator=(DescriptorPool&& rhs) noexcept(false);

  void create(const DescriptorSetLayout& layout, uint32_t maxSets);
  void create(const Device& device, std::vector<VkDescriptorPoolSize> poolSizes, uint32_t maxSets);
  void destroy();

  operator VkDescriptorPool() const { return _pool; }

  [[nodiscard]] const Device& device() const { return *_device; }

  [[nodiscard]] bool isCreated() const { return _pool != VK_NULL_HANDLE; }

 private:
  void moveFrom(DescriptorPool& rhs);

 private:
  VkDescriptorPool _pool = VK_NULL_HANDLE;

  const Device* _device = nullptr;
};

NAMESPACE_VULKAN_END