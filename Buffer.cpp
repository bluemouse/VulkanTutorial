#include "Buffer.h"

#include "Device.h"
#include "DeviceMemory.h"
#include "helpers_vulkan.h"

namespace Vulkan {

Buffer::Buffer(const Device& device,
               VkDeviceSize size,
               VkBufferUsageFlags usage,
               const BufferCreateInfoOverride& override) {
  create(device, size, usage, override);
}

Buffer::Buffer(const Device& device,
               VkDeviceSize size,
               VkBufferUsageFlags usage,
               VkMemoryPropertyFlags properties,
               const BufferCreateInfoOverride& override)
    : Buffer(device, size, usage, override) {
  allocate(properties);
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
  MI_VERIFY(!isCreated());
  _buffer = rhs._buffer;
  _size = rhs._size;
  _memory = rhs._memory;
  _device = rhs._device;

  rhs._buffer = VK_NULL_HANDLE;
  rhs._size = 0;
  rhs._memory = nullptr;
  rhs._device = nullptr;
}

void Buffer::create(const Device& device,
                    VkDeviceSize size,
                    VkBufferUsageFlags usage,
                    const BufferCreateInfoOverride& override) {
  MI_VERIFY(!isCreated());
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

void Buffer::destroy() {
  MI_VERIFY(isCreated());

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
  _memory = DeviceMemory::make();

  VkMemoryRequirements requirements;
  vkGetBufferMemoryRequirements(*_device, _buffer, &requirements);
  _memory->allocate(*_device, properties, requirements);

  vkBindBufferMemory(*_device, _buffer, *_memory.get(), 0);
}

void Buffer::free() {
  MI_VERIFY(isAllocated());
  _memory = nullptr;
}

void Buffer::bind(const DeviceMemory::Ptr& memory, VkDeviceSize offset) {
  MI_VERIFY(isCreated());
  MI_VERIFY(memory != _memory);
  if (isAllocated()) {
    free();
  }
  _memory = memory;
  vkBindBufferMemory(*_device, _buffer, *_memory.get(), offset);
}

void* Buffer::map() {
  MI_VERIFY(isAllocated());
  return _memory->map();
}
void* Buffer::map(VkDeviceSize offset, VkDeviceSize size) {
  MI_VERIFY(isAllocated());
  return _memory->map(offset, size);
}

void Buffer::unmap() {
  MI_VERIFY(isAllocated());
  _memory->unmap();
}

} // namespace Vulkan
