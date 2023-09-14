#include "Framebuffer.h"

#include "Device.h"
#include "Image.h"
#include "ImageView.h"
#include "RenderPass.h"

NAMESPACE_VULKAN_BEGIN

Framebuffer::Framebuffer(const Device& device,
                         const RenderPass& renderPass,
                         const ImageView& imageView)
    : _device(&device), _renderPass(&renderPass), _imageView(&imageView) {
  std::array<VkImageView, 1> attachments = {imageView};

  VkFramebufferCreateInfo framebufferInfo{};
  framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  framebufferInfo.renderPass = renderPass;
  framebufferInfo.attachmentCount = 1;
  framebufferInfo.pAttachments = attachments.data();
  framebufferInfo.width = imageView.image().width();
  framebufferInfo.height = imageView.image().height();
  framebufferInfo.layers = 1;

  MI_VERIFY_VKCMD(vkCreateFramebuffer(device, &framebufferInfo, nullptr, &_buffer));
}

Framebuffer::~Framebuffer() {
  vkDestroyFramebuffer(*_device, _buffer, nullptr);
}

VkExtent2D Framebuffer::extent() const {
  return _imageView->image().extent();
}

NAMESPACE_VULKAN_END
