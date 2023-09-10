#include "IndexBuffer.h"

#include "helpers_vulkan.h"

using namespace Vulkan;

IndexBuffer::IndexBuffer(const Device& device, size_t size) {
  allocate(device, size);
}

IndexBuffer::IndexBuffer(const IndexBuffer& rhs) : Buffer(rhs) {
}

IndexBuffer& IndexBuffer::operator=(const IndexBuffer& rhs) {
  if (isAllocated()) {
    throw std::runtime_error("Vulkan vertex buffer has been allocated and can not be assigned!");
  }
  Buffer::operator=(rhs);
  return *this;
}

void IndexBuffer::allocate(const Device& device, size_t size) {
  if (isAllocated()) {
    throw std::runtime_error("Vulkan vertex buffer has been allocated already!");
  }
  Buffer::allocate(device, size,
                   VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
}