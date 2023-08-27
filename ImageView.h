#pragma once

#include <vulkan/vulkan.h>

#include "Device.h"
#include "Image.h"

namespace Vulkan {

class ImageView {
public:
  ImageView(const Image& image, const Device& device);
  ~ImageView();

  operator VkImageView() const { return _view; }

private:
  const Image* _image;
  const Device* _device;

  VkImageView _view;
};

} //namespace Vulkan