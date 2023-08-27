#include "ImageView.h"

#include "helpers.h"

using namespace Vulkan;

ImageView::ImageView(const Image& image, const Device& device)
  : _image(&image), _device(&device) {
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

ImageView::~ImageView() {
  vkDestroyImageView(*_device, _view, nullptr);
}
