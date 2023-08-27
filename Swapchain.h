#pragma once

#include <vulkan/vulkan.h>

#include <vector>
#include <functional>

#include "Device.h"
#include "Image.h"
#include "ImageView.h"

namespace Vulkan {

class Swapchain {
public:
  Swapchain() = default;
  Swapchain(const Device& device,
            const std::function<VkSurfaceFormatKHR(const std::vector<VkSurfaceFormatKHR>&)>& chooseSwapSurfaceFormat,
            const std::function<VkPresentModeKHR(const std::vector<VkPresentModeKHR>&)>& chooseSwapPresentMode,
            const std::function<VkExtent2D(const VkSurfaceCapabilitiesKHR&)>& chooseSwapExtent);
  ~Swapchain();

  void init(const Device& device,
            const std::function<VkSurfaceFormatKHR(const std::vector<VkSurfaceFormatKHR>&)>& chooseSwapSurfaceFormat,
            const std::function<VkPresentModeKHR(const std::vector<VkPresentModeKHR>&)>& chooseSwapPresentMode,
            const std::function<VkExtent2D(const VkSurfaceCapabilitiesKHR&)>& chooseSwapExtent);
  void release();

  operator VkSwapchainKHR() const { return _swapchain; }

  const std::vector<Image>& images() const { return _images; }
  VkFormat imageFormat() const { return _images[0].format(); }
  VkExtent2D imageExtent() const { return _images[0].extent(); }

  const std::vector<ImageView>& imageViews() const { return _imageViews; }

  const Device& device() const { return *_device; }

private:
  const Device* _device = nullptr;

  VkSwapchainKHR _swapchain = VK_NULL_HANDLE;

  std::vector<Image> _images;
  std::vector<ImageView> _imageViews;
};

} // namespace Vulkan