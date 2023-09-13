#pragma once

#include <vulkan/vulkan.h>

#include "helpers_vulkan.h"

NAMESPACE_VULKAN_BEGIN

class Device;

class Sampler {
 public:
  struct Filter {
    VkFilter mag;
    VkFilter min;
  };

 public:
  Sampler() = default;
  Sampler(const Device& device, VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT,
          Filter filter = {VK_FILTER_LINEAR, VK_FILTER_LINEAR});
  ~Sampler();

  void create(const Device& device,
              VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT,
              Filter filter = {VK_FILTER_LINEAR, VK_FILTER_LINEAR});
  void destroy();

  operator VkSampler() const { return _sampler; }

 private:
  VkSampler _sampler = VK_NULL_HANDLE;

  const Device* _device = nullptr;
};

NAMESPACE_VULKAN_END