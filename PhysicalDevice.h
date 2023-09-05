#pragma once

#include <functional>
#include <optional>

#include <vulkan/vulkan.h>

namespace Vulkan {

class Instance;

class PhysicalDevice {
public:
  struct QueueFamilies {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
      return graphicsFamily.has_value() && presentFamily.has_value();
    }
  };

  struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
  };

public:
  PhysicalDevice() = default;
  PhysicalDevice(const Instance& instance,
                 const std::function<bool(VkPhysicalDevice)>& isDeviceSuitable =
                   [](VkPhysicalDevice) -> bool { return true; });
  ~PhysicalDevice();

  void create(const Instance& instance,
            const std::function<bool(VkPhysicalDevice)>& isDeviceSuitable =
              [](VkPhysicalDevice) -> bool { return true; });
  void initQueueFamilies();
  void destroy();

  operator VkPhysicalDevice() const { return _device; }

  uint32_t graphicsFamilyIndex() const { return _queueFamilies.graphicsFamily.value(); }
  uint32_t presentFamilyIndex() const { return _queueFamilies.presentFamily.value(); }

  const Instance& instance() const { return *_instance; }

  static QueueFamilies findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
  static SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);

private:
  bool isDeviceSuitable(VkPhysicalDevice device);

private:
  const Instance* _instance = nullptr;

  VkPhysicalDevice _device = VK_NULL_HANDLE;

  QueueFamilies _queueFamilies;
};

} // namespace Vulkan