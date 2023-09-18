#include "Swapchain.h"

#include "Device.h"
#include "Surface.h"
#include "Instance.h"
#include "PhysicalDevice.h"
#include "RenderPass.h"

NAMESPACE_VULKAN_BEGIN

Swapchain::Swapchain(
    const Device& device,
    const Surface& surface,
    const std::function<VkSurfaceFormatKHR(const std::vector<VkSurfaceFormatKHR>&)>&
        chooseSwapSurfaceFormat,
    const std::function<VkPresentModeKHR(const std::vector<VkPresentModeKHR>&)>&
        chooseSwapPresentMode,
    const std::function<VkExtent2D(const VkSurfaceCapabilitiesKHR&)>& chooseSwapExtent) {
  create(device, surface, chooseSwapSurfaceFormat, chooseSwapPresentMode, chooseSwapExtent);
}

Swapchain::~Swapchain() {
  if (isCreated()) {
    destroy();
  }
}

void Swapchain::create(
    const Device& device,
    const Surface& surface,
    const std::function<VkSurfaceFormatKHR(const std::vector<VkSurfaceFormatKHR>&)>&
        chooseSwapSurfaceFormat,
    const std::function<VkPresentModeKHR(const std::vector<VkPresentModeKHR>&)>&
        chooseSwapPresentMode,
    const std::function<VkExtent2D(const VkSurfaceCapabilitiesKHR&)>& chooseSwapExtent) {
  MI_VERIFY(!isCreated());
  _device = &device;

  const auto& physicalDevice = device.physicalDevice();
  PhysicalDevice::SwapChainSupportDetails swapChainSupport =
      PhysicalDevice::querySwapChainSupport(physicalDevice, surface);

  VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
  VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
  VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

  uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
  if (swapChainSupport.capabilities.maxImageCount > 0 &&
      imageCount > swapChainSupport.capabilities.maxImageCount) {
    imageCount = swapChainSupport.capabilities.maxImageCount;
  }

  VkSwapchainCreateInfoKHR createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.surface = surface;

  createInfo.minImageCount = imageCount;
  createInfo.imageFormat = surfaceFormat.format;
  createInfo.imageColorSpace = surfaceFormat.colorSpace;
  createInfo.imageExtent = extent;
  createInfo.imageArrayLayers = 1;
  createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  std::array<uint32_t, 2> queueFamilyIndices = {physicalDevice.queueFamilies().graphicsIndex(),
                                                physicalDevice.queueFamilies().presentIndex()};

  if (queueFamilyIndices[0] != queueFamilyIndices[1]) {
    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    createInfo.queueFamilyIndexCount = 2;
    createInfo.pQueueFamilyIndices = queueFamilyIndices.data();
  } else {
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0;     // Optional
    createInfo.pQueueFamilyIndices = nullptr; // Optional
  }

  createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  createInfo.presentMode = presentMode;
  createInfo.clipped = VK_TRUE;

  MI_VERIFY_VKCMD(vkCreateSwapchainKHR(device, &createInfo, nullptr, &_swapchain));

  vkGetSwapchainImagesKHR(device, _swapchain, &imageCount, nullptr);
  std::vector<VkImage> imgs(imageCount);
  vkGetSwapchainImagesKHR(device, _swapchain, &imageCount, imgs.data());

  // We need to reserve the space first to avoid resizing (which triggers the destructor)
  _images.reserve(imageCount);
  for (auto& img : imgs) {
    _images.emplace_back(img, surfaceFormat.format, extent);
  }

  // We need to reserve the space first to avoid resizing (which triggers the destructor)
  _imageViews.reserve(imageCount);
  for (auto& image : _images) {
    _imageViews.emplace_back(device, image);
  }
}

void Swapchain::createFramebuffers(const RenderPass& renderPass) {
  MI_VERIFY(isCreated());

  // Reserve enough space to avoid resizing that can trigger destructing
  _framebuffers.reserve(_imageViews.size());
  for (auto& view : _imageViews) {
    _framebuffers.emplace_back(device(), renderPass, view);
  }
}

void Swapchain::destroy() {
  MI_VERIFY(isCreated());

  // Be careful about changing the release order.
  _framebuffers.clear();
  _imageViews.clear();
  _images.clear();

  vkDestroySwapchainKHR(device(), _swapchain, nullptr);

  _swapchain = VK_NULL_HANDLE;
  _device = nullptr;
}

NAMESPACE_VULKAN_END
