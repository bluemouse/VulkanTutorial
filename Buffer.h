#pragma once

#include <vulkan/vulkan.h>

namespace Vulkan {

class Device;

class Buffer {
public:
  Buffer() = default;
  Buffer(const Device& device,
         VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
  virtual ~Buffer();

  // Transfer the ownership from `rhs` to `this`
  Buffer(const Buffer& rhs);
  Buffer& operator=(const Buffer& rhs);

  void allocate(const Device& device,
                VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
  void release();

  operator VkBuffer() const { return _buffer; }
  VkDeviceMemory memory() const { return _memory; }

  bool isAllocated() const { return _buffer != VK_NULL_HANDLE; }

private:
  uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;
  void moveFrom(Buffer& rhs);

protected:
  const Device* _device;

  VkBuffer _buffer = VK_NULL_HANDLE;
  VkDeviceMemory _memory = VK_NULL_HANDLE;
};

} //namespace Vulkan