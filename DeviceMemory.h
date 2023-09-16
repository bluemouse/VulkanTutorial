#pragma once

#include <vulkan/vulkan.h>

#include <functional>
#include <memory>

#include "helpers_vulkan.h"

NAMESPACE_VULKAN_BEGIN

class Device;

class DeviceMemory {
 public:
  using Ptr = std::shared_ptr<DeviceMemory>;

 public:
  DeviceMemory() = default;
  DeviceMemory(const Device& device,
               VkMemoryPropertyFlags properties,
               const VkMemoryRequirements& requirements);
  virtual ~DeviceMemory();

  static Ptr make() { return std::make_shared<DeviceMemory>(); }

  // Transfer the ownership from `rhs` to `this`
  DeviceMemory(DeviceMemory&& rhs) noexcept;
  DeviceMemory& operator=(DeviceMemory&& rhs) noexcept(false);

  void allocate(const Device& device,
                VkMemoryPropertyFlags properties,
                const VkMemoryRequirements& requirements);
  void free();

  void* map() { return map(0, _size); }
  void* map(VkDeviceSize offset, VkDeviceSize size);
  void unmap();

  operator VkDeviceMemory() const { return _memory; }

  [[nodiscard]] VkDeviceSize size() const { return _size; }
  [[nodiscard]] bool isAllocated() const { return _memory != VK_NULL_HANDLE; }
  [[nodiscard]] bool isMapped() const { return _mappedMemory != nullptr; }

 private:
  uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;
  void moveFrom(DeviceMemory& rhs);

 private:
  VkDeviceMemory _memory = VK_NULL_HANDLE;

  VkDeviceSize _size = 0; // in bytes
  void* _mappedMemory = nullptr;

  const Device* _device = nullptr;
};

NAMESPACE_VULKAN_END