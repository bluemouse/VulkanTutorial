#pragma once

#include <vulkan/vulkan.h>

#include "Buffer.h"

namespace Vulkan {

class Device;

class StagingBuffer : public Buffer{
public:
  StagingBuffer() = default;
  StagingBuffer(const Device& device, VkDeviceSize size);

  StagingBuffer(const StagingBuffer&) = delete;
  StagingBuffer& operator=(const StagingBuffer&) = delete;

  void allocate(const Device& device, VkDeviceSize size);

  operator VkBuffer() const { return _buffer; }

  void* map(size_t offset, size_t size);
  void unmap();

  void copyFromHost(const void* src, size_t size);
};

} //namespace Vulkan