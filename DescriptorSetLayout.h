#pragma once

#include <vulkan/vulkan.h>

#include <vector>

#include "helpers_vulkan.h"

NAMESPACE_VULKAN_BEGIN

class Device;
class ShaderModule;

class DescriptorSetLayout {
 public:
  DescriptorSetLayout() = default;
  DescriptorSetLayout(const Device& device, std::vector<ShaderModule*> shaders);
  ~DescriptorSetLayout();

  // Transfer the ownership from `rhs` to `this`
  DescriptorSetLayout(DescriptorSetLayout&& rhs) noexcept;
  DescriptorSetLayout& operator=(DescriptorSetLayout&& rhs) noexcept(false);

  void create(const Device& device, std::vector<ShaderModule*> shaders);
  void destroy();

  operator VkDescriptorSetLayout() const { return _layout; }
  operator const VkDescriptorSetLayout*() const { return &_layout; }

  [[nodiscard]] bool isCreated() const { return _layout != VK_NULL_HANDLE; }

  [[nodiscard]] const Device& device() const { return *_device; }

  [[nodiscard]] const std::vector<VkDescriptorPoolSize>& poolSizes() const { return _poolSizes; }
  [[nodiscard]] const std::vector<VkDescriptorSetLayoutBinding>& bindings() const {
    return _bindings;
  }

 private:
  void moveFrom(DescriptorSetLayout& rhs);

 private:
  VkDescriptorSetLayout _layout = VK_NULL_HANDLE;

  std::vector<VkDescriptorSetLayoutBinding> _bindings;
  std::vector<VkDescriptorPoolSize> _poolSizes;

  const Device* _device = nullptr;
};

NAMESPACE_VULKAN_END