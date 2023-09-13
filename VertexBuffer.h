#pragma once

#include <vulkan/vulkan.h>

#include "Buffer.h"

NAMESPACE_VULKAN_BEGIN

class Device;

class VertexBuffer : private Buffer {
 public:
  VertexBuffer() = default;
  VertexBuffer(const Device& device, size_t size);

  // Transfer the ownership from `rhs` to `this`
  VertexBuffer(VertexBuffer&& rhs) = default;
  VertexBuffer& operator=(VertexBuffer&& rhs) noexcept(false);

  void allocate(const Device& device, size_t size);
  using Buffer::free;

  using Buffer::operator VkBuffer;
  using Buffer::isAllocated;
  using Buffer::size;
};

NAMESPACE_VULKAN_END