#include "Surface.h"

#include "Instance.h"

NAMESPACE_VULKAN_BEGIN

Surface::Surface(const Instance& instance, VkSurfaceKHR surface) {
  create(instance, surface);
}

Surface::~Surface() {
  if (isCreated()) {
    destroy();
  }
}

void Surface::create(const Instance& instance, VkSurfaceKHR surface) {
  MI_VERIFY(!isCreated());
  _instance = &instance;

  _surface = surface;
}

void Surface::destroy() {
  MI_VERIFY(isCreated());

  vkDestroySurfaceKHR(*_instance, _surface, nullptr);

  _surface = VK_NULL_HANDLE;
  _instance = nullptr;
}

NAMESPACE_VULKAN_END
