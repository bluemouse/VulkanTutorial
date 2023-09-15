#include "RenderPass.h"

#include "Device.h"

NAMESPACE_VULKAN_BEGIN

RenderPass::RenderPass(const Device& device, VkFormat format) {
  create(device, format);
}

RenderPass::~RenderPass() {
  if (_renderPass != VK_NULL_HANDLE) {
    destroy();
  }
}

RenderPass::RenderPass(RenderPass&& rhs) noexcept {
  moveFrom(rhs);
}

RenderPass& RenderPass::operator=(RenderPass&& rhs) noexcept(false) {
  if (this != &rhs) {
    moveFrom(rhs);
  }
  return *this;
}

void RenderPass::create(const Device& device, VkFormat format) {
  MI_VERIFY(_renderPass == VK_NULL_HANDLE);
  _device = &device;

  VkAttachmentDescription colorAttachment{};
  colorAttachment.format = format;
  colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference colorAttachmentRef{};
  colorAttachmentRef.attachment = 0;
  colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass{};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &colorAttachmentRef;

  VkSubpassDependency dependency{};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  VkRenderPassCreateInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount = 1;
  renderPassInfo.pAttachments = &colorAttachment;
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subpass;
  renderPassInfo.dependencyCount = 1;
  renderPassInfo.pDependencies = &dependency;

  MI_VERIFY_VKCMD(vkCreateRenderPass(device, &renderPassInfo, nullptr, &_renderPass));
}

void RenderPass::destroy() {
  MI_VERIFY(_renderPass != VK_NULL_HANDLE);
  vkDestroyRenderPass(*_device, _renderPass, nullptr);

  _renderPass = VK_NULL_HANDLE;
  _device = nullptr;
}

void RenderPass::moveFrom(RenderPass& rhs) {
  MI_VERIFY(_renderPass == VK_NULL_HANDLE);
  _renderPass = rhs._renderPass;
  _device = rhs._device;

  rhs._renderPass = VK_NULL_HANDLE;
  rhs._device = nullptr;
}

NAMESPACE_VULKAN_END
