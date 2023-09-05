#include "DescriptorSetLayout.h"
#include "Device.h"

#include "helpers.h"

using namespace Vulkan;

DescriptorSetLayout::DescriptorSetLayout(const Device& device,
                                         std::vector<VkDescriptorSetLayoutBinding> bindings) {
  create(device, std::move(bindings));
}

DescriptorSetLayout::~DescriptorSetLayout() {
  if (_layout != VK_NULL_HANDLE) {
    destroy();
  }
}

void DescriptorSetLayout::create(const Device& device,
                               std::vector<VkDescriptorSetLayoutBinding> bindings) {
  if (_layout != VK_NULL_HANDLE) {
    throw std::runtime_error("Vulkan descriptor set layout has been initialized already!");
  }

  _device = &device;

  VkDescriptorSetLayoutCreateInfo layoutInfo{};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
  layoutInfo.pBindings = bindings.data();

  VK_VERIFY(vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &_layout));
}

void DescriptorSetLayout::destroy() {
  if (_layout == VK_NULL_HANDLE) {
    throw std::runtime_error("Vulkan null descriptor set layout cannot be released!");
  }

  vkDestroyDescriptorSetLayout(*_device, _layout, nullptr);
  _layout = VK_NULL_HANDLE;
  _device = nullptr;
}
