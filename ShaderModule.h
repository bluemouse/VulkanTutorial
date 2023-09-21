#pragma once

#include <vulkan/vulkan.h>

#include "helpers_vulkan.h"

#include <vector>

NAMESPACE_VULKAN_BEGIN

class Device;

class ShaderModule {
 public:
  ShaderModule() = default;
  ShaderModule(const Device& device, const char* entry, size_t codeSize, const char* codes);
  ShaderModule(const Device& device, const char* entry, const char* shaderFile);
  virtual ~ShaderModule();

  void create(const Device& device, const char* entry, size_t codeSize, const char* codes);
  void create(const Device& device, const char* entry, const char* shaderFile);
  void destroy();

  // Transfer the ownership from `rhs` to `this`
  ShaderModule(ShaderModule&& rhs) noexcept;
  ShaderModule& operator=(ShaderModule&& rhs) noexcept(false);

  operator VkShaderModule() const { return _shader; }

  [[nodiscard]] bool isCreated() const { return _shader != VK_NULL_HANDLE; }

  void addDescriptorSetLayoutBinding(uint32_t binding,
                                     VkDescriptorType descriptorType,
                                     VkShaderStageFlags stageFlags);

  [[nodiscard]] const std::vector<VkDescriptorSetLayoutBinding>& descriptorSetLayoutBindings() const {
    return _descriptorSetLayoutBindings;
  }

  [[nodiscard]] const char* entry() const { return _entry; }


 protected:
  void moveFrom(ShaderModule& rhs);

 protected:
  VkShaderModule _shader = VK_NULL_HANDLE;

  const char* _entry = nullptr;
  std::vector<VkDescriptorSetLayoutBinding> _descriptorSetLayoutBindings;

  const Device* _device;
};

NAMESPACE_VULKAN_END