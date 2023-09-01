#pragma once

#include <vulkan/vulkan.h>

namespace Vulkan {

class Device;
class RenderPass;
class ImageView;

class Framebuffer {
public:
  Framebuffer(const Device& device, const RenderPass& renderPass, const ImageView& imageView);
  ~Framebuffer();

  operator VkFramebuffer() const { return _buffer; }

  VkExtent2D extent() const;

private:
  const Device* _device;
  const RenderPass* _renderPass;
  const ImageView* _imageView;

  VkFramebuffer _buffer = VK_NULL_HANDLE;
};

} //namespace Vulkan