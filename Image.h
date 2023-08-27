#pragma once

#include <vulkan/vulkan.h>

namespace Vulkan {

class Image {
public:
  Image(VkImage image, VkFormat format, VkExtent2D extent);
  ~Image();

  operator VkImage() const { return _image; }

  VkFormat format() const { return _format; }
  VkExtent2D extent() const { return _extent; }

private:
  VkImage _image;
  VkFormat _format;
  VkExtent2D _extent;
};

} //namespace Vulkan