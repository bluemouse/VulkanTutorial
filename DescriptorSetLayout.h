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

  // Transfer the ownership from `rhs` to `this`
  DescriptorSetLayout(DescriptorSetLayout&& rhs) noexcept;
  DescriptorSetLayout& operator=(DescriptorSetLayout&& rhs) noexcept(false);

  void create(const Device& device, std::vector<VkDescriptorSetLayoutBinding> bindings);
  void destroy();

  operator VkDescriptorSetLayout() const { return _layout; }
  operator const VkDescriptorSetLayout*() const { return &_layout; }

  [[nodiscard]] bool isCreated() const { return _layout != VK_NULL_HANDLE; }

 private:
  void moveFrom(DescriptorSetLayout& rhs);

 private:
  VkDescriptorSetLayout _layout = VK_NULL_HANDLE;

  const Device* _device = nullptr;
};

NAMESPACE_VULKAN_END