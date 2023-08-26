#include "PhysicalDevice.h"

#include <vector>
#include <stdexcept>

using namespace Vulkan;

PhysicalDevice::PhysicalDevice(
    const Instance& instance,
    const std::function<bool(VkPhysicalDevice)>& isDeviceSuitable) {
  init(instance, isDeviceSuitable);
}
PhysicalDevice::~PhysicalDevice() {
  release();
}

void PhysicalDevice::init(
    const Instance& instance,
    const std::function<bool(VkPhysicalDevice)>& isDeviceSuitable) {
  if (_device != VK_NULL_HANDLE) {
    throw std::runtime_error("Vulkan physical device has been initialized already!");
  }

  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

  if (deviceCount == 0) {
    throw std::runtime_error("Failed to find devices with Vulkan support!");
  }

  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

  for (const auto &device : devices) {
    if (isDeviceSuitable(device)) {
      _device = device;
      break;
    }
  }

  if (_device == VK_NULL_HANDLE) {
    throw std::runtime_error("Failed to find a suitable Vulkan device!");
  }
}

void PhysicalDevice::release() {
  // Phyiscal device is not created or allocated so nothing to do here
}