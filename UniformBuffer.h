#pragma once

#include <vulkan/vulkan.h>

#include "Buffer.h"

namespace Vulkan {

class Device;

class UniformBuffer : private Buffer {
public:
  UniformBuffer() = default;
  UniformBuffer(const Device& device, size_t size);

  // Transfer the ownership from `rhs` to `this`
  UniformBuffer(const UniformBuffer& rhs);
  UniformBuffer& operator=(const UniformBuffer& rhs);

  void allocate(const Device& device, size_t size);
  void release();

  using Buffer::operator VkBuffer;
  using Buffer::memory;
  using Buffer::map;
  using Buffer::unmap;
  using Buffer::isAllocated;
  using Buffer::size;
};

} //namespace Vulkan