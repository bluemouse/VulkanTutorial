#pragma once

#include <functional>
#include <optional>

#include <vulkan/vulkan.h>

namespace Vulkan {

class Instance;

class PhysicalDevice {
public:
  struct QueueFamilies {
    std::optional<uint32_t> graphics;
    std::optional<uint32_t> compute;
    std::optional<uint32_t> transfer;
    std::optional<uint32_t> present;

    uint32_t graphicsIndex() const { return graphics.value(); }
    uint32_t computeIndex() const { return compute.value(); }
    uint32_t transferIndex() const { return transfer.value(); }
    uint32_t presentIndex() const { return present.value(); }
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

  void instantiate(const Instance& instance,
                   const std::function<bool(VkPhysicalDevice)>& isDeviceSuitable =
                     [](VkPhysicalDevice) -> bool { return true; });
  void reset();

  operator VkPhysicalDevice() const { return _device; }

  const QueueFamilies& queueFamilies() const { return _queueFamilies; }

  const Instance& instance() const { return *_instance; }

  static QueueFamilies findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
  static SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);

private:
  bool isDeviceSuitable(VkPhysicalDevice device);

private:
  VkPhysicalDevice _device = VK_NULL_HANDLE;
  QueueFamilies _queueFamilies;

  const Instance* _instance = nullptr;
};

} // namespace Vulkan