#pragma once

#include <vulkan/vulkan.h>

#include "Buffer.h"

NAMESPACE_VULKAN_BEGIN

class Device;

class UniformBuffer : public Buffer {
 public:
  UniformBuffer() = default;
  UniformBuffer(const Device& device, VkDeviceSize size);

  // Transfer the ownership from `rhs` to `this`
  UniformBuffer(UniformBuffer&& rhs) = default;
  UniformBuffer& operator=(UniformBuffer&& rhs) noexcept(false) = default;

  void create(const Device& device, VkDeviceSize size);
};

NAMESPACE_VULKAN_END