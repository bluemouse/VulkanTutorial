#include "UniformBuffer.h"
#include "Device.h"

#include "helpers_vulkan.h"

using namespace Vulkan;

UniformBuffer::UniformBuffer(const Device& device, size_t size) {
  allocate(device, size);
}

UniformBuffer::UniformBuffer(const UniformBuffer& rhs) : Buffer(rhs) {
}

UniformBuffer& UniformBuffer::operator=(const UniformBuffer& rhs) {
  if (isAllocated()) {
    throw std::runtime_error("Vulkan uniform buffer has been allocated and can not be assigned!");
  }
  Buffer::operator=(rhs);
  return *this;
}

void UniformBuffer::allocate(const Device& device, size_t size) {
  if (isAllocated()) {
    throw std::runtime_error("Vulkan uniform buffer has been allocated already!");
  }
  Buffer::allocate(device, size,
                   VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}
