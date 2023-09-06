#pragma once

#include <vulkan/vulkan.h>

namespace Vulkan {

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
  const Device* _device;

  VkSemaphore _semaphore = VK_NULL_HANDLE;
};

} //namespace Vulkan