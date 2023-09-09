#include "Buffer.h"
#include "Device.h"

#include "helpers_vkdebug.h"

using namespace Vulkan;

Buffer::Buffer(const Device& device, size_t size,
               VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
  allocate(device, size, usage, properties);
}

Buffer::~Buffer() {
  if (isAllocated()) {
    free();
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

void Buffer::allocate(const Device& device, size_t size,
                      VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
  if (isAllocated()) {
    throw std::runtime_error("Vulkan buffer has been allocated already!");
  }
  _device = &device;

  _size = size;

  VkBufferCreateInfo bufferInfo{};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = size;
  bufferInfo.usage = usage;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  MG_VERIFY_VKCMD(vkCreateBuffer(device, &bufferInfo, nullptr, &_buffer));

  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements(device, _buffer, &memRequirements);

  VkMemoryAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

  MG_VERIFY_VKCMD(vkAllocateMemory(device, &allocInfo, nullptr, &_memory));

  vkBindBufferMemory(device, _buffer, _memory, 0);
}

void Buffer::free() {
  if (!isAllocated()) {
    throw std::runtime_error("Vulkan null buffer cannot be released!");
  }
  vkFreeMemory(*_device, _memory, nullptr);
  vkDestroyBuffer(*_device, _buffer, nullptr);

  _device = nullptr;
  _buffer = VK_NULL_HANDLE;
  _memory = VK_NULL_HANDLE;
  _size = 0;
}

void* Buffer::map() {
  return map(0, size());
}

void* Buffer::map(size_t offset, size_t size) {
  void* data;
  vkMapMemory(*_device, _memory, offset, size, 0, &data);
  return data;
}

void Buffer::unmap() {
  vkUnmapMemory(*_device, _memory);
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
