#pragma once

#include <vulkan/vulkan.h>

#include "helpers_vulkan.h"

NAMESPACE_VULKAN_BEGIN

class Device;

class CommandPool {
 public:
  CommandPool() = default;
  CommandPool(const Device& device, uint32_t queueFamilyIndex);
  ~CommandPool();

  void create(const Device& device, uint32_t queueFamilyIndex);
  void destroy();

  operator VkCommandPool() const { return _pool; }
  [[nodiscard]] VkQueue queue() const { return _queue; }

  [[nodiscard]] const Device& device() const { return *_device; }

 private:
  VkCommandPool _pool = VK_NULL_HANDLE;
  VkQueue _queue = VK_NULL_HANDLE;

  const Device* _device = nullptr;
};

NAMESPACE_VULKAN_END