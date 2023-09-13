#include "PhysicalDevice.h"

#include <vector>

#include "Instance.h"

NAMESPACE_VULKAN_BEGIN

PhysicalDevice::PhysicalDevice(const Instance& instance,
                               const IsDeviceSuitablePredicate& isDeviceSuitable) {
  instantiate(instance, isDeviceSuitable);
}

PhysicalDevice::~PhysicalDevice() {
  reset();
}

void PhysicalDevice::instantiate(const Instance& instance,
                                 const IsDeviceSuitablePredicate& isDeviceSuitable) {
  MI_VERIFY(_device == VK_NULL_HANDLE);

  _instance = &instance;

  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

  MI_VERIFY_MSG(deviceCount > 0, "Error: failed to find physical Vulkan devices!");

  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

  for (const auto& device : devices) {
    if (isDeviceSuitable(device)) {
      _device = device;
      break;
    }
  }

  MI_VERIFY_VKHANDLE(_device);

  _queueFamilies = findQueueFamilies(_device, _instance->surface());
}

void PhysicalDevice::reset() {
  _device = VK_NULL_HANDLE;
  _queueFamilies = {};
  _instance = nullptr;
}

PhysicalDevice::QueueFamilies PhysicalDevice::findQueueFamilies(VkPhysicalDevice device,
                                                                VkSurfaceKHR surface) {
  QueueFamilies queueFamilies;

  uint32_t count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &count, nullptr);
  if (count > 0) {
    std::vector<VkQueueFamilyProperties> props(count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &count, props.data());

    for (size_t i = 0; i < props.size(); ++i) {
      if (!queueFamilies.graphics && ((props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0u)) {
        queueFamilies.graphics = i;
      }
      if (!queueFamilies.compute && ((props[i].queueFlags & VK_QUEUE_COMPUTE_BIT) != 0u)) {
        queueFamilies.compute = i;
      }
      if (!queueFamilies.transfer && ((props[i].queueFlags & VK_QUEUE_TRANSFER_BIT) != 0u)) {
        queueFamilies.transfer = i;
      }

      if (!queueFamilies.present) {
        VkBool32 presentSupport = 0u;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
        if (presentSupport != 0u) {
          queueFamilies.present = i;
        }
      }
    }
  }

  return queueFamilies;
}

PhysicalDevice::SwapChainSupportDetails PhysicalDevice::querySwapChainSupport(
    VkPhysicalDevice device, VkSurfaceKHR surface) {
  SwapChainSupportDetails details;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

  uint32_t formatCount = 0;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
  if (formatCount > 0) {
    details.formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
  }

  uint32_t presentModeCount = 0;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
  if (presentModeCount > 0) {
    details.presentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount,
                                              details.presentModes.data());
  }

  return details;
}

NAMESPACE_VULKAN_END
