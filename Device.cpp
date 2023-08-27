#include "Device.h"
#include "helpers.h"

#include <set>

using namespace Vulkan;

Device::Device(const PhysicalDevice &physicalDevice, std::vector<const char*> extensions) {
  init(physicalDevice, extensions);
}

Device::~Device() {
  if (_device != VK_NULL_HANDLE) {
    release();
  }
}

void Device::init(const PhysicalDevice& physicalDevice, std::vector<const char*> extensions) {
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
    createInfo.enabledLayerCount =
        static_cast<uint32_t>(instance.validationLayers.size());
    createInfo.ppEnabledLayerNames = instance.validationLayers.data();
  } else {
    createInfo.enabledLayerCount = 0;
  }

  VK_VERIFY(vkCreateDevice(physicalDevice, &createInfo, nullptr, &_device));

  vkGetDeviceQueue(_device, physicalDevice.graphicsFamilyIndex(), 0, &_graphicsQueue);
  vkGetDeviceQueue(_device, physicalDevice.presentFamilyIndex(), 0, &_presentQueue);
}

void Device::release() {
  if (_device == VK_NULL_HANDLE) {
    throw std::runtime_error("Vulkan null logical device cannot be released!");
  }
  vkDestroyDevice(_device, nullptr);
  _device = VK_NULL_HANDLE;
  _physicalDevice = nullptr;
}