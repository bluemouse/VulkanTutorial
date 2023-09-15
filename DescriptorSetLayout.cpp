#include "DescriptorSetLayout.h"

#include "Device.h"
#include "helpers_vulkan.h"

NAMESPACE_VULKAN_BEGIN

DescriptorSetLayout::DescriptorSetLayout(const Device& device,
                                         std::vector<VkDescriptorSetLayoutBinding> bindings) {
  create(device, std::move(bindings));
}

DescriptorSetLayout::~DescriptorSetLayout() {
  if (_layout != VK_NULL_HANDLE) {
    destroy();
  }
}

DescriptorSetLayout::DescriptorSetLayout(DescriptorSetLayout&& rhs) noexcept {
  moveFrom(rhs);
}

DescriptorSetLayout& DescriptorSetLayout::operator=(DescriptorSetLayout&& rhs) noexcept(false) {
  if (this != &rhs) {
    moveFrom(rhs);
  }
  return *this;
}

void DescriptorSetLayout::create(const Device& device,
                                 std::vector<VkDescriptorSetLayoutBinding> bindings) {
  MI_VERIFY(_layout == VK_NULL_HANDLE);
  _device = &device;

  VkDescriptorSetLayoutCreateInfo layoutInfo{};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
  layoutInfo.pBindings = bindings.data();

  MI_VERIFY_VKCMD(vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &_layout));
}

void DescriptorSetLayout::destroy() {
  MI_VERIFY(_layout != VK_NULL_HANDLE);

  vkDestroyDescriptorSetLayout(*_device, _layout, nullptr);
  _layout = VK_NULL_HANDLE;
  _device = nullptr;
}

void DescriptorSetLayout::moveFrom(DescriptorSetLayout& rhs) {
  MI_VERIFY(_layout == VK_NULL_HANDLE);
  _layout = rhs._layout;
  _device = rhs._device;

  rhs._layout = VK_NULL_HANDLE;
  rhs._device = nullptr;
}

NAMESPACE_VULKAN_END
