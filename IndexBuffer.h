#pragma once

#include <vulkan/vulkan.h>

#include "Buffer.h"

namespace Vulkan {

class Device;

class IndexBuffer : private Buffer {
public:
  IndexBuffer() = default;
  IndexBuffer(const Device& device, size_t size);

  // Transfer the ownership from `rhs` to `this`
  IndexBuffer(const IndexBuffer& rhs);
  IndexBuffer& operator=(const IndexBuffer& rhs);

  void allocate(const Device& device, size_t size);
  using Buffer::free;

  using Buffer::operator VkBuffer;
  using Buffer::isAllocated;
  using Buffer::size;
};

} //namespace Vulkan