#pragma once

#include <vulkan/vulkan.h>

namespace Vulkan {

class Device;
class Image;

class ImageView {
public:
  ImageView(const Device& device, const Image& image);
  ~ImageView();

  operator VkImageView() const { return _view; }

private:
  const Device* _device;
  const Image* _image;

  VkImageView _view;
};

} //namespace Vulkan