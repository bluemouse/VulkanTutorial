#pragma once

#include <vulkan/vulkan.h>

#include "Buffer.h"

NAMESPACE_VULKAN_BEGIN

class Device;

class StagingBuffer : private Buffer {
 public:
  StagingBuffer() = default;
  StagingBuffer(const Device& device, size_t size);

  // Transfer the ownership from `rhs` to `this`
  StagingBuffer(StagingBuffer&& rhs) = default;
  StagingBuffer& operator=(StagingBuffer&& rhs) noexcept(false) = default;

  void allocate(const Device& device, size_t size);
  using Buffer::free;

  using Buffer::map;
  using Buffer::unmap;

  void copyFromHost(const void* src, size_t size);

  using Buffer::operator VkBuffer;
  using Buffer::size;
};

NAMESPACE_VULKAN_END