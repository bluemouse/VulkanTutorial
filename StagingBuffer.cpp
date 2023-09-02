#include "StagingBuffer.h"
#include "Device.h"

#include "helpers.h"

#include <cstring>

using namespace Vulkan;

StagingBuffer::StagingBuffer(const Device& device, VkDeviceSize size) {
  allocate(device, size);
}

void StagingBuffer::allocate(const Device& device, VkDeviceSize size) {
  Buffer::allocate(device, size,
                   VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

void* StagingBuffer::map(size_t offset, size_t size) {
  void* data;
  vkMapMemory(*_device, _memory, offset, size, 0, &data);
  return data;
}

void StagingBuffer::unmap() {
  vkUnmapMemory(*_device, _memory);
}

void StagingBuffer::copyFromHost(const void* src, size_t size) {
  void* data = map(0, size);
  std::memcpy(data, src, size);
  unmap();
}
