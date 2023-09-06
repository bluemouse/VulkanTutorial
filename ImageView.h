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

  void create(const Device& device, const Image& image);
  void destroy();

  operator VkImageView() const { return _view; }

  const Image& image() const { return *_image; }

private:
  VkImageView _view = VK_NULL_HANDLE;

  const Device* _device = nullptr;
  const Image* _image = nullptr;
};

} //namespace Vulkan