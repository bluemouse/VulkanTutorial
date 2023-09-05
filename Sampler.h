#pragma once

#include <vulkan/vulkan.h>

namespace Vulkan {

class Device;

class Sampler {
public:
  Sampler() = default;
  Sampler(const Device& device, VkFilter magFilter, VkFilter minFilter, VkSamplerAddressMode mode);
  ~Sampler();

  void init(const Device& device, VkFilter magFilter, VkFilter minFilter, VkSamplerAddressMode mode);
  void release();

  operator VkSampler() const { return _sampler; }

private:
  const Device* _device;

  VkSampler _sampler = VK_NULL_HANDLE;
};

} //namespace Vulkan