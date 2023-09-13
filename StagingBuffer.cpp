#include "StagingBuffer.h"

#include <cstring>

#include "Device.h"

NAMESPACE_VULKAN_BEGIN

StagingBuffer::StagingBuffer(const Device& device, size_t size) {
  allocate(device, size);
}

void StagingBuffer::allocate(const Device& device, size_t size) {
  Buffer::allocate(device, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

void StagingBuffer::copyFromHost(const void* src, size_t size) {
  void* data = map(0, size);
  std::memcpy(data, src, size);
  unmap();
}

NAMESPACE_VULKAN_END
