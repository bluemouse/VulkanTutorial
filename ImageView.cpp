#include "ImageView.h"
#include "Device.h"
#include "Image.h"

#include "helpers.h"

using namespace Vulkan;

ImageView::ImageView(const Device& device, const Image& image) {
  create(device, image);
}

ImageView::~ImageView() {
  if (_view != VK_NULL_HANDLE) {
    destroy();
  }
}

void ImageView::create(const Device& device, const Image& image) {
  if (_view != VK_NULL_HANDLE) {
    throw std::runtime_error("Vulkan image view has been initialized already!");
  }
  _device = &device;
  _image = &image;

  VkImageViewCreateInfo viewInfo{};
  viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  viewInfo.image = image;
  viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  viewInfo.format = image.format();
  viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  viewInfo.subresourceRange.baseMipLevel = 0;
  viewInfo.subresourceRange.levelCount = 1;
  viewInfo.subresourceRange.baseArrayLayer = 0;
  viewInfo.subresourceRange.layerCount = 1;

  VK_VERIFY(vkCreateImageView(*_device, &viewInfo, nullptr, &_view));
}

void ImageView::destroy() {
  if (_view == VK_NULL_HANDLE) {
    throw std::runtime_error("Vulkan null image view cannot be released!");
  }

  vkDestroyImageView(*_device, _view, nullptr);

  _view = VK_NULL_HANDLE;
  _device = nullptr;
  _image = nullptr;
}
