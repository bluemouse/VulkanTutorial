#pragma once

#include <functional>
#include <vulkan/vulkan.h>

#include "Instance.h"


namespace Vulkan {

class PhysicalDevice {
public:
  PhysicalDevice() = default;
  PhysicalDevice(const Instance& instance,
                 const std::function<bool(VkPhysicalDevice)>& isDeviceSuitable =
                   [](VkPhysicalDevice) -> bool { return true; });
  ~PhysicalDevice();

  void init(const Instance& instance,
            const std::function<bool(VkPhysicalDevice)>& isDeviceSuitable =
              [](VkPhysicalDevice) -> bool { return true; });
  void release();

  operator VkPhysicalDevice() const { return _device; }

private:
  bool isDeviceSuitable(VkPhysicalDevice device);

private:
  VkPhysicalDevice _device = VK_NULL_HANDLE;
};

} // namespace Vulkan