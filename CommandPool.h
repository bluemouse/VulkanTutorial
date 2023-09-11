#pragma once

#include <vulkan/vulkan.h>

namespace Vulkan {

class Device;

class CommandPool {
 public:
  CommandPool() = default;
  CommandPool(const Device& device, uint32_t queueFamilyIndex);
  ~CommandPool();

  void create(const Device& device, uint32_t queueFamilyIndex);
  void destroy();

  operator VkCommandPool() const { return _pool; }
  VkQueue queue() const { return _queue; }

  const Device& device() const { return *_device; }

 private:
  VkCommandPool _pool = VK_NULL_HANDLE;
  VkQueue _queue = VK_NULL_HANDLE;

  const Device* _device = nullptr;
};

} // namespace Vulkan