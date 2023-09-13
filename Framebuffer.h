#pragma once

#include <vulkan/vulkan.h>

#include "helpers_vulkan.h"

NAMESPACE_VULKAN_BEGIN

class Device;
class RenderPass;
class ImageView;

class Framebuffer {
 public:
  Framebuffer(const Device& device, const RenderPass& renderPass, const ImageView& imageView);
  ~Framebuffer();

  operator VkFramebuffer() const { return _buffer; }

  [[nodiscard]] VkExtent2D extent() const;

 private:
  VkFramebuffer _buffer = VK_NULL_HANDLE;

  const Device* _device;
  const RenderPass* _renderPass;
  const ImageView* _imageView;
};

NAMESPACE_VULKAN_END