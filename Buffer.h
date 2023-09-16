#pragma once

#include <vulkan/vulkan.h>

#include <functional>

#include "DeviceMemory.h"
#include "helpers_vulkan.h"

NAMESPACE_VULKAN_BEGIN

class Device;

class Buffer {
 public:
  Buffer() = default;
  Buffer(const Device& device, VkDeviceSize size, VkBufferUsageFlags usage);
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

  void bind(DeviceMemory::Ptr memory, VkDeviceSize offset = 0);

  void* map() { return map(0, _size); }
  void* map(VkDeviceSize offset, VkDeviceSize size);
  void unmap();

  operator VkBuffer() const { return _buffer; }
  [[nodiscard]] const DeviceMemory& memory() const { return *_memory.get(); }

  [[nodiscard]] VkDeviceSize size() const { return _size; }
  [[nodiscard]] bool isAllocated() const {
    return _buffer != VK_NULL_HANDLE && (_memory && _memory->isAllocated());
  }
  [[nodiscard]] bool isMapped() const { return _memory->isMapped(); }

 private:
  void moveFrom(Buffer& rhs);

 protected:
  VkBuffer _buffer = VK_NULL_HANDLE;

  VkDeviceSize _size = 0; // in bytes
  DeviceMemory::Ptr _memory;

  const Device* _device = nullptr;
};

NAMESPACE_VULKAN_END