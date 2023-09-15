#include "VertexBuffer.h"

NAMESPACE_VULKAN_BEGIN

VertexBuffer::VertexBuffer(const Device& device, size_t size) {
  allocate(device, size);
}

void VertexBuffer::allocate(const Device& device, size_t size) {
  Buffer::allocate(device,
                   size,
                   VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
}

NAMESPACE_VULKAN_END
