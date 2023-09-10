#include "StagingBuffer.h"
#include "Device.h"

#include "helpers_vulkan.h"

#include <cstring>

using namespace Vulkan;

StagingBuffer::StagingBuffer(const Device& device, size_t size) {
  allocate(device, size);
}

void StagingBuffer::allocate(const Device& device, size_t size) {
  Buffer::allocate(device, size,
                   VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

void StagingBuffer::copyFromHost(const void* src, size_t size) {
  void* data = map(0, size);
  std::memcpy(data, src, size);
  unmap();
}
