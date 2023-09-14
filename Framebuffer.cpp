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

Framebuffer::Framebuffer(Framebuffer&& rhs) noexcept {
  moveFrom(rhs);
}

Framebuffer& Framebuffer::operator=(Framebuffer&& rhs) noexcept(false) {
  if (this != &rhs) {
    moveFrom(rhs);
  }
  return *this;
}

VkExtent2D Framebuffer::extent() const {
  return _imageView->image().extent();
}

void Framebuffer::moveFrom(Framebuffer& rhs) {
  MI_VERIFY(_buffer == VK_NULL_HANDLE);
  _buffer = rhs._buffer;
  _device = rhs._device;
  _renderPass = rhs._renderPass;
  _imageView = rhs._imageView;

  rhs._buffer = VK_NULL_HANDLE;
  rhs._device = nullptr;
  rhs._renderPass = nullptr;
  rhs._imageView = nullptr;
}

NAMESPACE_VULKAN_END
