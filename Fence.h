#pragma once

#include <vulkan/vulkan.h>

namespace Vulkan {

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
  const Device* _device;

  VkFence _fence = VK_NULL_HANDLE;
};

} //namespace Vulkan