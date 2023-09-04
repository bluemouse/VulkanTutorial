#pragma once

#include <vulkan/vulkan.h>

namespace Vulkan {

class Device;
class Image;

class ImageView {
public:
  ImageView() = default;
  ImageView(const Device& device, const Image& image);
  ~ImageView();

  void init(const Device& device, const Image& image);
  void release();

  operator VkImageView() const { return _view; }

  const Image& image() const { return *_image; }

private:
  const Device* _device = nullptr;
  const Image* _image = nullptr;

  VkImageView _view = VK_NULL_HANDLE;
};

} //namespace Vulkan