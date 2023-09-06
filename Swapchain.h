#pragma once

#include <vulkan/vulkan.h>

#include <vector>
#include <functional>

#include "Image.h"
#include "ImageView.h"
#include "Framebuffer.h"

namespace Vulkan {

class Device;
class RenderPass;
class Swapchain {
public:
  Swapchain() = default;
  Swapchain(const Device& device,
            const std::function<VkSurfaceFormatKHR(const std::vector<VkSurfaceFormatKHR>&)>& chooseSwapSurfaceFormat,
            const std::function<VkPresentModeKHR(const std::vector<VkPresentModeKHR>&)>& chooseSwapPresentMode,
            const std::function<VkExtent2D(const VkSurfaceCapabilitiesKHR&)>& chooseSwapExtent);
  ~Swapchain();

  void create(const Device& device,
            const std::function<VkSurfaceFormatKHR(const std::vector<VkSurfaceFormatKHR>&)>& chooseSwapSurfaceFormat,
            const std::function<VkPresentModeKHR(const std::vector<VkPresentModeKHR>&)>& chooseSwapPresentMode,
            const std::function<VkExtent2D(const VkSurfaceCapabilitiesKHR&)>& chooseSwapExtent);
  void createFramebuffers(const RenderPass& renderPass);
  void destroy();

  operator VkSwapchainKHR() const { return _swapchain; }

  const std::vector<Image>& images() const { return _images; }
  VkFormat imageFormat() const { return _images[0].format(); }
  VkExtent2D imageExtent() const { return _images[0].extent(); }

  const std::vector<ImageView>& imageViews() const { return _imageViews; }
  const ImageView& imageView(size_t i) const { return _imageViews[i]; }

  const std::vector<Framebuffer>& framebuffers() const { return _framebuffers; }
  const Framebuffer& framebuffer(size_t i) const { return _framebuffers[i]; }

  const Device& device() const { return *_device; }

private:
  VkSwapchainKHR _swapchain = VK_NULL_HANDLE;

  std::vector<Image> _images;
  std::vector<ImageView> _imageViews;
  std::vector<Framebuffer> _framebuffers;

  const Device* _device = nullptr;
};

} // namespace Vulkan