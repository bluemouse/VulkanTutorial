#pragma once

#include <vulkan/vulkan.h>

#include <functional>
#include <optional>
#include <vector>

#include "helpers_vulkan.h"

NAMESPACE_VULKAN_BEGIN

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

  using IsDeviceSuitablePredicate = std::function<bool(VkPhysicalDevice)>;

 public:
  PhysicalDevice() = default;
  PhysicalDevice(const Instance& instance, const IsDeviceSuitablePredicate& isDeviceSuitable);
  ~PhysicalDevice();

  void instantiate(const Instance& instance, const IsDeviceSuitablePredicate& isDeviceSuitable);
  void reset();

  operator VkPhysicalDevice() const { return _device; }

  [[nodiscard]] const QueueFamilies& queueFamilies() const { return _queueFamilies; }

  [[nodiscard]] const Instance& instance() const { return *_instance; }

  [[nodiscard]] static QueueFamilies findQueueFamilies(VkPhysicalDevice device,
                                                       VkSurfaceKHR surface);
  [[nodiscard]] static SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device,
                                                                     VkSurfaceKHR surface);

  // Disable copy and assignment operators
  PhysicalDevice(const PhysicalDevice&) = delete;
  PhysicalDevice& operator=(const PhysicalDevice&) = delete;

  [[nodiscard]] bool isInstantiated() const { return _device != VK_NULL_HANDLE; }

 private:
  [[nodiscard]] bool isDeviceSuitable(VkPhysicalDevice device);

 private:
  VkPhysicalDevice _device = VK_NULL_HANDLE;
  QueueFamilies _queueFamilies;

  const Instance* _instance = nullptr;
};

NAMESPACE_VULKAN_END