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

  // Transfer the ownership from `rhs` to `this`
  Fence(Fence&& rhs) noexcept;
  Fence& operator=(Fence&& rhs) noexcept(false);

  void create(const Device& device);
  void destroy();

  operator VkFence() const { return _fence; }
  operator const VkFence*() const { return &_fence; }

  [[nodiscard]] bool isCreated() const { return _fence != VK_NULL_HANDLE; }

 private:
  void moveFrom(Fence& rhs);

 private:
  VkFence _fence = VK_NULL_HANDLE;

  const Device* _device = nullptr;
};

NAMESPACE_VULKAN_END