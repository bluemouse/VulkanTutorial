#pragma once

#include <vulkan/vulkan.h>

#include "helpers_vulkan.h"

NAMESPACE_VULKAN_BEGIN

class Device;

class Buffer {
 public:
  Buffer() = default;
  Buffer(const Device& device, size_t size, VkBufferUsageFlags usage,
         VkMemoryPropertyFlags properties);
  virtual ~Buffer();

  // Transfer the ownership from `rhs` to `this`
  Buffer(Buffer&& rhs) noexcept;
  Buffer& operator=(Buffer&& rhs) noexcept(false);

  void allocate(const Device& device, size_t size, VkBufferUsageFlags usage,
                VkMemoryPropertyFlags properties);
  void free();

  void* map();
  void* map(size_t offset, size_t size);
  void unmap();

  operator VkBuffer() const { return _buffer; }
  VkDeviceMemory memory() const { return _memory; }

  size_t size() const { return _size; }
  bool isAllocated() const { return _buffer != VK_NULL_HANDLE; }

 private:
  uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;
  void moveFrom(Buffer& rhs);

 protected:
  const Device* _device = nullptr;

  VkBuffer _buffer = VK_NULL_HANDLE;
  VkDeviceMemory _memory = VK_NULL_HANDLE;

  size_t _size = 0; // in bytes
};

NAMESPACE_VULKAN_END