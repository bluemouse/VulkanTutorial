#include "IndexBuffer.h"

NAMESPACE_VULKAN_BEGIN

IndexBuffer::IndexBuffer(const Device& device, VkDeviceSize size) {
  create(device, size);
}

void IndexBuffer::create(const Device& device, VkDeviceSize size) {
  Buffer::create(device,
                 size,
                 VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
}

NAMESPACE_VULKAN_END
