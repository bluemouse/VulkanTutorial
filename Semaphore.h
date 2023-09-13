#pragma once

#include <vulkan/vulkan.h>

#include "helpers_vulkan.h"

NAMESPACE_VULKAN_BEGIN

class Device;

class Semaphore {
 public:
  Semaphore() = default;
  explicit Semaphore(const Device& device);
  ~Semaphore();

  void create(const Device& device);
  void destroy();

  operator VkSemaphore() const { return _semaphore; }
  operator const VkSemaphore*() const { return &_semaphore; }

 private:
  VkSemaphore _semaphore = VK_NULL_HANDLE;

  const Device* _device = nullptr;
};

NAMESPACE_VULKAN_END