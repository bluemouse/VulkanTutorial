#include "UniformBuffer.h"

#include "Device.h"

NAMESPACE_VULKAN_BEGIN

UniformBuffer::UniformBuffer(const Device& device, size_t size) {
  allocate(device, size);
}

void UniformBuffer::allocate(const Device& device, size_t size) {
  if (isAllocated()) {
    throw std::runtime_error("Vulkan uniform buffer has been allocated already!");
  }
  Buffer::allocate(device,
                   size,
                   VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

NAMESPACE_VULKAN_END
