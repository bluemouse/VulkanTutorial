#pragma once

#include <functional>
#include <optional>

#include <vulkan/vulkan.h>

#include "Instance.h"


namespace Vulkan {

class PhysicalDevice {
  public:
    struct QueueFamilies {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
      return graphicsFamily.has_value() && presentFamily.has_value();
    }
  };

public:
  PhysicalDevice() = default;
  PhysicalDevice(const Instance& instance,
                 const std::function<bool(VkPhysicalDevice)>& isDeviceSuitable =
                   [](VkPhysicalDevice) -> bool { return true; });
  ~PhysicalDevice();

  void init(const Instance& instance,
            const std::function<bool(VkPhysicalDevice)>& isDeviceSuitable =
              [](VkPhysicalDevice) -> bool { return true; });
  void initQueueFamilies(VkSurfaceKHR surface);
  void release();

  operator VkPhysicalDevice() const { return _device; }

  uint32_t graphicsFamilyIndex() const { return _queueFamilies.graphicsFamily.value(); }
  uint32_t presentFamilyIndex() const { return _queueFamilies.presentFamily.value(); }

  static QueueFamilies findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);

private:
  bool isDeviceSuitable(VkPhysicalDevice device);

private:
  VkPhysicalDevice _device = VK_NULL_HANDLE;

  QueueFamilies _queueFamilies;
};

} // namespace Vulkan