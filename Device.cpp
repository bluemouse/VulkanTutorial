#include "Device.h"

#include <set>
#include <utility>

#include "Instance.h"

NAMESPACE_VULKAN_BEGIN

Device::Device(const PhysicalDevice& physicalDevice, std::vector<const char*> extensions) {
  create(physicalDevice, std::move(extensions));
}

Device::~Device() {
  if (isCreated()) {
    destroy();
  }
}

Device::Device(Device&& rhs) noexcept {
  moveFrom(rhs);
}

Device& Device::operator=(Device&& rhs) noexcept(false) {
  if (this != &rhs) {
    moveFrom(rhs);
  }
  return *this;
}

void Device::moveFrom(Device& rhs) {
  MI_VERIFY(!isCreated());
  _device = rhs._device;
  _graphicsQueue = rhs._graphicsQueue;
  _presentQueue = rhs._presentQueue;
  _physicalDevice = rhs._physicalDevice;

  rhs._device = VK_NULL_HANDLE;
  rhs._graphicsQueue = VK_NULL_HANDLE;
  rhs._presentQueue = VK_NULL_HANDLE;
  rhs._physicalDevice = nullptr;
}

void Device::create(const PhysicalDevice& physicalDevice, std::vector<const char*> extensions) {
  MI_VERIFY(!isCreated());
  _physicalDevice = &physicalDevice;

  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  float queuePriority = 1.0f;

  std::set<uint32_t> uniqueQueueFamilies = {physicalDevice.queueFamilies().graphicsIndex(),
                                            physicalDevice.queueFamilies().presentIndex()};
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

  createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
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

  vkGetDeviceQueue(_device, physicalDevice.queueFamilies().graphicsIndex(), 0, &_graphicsQueue);
  vkGetDeviceQueue(_device, physicalDevice.queueFamilies().presentIndex(), 0, &_presentQueue);
}

void Device::destroy() {
  MI_VERIFY(isCreated());
  vkDestroyDevice(_device, nullptr);
  _device = VK_NULL_HANDLE;
  _physicalDevice = nullptr;
}

NAMESPACE_VULKAN_END
