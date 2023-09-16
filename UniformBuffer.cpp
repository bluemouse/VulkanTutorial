#include "UniformBuffer.h"

#include "Device.h"

NAMESPACE_VULKAN_BEGIN

UniformBuffer::UniformBuffer(const Device& device, VkDeviceSize size) {
  create(device, size);
}

void UniformBuffer::create(const Device& device, VkDeviceSize size) {
  Buffer::create(device, size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
  Buffer::allocate(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

NAMESPACE_VULKAN_END
