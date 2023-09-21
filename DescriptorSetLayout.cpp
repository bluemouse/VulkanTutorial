#include "DescriptorSetLayout.h"

#include "Device.h"
#include "ShaderModule.h"
#include "helpers_vulkan.h"

NAMESPACE_VULKAN_BEGIN

DescriptorSetLayout::DescriptorSetLayout(const Device& device, std::vector<ShaderModule*> shaders) {
  create(device, std::move(shaders));
}

DescriptorSetLayout::~DescriptorSetLayout() {
  if (isCreated()) {
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

void DescriptorSetLayout::moveFrom(DescriptorSetLayout& rhs) {
  MI_VERIFY(!isCreated());
  _layout = rhs._layout;
  _device = rhs._device;

  rhs._layout = VK_NULL_HANDLE;
  rhs._device = nullptr;
}

void DescriptorSetLayout::create(const Device& device, std::vector<ShaderModule*> shaders) {
  MI_VERIFY(!isCreated());
  _device = &device;

  size_t numBindings = 0;
  for (auto* shader : shaders) {
    numBindings += shader->descriptorSetLayoutBindings().size();
  }

  std::vector<VkDescriptorSetLayoutBinding> bindings;
  bindings.reserve(numBindings);
  for (auto* shader : shaders) {
    for (const auto& binding : shader->descriptorSetLayoutBindings()) {
      bindings.push_back(binding);
    }
  }

  VkDescriptorSetLayoutCreateInfo layoutInfo{};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
  layoutInfo.pBindings = bindings.data();

  MI_VERIFY_VKCMD(vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &_layout));
}

void DescriptorSetLayout::destroy() {
  MI_VERIFY(isCreated());

  vkDestroyDescriptorSetLayout(*_device, _layout, nullptr);
  _layout = VK_NULL_HANDLE;
  _device = nullptr;
}

NAMESPACE_VULKAN_END
