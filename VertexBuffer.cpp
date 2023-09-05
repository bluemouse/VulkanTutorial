#include "VertexBuffer.h"

#include "helpers.h"

using namespace Vulkan;

VertexBuffer::VertexBuffer(const Device& device, size_t size) {
  allocate(device, size);
}

VertexBuffer::VertexBuffer(const VertexBuffer& rhs) : Buffer(rhs) {
}

VertexBuffer& VertexBuffer::operator=(const VertexBuffer& rhs) {
  if (isAllocated()) {
    throw std::runtime_error("Vulkan vertex buffer has been allocated and can not be assigned!");
  }
  Buffer::operator=(rhs);
  return *this;
}

void VertexBuffer::allocate(const Device& device, size_t size) {
  if (isAllocated()) {
    throw std::runtime_error("Vulkan vertex buffer has been allocated already!");
  }
  Buffer::allocate(device, size,
                   VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
}
