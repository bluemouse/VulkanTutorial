#include "StagingBuffer.h"

#include <cstring>

#include "Device.h"

NAMESPACE_VULKAN_BEGIN

StagingBuffer::StagingBuffer(const Device& device, VkDeviceSize size) {
  create(device, size);
}

void StagingBuffer::create(const Device& device, VkDeviceSize size) {
  Buffer::create(device, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
  Buffer::allocate(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

void StagingBuffer::copyFromHost(const void* src, VkDeviceSize size) {
  void* data = map(0, size);
  std::memcpy(data, src, size);
  unmap();
}

NAMESPACE_VULKAN_END
