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
  Sampler(const Device& device,
          VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT,
          Filter filter = {VK_FILTER_LINEAR, VK_FILTER_LINEAR});
  ~Sampler();

  // Transfer the ownership from `rhs` to `this`
  Sampler(Sampler&& rhs) noexcept;
  Sampler& operator=(Sampler&& rhs) noexcept(false);

  void create(const Device& device,
              VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT,
              Filter filter = {VK_FILTER_LINEAR, VK_FILTER_LINEAR});
  void destroy();

  operator VkSampler() const { return _sampler; }

  [[nodiscard]] bool isCreated() const { return _sampler != VK_NULL_HANDLE; }

 private:
  void moveFrom(Sampler& rhs);

 private:
  VkSampler _sampler = VK_NULL_HANDLE;

  const Device* _device = nullptr;
};

NAMESPACE_VULKAN_END