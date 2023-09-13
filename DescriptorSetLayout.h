#pragma once

#include <vulkan/vulkan.h>

#include <vector>

#include "helpers_vulkan.h"

NAMESPACE_VULKAN_BEGIN

class Device;

class DescriptorSetLayout {
 public:
  DescriptorSetLayout() = default;
  DescriptorSetLayout(const Device& device, std::vector<VkDescriptorSetLayoutBinding> bindings);
  ~DescriptorSetLayout();

  void create(const Device& device, std::vector<VkDescriptorSetLayoutBinding> bindings);
  void destroy();

  operator VkDescriptorSetLayout() const { return _layout; }
  operator const VkDescriptorSetLayout*() const { return &_layout; }

 private:
  VkDescriptorSetLayout _layout = VK_NULL_HANDLE;

  const Device* _device = nullptr;
};

NAMESPACE_VULKAN_END