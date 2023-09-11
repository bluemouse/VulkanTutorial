#pragma once

#include <vulkan/vulkan.h>

#include "PhysicalDevice.h"

namespace Vulkan {

class Device {
 public:
  Device() = default;
  Device(const PhysicalDevice& physicalDevice, std::vector<const char*> extensions = {});
  ~Device();

  void create(const PhysicalDevice& physicalDevice, std::vector<const char*> extensions = {});
  void destroy();

  operator VkDevice() const { return _device; }

  const Instance& instance() const { return physicalDevice().instance(); }
  const PhysicalDevice& physicalDevice() const { return *_physicalDevice; }

  VkQueue graphicsQueue() const { return _graphicsQueue; }
  VkQueue presentQueue() const { return _presentQueue; }

 private:
  VkDevice _device = VK_NULL_HANDLE;

  VkQueue _graphicsQueue = VK_NULL_HANDLE;
  VkQueue _presentQueue = VK_NULL_HANDLE;

  const PhysicalDevice* _physicalDevice = nullptr;
};

} // namespace Vulkan