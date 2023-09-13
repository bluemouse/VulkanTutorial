#pragma once

#include <vulkan/vulkan.h>

#include "helpers_vulkan.h"

NAMESPACE_VULKAN_BEGIN

class Device;

class Fence {
 public:
  Fence() = default;
  explicit Fence(const Device& device);
  ~Fence();

  void create(const Device& device);
  void destroy();

  operator VkFence() const { return _fence; }
  operator const VkFence*() const { return &_fence; }

 private:
  VkFence _fence = VK_NULL_HANDLE;

  const Device* _device = nullptr;
};

NAMESPACE_VULKAN_END