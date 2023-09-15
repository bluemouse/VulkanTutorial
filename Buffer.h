#pragma once

#include <vulkan/vulkan.h>

#include <functional>

#include "helpers_vulkan.h"

NAMESPACE_VULKAN_BEGIN

class Device;

class Buffer {
 public:
  Buffer() = default;
  Buffer(const Device& device,
         VkDeviceSize size,
         VkBufferUsageFlags usage);
  Buffer(const Device& device,
         VkDeviceSize size,
         VkBufferUsageFlags usage,
         VkMemoryPropertyFlags properties);
  virtual ~Buffer();

  // Transfer the ownership from `rhs` to `this`
  Buffer(Buffer&& rhs) noexcept;
  Buffer& operator=(Buffer&& rhs) noexcept(false);

  using BufferCreateInfoOverride = std::function<void(VkBufferCreateInfo*)>;
  void create(const Device& device,
              VkDeviceSize size,
              VkBufferUsageFlags usage,
              const BufferCreateInfoOverride& override = {});
  void create(const Device& device,
              VkDeviceSize size,
              VkBufferUsageFlags usage,
              VkMemoryPropertyFlags properties,
              const BufferCreateInfoOverride& override = {});
  void destroy();

  void allocate(VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  void free();

  void* map();
  void* map(VkDeviceSize offset, VkDeviceSize size);
  void unmap();

  operator VkBuffer() const { return _buffer; }
  [[nodiscard]] VkDeviceMemory memory() const { return _memory; }

  [[nodiscard]] VkDeviceSize size() const { return _size; }
  [[nodiscard]] bool isAllocated() const {
    return _buffer != VK_NULL_HANDLE && _memory != VK_NULL_HANDLE;
  }
  [[nodiscard]] bool isMapped() const { return _mappedMemory != nullptr; }

 private:
  uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;
  void moveFrom(Buffer& rhs);

 protected:
  VkBuffer _buffer = VK_NULL_HANDLE;
  VkDeviceMemory _memory = VK_NULL_HANDLE;

  VkDeviceSize _size = 0; // in bytes

  void* _mappedMemory = nullptr;

  const Device* _device = nullptr;
};

NAMESPACE_VULKAN_END