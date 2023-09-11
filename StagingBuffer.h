#pragma once

#include <vulkan/vulkan.h>

#include "Buffer.h"

namespace Vulkan {

class Device;

class StagingBuffer : private Buffer {
 public:
  StagingBuffer() = default;
  StagingBuffer(const Device& device, size_t size);

  StagingBuffer(const StagingBuffer&) = delete;
  StagingBuffer& operator=(const StagingBuffer&) = delete;

  void allocate(const Device& device, size_t size);
  using Buffer::free;

  using Buffer::map;
  using Buffer::unmap;

  void copyFromHost(const void* src, size_t size);

  using Buffer::operator VkBuffer;
  using Buffer::size;
};

} // namespace Vulkan