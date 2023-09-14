#pragma once

#include <vulkan/vulkan.h>

#include "helpers_vulkan.h"

NAMESPACE_VULKAN_BEGIN

class Device;
class Image;

class ImageView {
 public:
  ImageView() = default;
  ImageView(const Device& device, const Image& image);
  ~ImageView();

  // Transfer the ownership from `rhs` to `this`
  ImageView(ImageView&& rhs) noexcept;
  ImageView& operator=(ImageView&& rhs) noexcept(false);

  void create(const Device& device, const Image& image);
  void destroy();

  operator VkImageView() const { return _view; }

  [[nodiscard]] const Image& image() const { return *_image; }

 private:
  void moveFrom(ImageView& rhs);

 private:
  VkImageView _view = VK_NULL_HANDLE;

  const Device* _device = nullptr;
  const Image* _image = nullptr;
};

NAMESPACE_VULKAN_END