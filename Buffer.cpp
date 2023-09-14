#include "Buffer.h"

#include "Device.h"
#include "helpers_vulkan.h"

namespace Vulkan {

Buffer::Buffer(const Device& device,
               size_t size,
               VkBufferUsageFlags usage,
               VkMemoryPropertyFlags properties) {
  allocate(device, size, usage, properties);
}

Buffer::~Buffer() {
  if (isAllocated()) {
    free();
  }
}

Buffer::Buffer(Buffer&& rhs) noexcept {
  moveFrom(rhs);
}

Buffer& Buffer::operator=(Buffer&& rhs) noexcept(false) {
  if (this == &rhs) {
    return *this;
  }
  MI_VERIFY(!isAllocated());
  moveFrom(rhs);
  return *this;
}

void Buffer::moveFrom(Buffer& rhs) {
  _device = rhs._device;
  _buffer = rhs._buffer;
  _memory = rhs._memory;

  rhs._device = nullptr;
  rhs._buffer = VK_NULL_HANDLE;
  rhs._memory = VK_NULL_HANDLE;
}

void Buffer::allocate(const Device& device,
                      size_t size,
                      VkBufferUsageFlags usage,
                      VkMemoryPropertyFlags properties) {
  MI_VERIFY(!isAllocated());
  _device = &device;

  _size = size;

  VkBufferCreateInfo bufferInfo{};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = size;
  bufferInfo.usage = usage;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  MI_VERIFY_VKCMD(vkCreateBuffer(device, &bufferInfo, nullptr, &_buffer));

  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements(device, _buffer, &memRequirements);

  VkMemoryAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

  MI_VERIFY_VKCMD(vkAllocateMemory(device, &allocInfo, nullptr, &_memory));

  vkBindBufferMemory(device, _buffer, _memory, 0);
}

void Buffer::free() {
  MI_VERIFY(isAllocated());

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
  void* data = nullptr;
  vkMapMemory(*_device, _memory, offset, size, 0, &data);
  return data;
}

void Buffer::unmap() {
  vkUnmapMemory(*_device, _memory);
}

uint32_t Buffer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const {
  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(_device->physicalDevice(), &memProperties);

  uint32_t idx = 0;
  for (auto& memType : memProperties.memoryTypes) {
    if ((typeFilter & (1 << idx)) != 0 && (memType.propertyFlags & properties) == properties) {
      return idx;
    }
    ++idx;
  }
  throw std::runtime_error("Failed to find suitable memory type!");
}

} // namespace Vulkan
