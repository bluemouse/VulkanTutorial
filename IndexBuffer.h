#pragma once

#include <vulkan/vulkan.h>

#include "Buffer.h"

NAMESPACE_VULKAN_BEGIN

class Device;

class IndexBuffer : private Buffer {
 public:
  IndexBuffer() = default;
  IndexBuffer(const Device& device, size_t size);

  // Transfer the ownership from `rhs` to `this`
  IndexBuffer(IndexBuffer&& rhs) = default;
  IndexBuffer& operator=(IndexBuffer&& rhs) noexcept(false) = default;

  void allocate(const Device& device, size_t size);
  using Buffer::free;

  using Buffer::operator VkBuffer;
  using Buffer::isAllocated;
  using Buffer::size;
};

NAMESPACE_VULKAN_END