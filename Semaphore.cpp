#include "Semaphore.h"

#include "Device.h"
#include "helpers_vulkan.h"

using namespace Vulkan;

Semaphore::Semaphore(const Device& device) {
  create(device);
}

Semaphore::~Semaphore() {
  if (_semaphore != VK_NULL_HANDLE) {
    destroy();
  }
}

void Semaphore::create(const Device& device) {
  if (_semaphore != VK_NULL_HANDLE) {
    throw std::runtime_error("Vulkan samaphore has been initialized already!");
  }
  _device = &device;

  VkSemaphoreCreateInfo semaphoreInfo{};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  MI_VERIFY_VKCMD(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &_semaphore));
}

void Semaphore::destroy() {
  if (_semaphore == VK_NULL_HANDLE) {
    throw std::runtime_error("Vulkan null semaphore cannot be released!");
  }
  vkDestroySemaphore(*_device, _semaphore, nullptr);

  _semaphore = VK_NULL_HANDLE;
  _device = nullptr;
}
