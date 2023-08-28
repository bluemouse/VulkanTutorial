#pragma once

#include <vulkan/vulkan.h>

namespace Vulkan {

class Device;

class ShaderModule {
public:
  ShaderModule(const Device& device, const char* shaderFile);
  ~ShaderModule();

  operator VkShaderModule() const { return _shader; }

private:
  const Device* _device;

  VkShaderModule _shader = VK_NULL_HANDLE;
};

} //namespace Vulkan