#pragma once

#include <vulkan/vulkan.h>

#include "Buffer.h"

NAMESPACE_VULKAN_BEGIN

class Device;

class StagingBuffer : private Buffer {
 public:
  StagingBuffer() = default;
  StagingBuffer(const Device& device, VkDeviceSize size);

  // Transfer the ownership from `rhs` to `this`
  StagingBuffer(StagingBuffer&& rhs) = default;
  StagingBuffer& operator=(StagingBuffer&& rhs) noexcept(false) = default;

  void create(const Device& device, VkDeviceSize size);
  using Buffer::destroy;

  using Buffer::allocate;

  using Buffer::map;
  using Buffer::unmap;
  using Buffer::isMapped;

  void copyFromHost(const void* src, VkDeviceSize size);

  using Buffer::operator VkBuffer;
  using Buffer::size;
};

NAMESPACE_VULKAN_END