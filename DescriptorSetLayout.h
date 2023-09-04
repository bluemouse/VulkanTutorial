#pragma once

#include <vulkan/vulkan.h>

#include <vector>

namespace Vulkan {

class Device;

class DescriptorSetLayout {
public:
  DescriptorSetLayout() = default;
  DescriptorSetLayout(const Device& device, std::vector<VkDescriptorSetLayoutBinding> bindings);
  ~DescriptorSetLayout();

  void init(const Device& device, std::vector<VkDescriptorSetLayoutBinding> bindings);
  void release();

  operator VkDescriptorSetLayout() const { return _layout; }

private:
  const Device* _device = nullptr;

  VkDescriptorSetLayout _layout = VK_NULL_HANDLE;
};

} //namespace Vulkan