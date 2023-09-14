#include "Semaphore.h"

#include "Device.h"

NAMESPACE_VULKAN_BEGIN

Semaphore::Semaphore(const Device& device) {
  create(device);
}

Semaphore::~Semaphore() {
  if (_semaphore != VK_NULL_HANDLE) {
    destroy();
  }
}

Semaphore::Semaphore(Semaphore&& rhs) noexcept {
  moveFrom(rhs);
}

Semaphore& Semaphore::operator=(Semaphore&& rhs) noexcept(false) {
  if (this == &rhs) {
    moveFrom(rhs);
  }
  return *this;
}

void Semaphore::create(const Device& device) {
  MI_VERIFY(_semaphore == VK_NULL_HANDLE);
  _device = &device;

  VkSemaphoreCreateInfo semaphoreInfo{};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  MI_VERIFY_VKCMD(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &_semaphore));
}

void Semaphore::destroy() {
  MI_VERIFY(_semaphore != VK_NULL_HANDLE);
  vkDestroySemaphore(*_device, _semaphore, nullptr);

  _semaphore = VK_NULL_HANDLE;
  _device = nullptr;
}

void Semaphore::moveFrom(Semaphore& rhs) {
  MI_VERIFY(_semaphore == VK_NULL_HANDLE);
  _semaphore = rhs._semaphore;
  _device = rhs._device;

  rhs._semaphore = VK_NULL_HANDLE;
  rhs._device = nullptr;
}

NAMESPACE_VULKAN_END
