#include "Buffer.h"

#include "Device.h"
#include "helpers_vulkan.h"

namespace Vulkan {

Buffer::Buffer(const Device& device,
               VkDeviceSize size,
               VkBufferUsageFlags usage) {
  create(device, size, usage);
}

Buffer::Buffer(const Device& device,
               VkDeviceSize size,
               VkBufferUsageFlags usage,
               VkMemoryPropertyFlags properties) {
  create(device, size, usage, properties);
}

Buffer::~Buffer() {
  if (isAllocated()) {
    destroy();
  }
}

Buffer::Buffer(Buffer&& rhs) noexcept {
  moveFrom(rhs);
}

Buffer& Buffer::operator=(Buffer&& rhs) noexcept(false) {
  if (this != &rhs) {
    moveFrom(rhs);
  }
  return *this;
}

void Buffer::moveFrom(Buffer& rhs) {
  MI_VERIFY(!isAllocated());
  _device = rhs._device;
  _buffer = rhs._buffer;
  _memory = rhs._memory;

  rhs._device = nullptr;
  rhs._buffer = VK_NULL_HANDLE;
  rhs._memory = VK_NULL_HANDLE;
}

void Buffer::create(const Device& device,
                    VkDeviceSize size,
                    VkBufferUsageFlags usage,
                    const BufferCreateInfoOverride& override) {
  MI_VERIFY(!isAllocated());
  _device = &device;

  _size = size;

  VkBufferCreateInfo bufferInfo{};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  // bufferInfo.pNext =
  // bufferInfo.flags =
  bufferInfo.size = size;
  bufferInfo.usage = usage;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  // bufferInfo.queueFamilyIndexCount =
  // bufferInfo.pQueueFamilyIndices =

  if (override) {
    override(&bufferInfo);
  }

  MI_VERIFY_VKCMD(vkCreateBuffer(device, &bufferInfo, nullptr, &_buffer));
}

void Buffer::create(const Device& device,
                    VkDeviceSize size,
                    VkBufferUsageFlags usage,
                    VkMemoryPropertyFlags properties,
                    const BufferCreateInfoOverride& override) {
  create(device, size, usage, override);
  allocate(properties);
}

void Buffer::destroy() {
  MI_VERIFY(_buffer != VK_NULL_HANDLE);

  if (isAllocated()) {
    free();
  }
  vkDestroyBuffer(*_device, _buffer, nullptr);

  _device = nullptr;
  _buffer = VK_NULL_HANDLE;
  _size = 0;
}

void Buffer::allocate(VkMemoryPropertyFlags properties) {
  MI_VERIFY(!isAllocated());

  VkMemoryRequirements requirements;
  vkGetBufferMemoryRequirements(*_device, _buffer, &requirements);

  VkMemoryAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = requirements.size;
  allocInfo.memoryTypeIndex = findMemoryType(requirements.memoryTypeBits, properties);

  MI_VERIFY_VKCMD(vkAllocateMemory(*_device, &allocInfo, nullptr, &_memory));

  vkBindBufferMemory(*_device, _buffer, _memory, 0);
}

void Buffer::free() {
  MI_VERIFY(isAllocated());

  vkFreeMemory(*_device, _memory, nullptr);

  _memory = VK_NULL_HANDLE;
  _mappedMemory = nullptr;
}

void* Buffer::map() {
  return map(0, size());
}

void* Buffer::map(VkDeviceSize offset, VkDeviceSize size) {
  MI_VERIFY(isAllocated());
  MI_VERIFY(!isMapped());

  vkMapMemory(*_device, _memory, offset, size, 0, &_mappedMemory);
  return _mappedMemory;
}

void Buffer::unmap() {
  MI_VERIFY(isMapped());
  vkUnmapMemory(*_device, _memory);
  _mappedMemory = nullptr;
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
