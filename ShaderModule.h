#pragma once

#include <vulkan/vulkan.h>

#include "helpers_vulkan.h"

NAMESPACE_VULKAN_BEGIN

class Device;

class ShaderModule {
 public:
  ShaderModule(const Device& device, const char* shaderFile);
  ~ShaderModule();

  // Transfer the ownership from `rhs` to `this`
  ShaderModule(ShaderModule&& rhs) noexcept;
  ShaderModule& operator=(ShaderModule&& rhs) noexcept(false);

  operator VkShaderModule() const { return _shader; }

private:
  void moveFrom(ShaderModule& rhs);

 private:
  VkShaderModule _shader = VK_NULL_HANDLE;

  const Device* _device;
};

NAMESPACE_VULKAN_END