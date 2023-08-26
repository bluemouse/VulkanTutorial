#include "PhysicalDevice.h"

#include <stdexcept>
#include <vector>

using namespace Vulkan;

PhysicalDevice::PhysicalDevice(
    const Instance &instance,
    const std::function<bool(VkPhysicalDevice)> &isDeviceSuitable) {
  init(instance, isDeviceSuitable);
}
PhysicalDevice::~PhysicalDevice() { release(); }

void PhysicalDevice::init(
    const Instance &instance,
    const std::function<bool(VkPhysicalDevice)> &isDeviceSuitable) {
  if (_device != VK_NULL_HANDLE) {
    throw std::runtime_error(
        "Vulkan physical device has been initialized already!");
  }

  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

  if (deviceCount == 0) {
    throw std::runtime_error("Failed to find devices with Vulkan support!");
  }

  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

  for (const auto& device : devices) {
    if (isDeviceSuitable(device)) {
      _device = device;
      break;
    }
  }

  if (_device == VK_NULL_HANDLE) {
    throw std::runtime_error("Failed to find a suitable Vulkan device!");
  }
}

void PhysicalDevice::initQueueFamilies(VkSurfaceKHR surface) {
  _queueFamilies = findQueueFamilies(_device, surface);
}

void PhysicalDevice::release() {
  // Phyiscal device is not created or allocated so nothing to do here
}

PhysicalDevice::QueueFamilies
PhysicalDevice::findQueueFamilies(VkPhysicalDevice device,
                                  VkSurfaceKHR surface) {
  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilyProps(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
                                           queueFamilyProps.data());

  PhysicalDevice::QueueFamilies queueFamilies;
  for (size_t i = 0; i < queueFamilyProps.size(); ++i) {
    if (queueFamilyProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      queueFamilies.graphicsFamily = i;
    }

    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
    if (presentSupport) {
      queueFamilies.presentFamily = i;
    }

    if (queueFamilies.isComplete()) {
      break;
    }
  }

  return queueFamilies;
}
