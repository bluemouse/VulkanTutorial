#pragma once

#include <vulkan/vulkan.h>

#include "helpers_vulkan.h"

NAMESPACE_VULKAN_BEGIN

class Device;

class ShaderModule {
 public:
  ShaderModule(const Device& device, const char* shaderFile);
  ~ShaderModule();

  operator VkShaderModule() const { return _shader; }

 private:
  VkShaderModule _shader = VK_NULL_HANDLE;

  const Device* _device;
};

NAMESPACE_VULKAN_END