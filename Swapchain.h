#pragma once

#include <vulkan/vulkan.h>

#include <vector>
#include <functional>

#include "Device.h"

namespace Vulkan {

class Swapchain {
public:
  Swapchain() = default;
  Swapchain(const Device& device, VkSurfaceKHR surface,
            const std::function<VkSurfaceFormatKHR(const std::vector<VkSurfaceFormatKHR>&)>& chooseSwapSurfaceFormat,
            const std::function<VkPresentModeKHR(const std::vector<VkPresentModeKHR>&)>& chooseSwapPresentMode,
            const std::function<VkExtent2D(const VkSurfaceCapabilitiesKHR&)>& chooseSwapExtent);
  ~Swapchain();

  void init(const Device& device, VkSurfaceKHR surface,
            const std::function<VkSurfaceFormatKHR(const std::vector<VkSurfaceFormatKHR>&)>& chooseSwapSurfaceFormat,
            const std::function<VkPresentModeKHR(const std::vector<VkPresentModeKHR>&)>& chooseSwapPresentMode,
            const std::function<VkExtent2D(const VkSurfaceCapabilitiesKHR&)>& chooseSwapExtent);
  void release();

  operator VkSwapchainKHR() const { return _swapchain; }

  const std::vector<VkImage>& images() const { return _images; }
  VkFormat imageFormat() const { return _imageFormat; }
  VkExtent2D extent() const { return _extent; }

  const Device& device() const { return *_device; }

private:
  const Device* _device = nullptr;

  VkSwapchainKHR _swapchain = VK_NULL_HANDLE;

  std::vector<VkImage> _images;
  VkFormat _imageFormat;
  VkExtent2D _extent;
};

} // namespace Vulkan