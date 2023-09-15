#pragma once

#include <vulkan/vulkan.h>

#include "Buffer.h"

NAMESPACE_VULKAN_BEGIN

class Device;

class UniformBuffer : private Buffer {
 public:
  UniformBuffer() = default;
  UniformBuffer(const Device& device, VkDeviceSize size);

  // Transfer the ownership from `rhs` to `this`
  UniformBuffer(UniformBuffer&& rhs) = default;
  UniformBuffer& operator=(UniformBuffer&& rhs) noexcept(false) = default;

  void create(const Device& device, VkDeviceSize size);
  using Buffer::destroy;

  using Buffer::map;
  using Buffer::unmap;
  using Buffer::isMapped;

  using Buffer::operator VkBuffer;
  using Buffer::memory;

  using Buffer::isAllocated;
  using Buffer::size;
};

NAMESPACE_VULKAN_END