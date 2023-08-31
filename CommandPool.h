#pragma once

#include <vulkan/vulkan.h>

namespace Vulkan {

class Device;

class CommandPool {
public:
  CommandPool() = default;
  CommandPool(const Device& device, uint32_t queueFamilyIndex);
  ~CommandPool();

  void init(const Device& device, uint32_t queueFamilyIndex);
  void release();

  operator VkCommandPool() const { return _pool; }

private:
  const Device* _device = nullptr;

  VkCommandPool _pool = VK_NULL_HANDLE;
};

} //namespace Vulkan