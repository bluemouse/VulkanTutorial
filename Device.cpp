#include "Device.h"
#include "Instance.h"

#include "helpers_vulkan.h"

#include <set>

using namespace Vulkan;

Device::Device(const PhysicalDevice &physicalDevice, std::vector<const char*> extensions) {
  create(physicalDevice, extensions);
}

Device::~Device() {
  if (_device != VK_NULL_HANDLE) {
    destroy();
  }
}

void Device::create(const PhysicalDevice& physicalDevice, std::vector<const char*> extensions) {
  if (_device != VK_NULL_HANDLE) {
    throw std::runtime_error("Vulkan logical device has been initialized already!");
  }
  _physicalDevice = &physicalDevice;

  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  float queuePriority = 1.0f;

  std::set<uint32_t> uniqueQueueFamilies = {
      physicalDevice.graphicsFamilyIndex(),
      physicalDevice.presentFamilyIndex()
  };
  for (uint32_t queueFamily : uniqueQueueFamilies) {
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = queueFamily;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;
    queueCreateInfos.push_back(queueCreateInfo);
  }

  VkPhysicalDeviceFeatures deviceFeatures{};
  deviceFeatures.samplerAnisotropy = VK_TRUE;

  VkDeviceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

  createInfo.queueCreateInfoCount =
      static_cast<uint32_t>(queueCreateInfos.size());
  createInfo.pQueueCreateInfos = queueCreateInfos.data();

  createInfo.pEnabledFeatures = &deviceFeatures;

  createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
  createInfo.ppEnabledExtensionNames = extensions.data();

  const auto& instance = physicalDevice.instance();
  if (instance.isValidationLayersEnabled()) {
    createInfo.enabledLayerCount = static_cast<uint32_t>(instance.layers().size());
    createInfo.ppEnabledLayerNames = instance.layers().data();
  } else {
    createInfo.enabledLayerCount = 0;
  }

  MI_VERIFY_VKCMD(vkCreateDevice(physicalDevice, &createInfo, nullptr, &_device));

  vkGetDeviceQueue(_device, physicalDevice.graphicsFamilyIndex(), 0, &_graphicsQueue);
  vkGetDeviceQueue(_device, physicalDevice.presentFamilyIndex(), 0, &_presentQueue);
}

void Device::destroy() {
  if (_device == VK_NULL_HANDLE) {
    throw std::runtime_error("Vulkan null logical device cannot be released!");
  }
  vkDestroyDevice(_device, nullptr);
  _device = VK_NULL_HANDLE;
  _physicalDevice = nullptr;
}