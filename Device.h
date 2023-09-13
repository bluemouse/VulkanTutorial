#pragma once

#include <vulkan/vulkan.h>

#include <vector>

#include "PhysicalDevice.h"
#include "helpers_vulkan.h"

NAMESPACE_VULKAN_BEGIN

class Device {
 public:
  Device() = default;
  Device(const PhysicalDevice& physicalDevice, std::vector<const char*> extensions = {});
  ~Device();

  void create(const PhysicalDevice& physicalDevice, std::vector<const char*> extensions = {});
  void destroy();

  operator VkDevice() const { return _device; }

  [[nodiscard]] const Instance& instance() const { return physicalDevice().instance(); }
  [[nodiscard]] const PhysicalDevice& physicalDevice() const { return *_physicalDevice; }

  [[nodiscard]] VkQueue graphicsQueue() const { return _graphicsQueue; }
  [[nodiscard]] VkQueue presentQueue() const { return _presentQueue; }

 private:
  VkDevice _device = VK_NULL_HANDLE;

  VkQueue _graphicsQueue = VK_NULL_HANDLE;
  VkQueue _presentQueue = VK_NULL_HANDLE;

  const PhysicalDevice* _physicalDevice = nullptr;
};

NAMESPACE_VULKAN_END