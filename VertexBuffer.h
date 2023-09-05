#pragma once

#include <vulkan/vulkan.h>

#include "Buffer.h"

namespace Vulkan {

class Device;

class VertexBuffer : private Buffer {
public:
  VertexBuffer() = default;
  VertexBuffer(const Device& device, size_t size);

  // Transfer the ownership from `rhs` to `this`
  VertexBuffer(const VertexBuffer& rhs);
  VertexBuffer& operator=(const VertexBuffer& rhs);

  void allocate(const Device& device, size_t size);
  using Buffer::free;

  using Buffer::operator VkBuffer;
  using Buffer::isAllocated;
  using Buffer::size;
};

} //namespace Vulkan