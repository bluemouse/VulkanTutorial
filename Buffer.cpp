#include "Buffer.h"
#include "Device.h"

#include "helpers.h"

using namespace Vulkan;

Buffer::Buffer(const Device& device,
              VkDeviceSize size, VkBufferUsageFlags usage,
              VkMemoryPropertyFlags properties) {
  allocate(device, size, usage, properties);
}

Buffer::~Buffer() {
  if (_buffer != VK_NULL_HANDLE) {
    release();
  }
}

Buffer::Buffer(const Buffer& rhs) {
  moveFrom(const_cast<Buffer&>(rhs));
}

Buffer& Buffer::operator=(const Buffer& rhs) {
  if (isAllocated()) {
    throw std::runtime_error("Vulkan buffer has been allocated and can not be assigned!");
  }
  moveFrom(const_cast<Buffer&>(rhs));
  return *this;
}

void Buffer::moveFrom(Buffer& rhs) {
  if (this != &rhs) {
    _device = rhs._device;
    _buffer = rhs._buffer;
    _memory = rhs._memory;

    rhs._device = nullptr;
    rhs._buffer = VK_NULL_HANDLE;
    rhs._memory = VK_NULL_HANDLE;
  }
}

void Buffer::allocate(const Device& device,
                      VkDeviceSize size, VkBufferUsageFlags usage,
                      VkMemoryPropertyFlags properties) {
  if (isAllocated()) {
    throw std::runtime_error("Vulkan buffer has been allocated already!");
  }
  _device = &device;

  VkBufferCreateInfo bufferInfo{};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = size;
  bufferInfo.usage = usage;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VK_VERIFY(vkCreateBuffer(device, &bufferInfo, nullptr, &_buffer));

  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements(device, _buffer, &memRequirements);

  VkMemoryAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

  VK_VERIFY(vkAllocateMemory(device, &allocInfo, nullptr, &_memory));

  vkBindBufferMemory(device, _buffer, _memory, 0);
}

void Buffer::release() {
  if (!isAllocated()) {
    throw std::runtime_error("Vulkan null buffer cannot be released!");
  }
  vkFreeMemory(*_device, _memory, nullptr);
  vkDestroyBuffer(*_device, _buffer, nullptr);

  _device = nullptr;
  _buffer = VK_NULL_HANDLE;
  _memory = VK_NULL_HANDLE;
}

uint32_t Buffer::findMemoryType(uint32_t typeFilter,
                                VkMemoryPropertyFlags properties) const {
  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(_device->physicalDevice(), &memProperties);

  for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
    if ((typeFilter & (1 << i)) &&
        (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
      return i;
    }
  }
  throw std::runtime_error("Failed to find suitable memory type!");
}
