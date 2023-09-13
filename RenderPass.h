#pragma once

#include <vulkan/vulkan.h>

#include "helpers_vulkan.h"

NAMESPACE_VULKAN_BEGIN

class Device;

class RenderPass {
 public:
  RenderPass() = default;
  RenderPass(const Device& device, VkFormat format);
  ~RenderPass();

  void create(const Device& device, VkFormat format);
  void destroy();

  operator VkRenderPass() const { return _renderPass; }

 private:
  VkRenderPass _renderPass = VK_NULL_HANDLE;

  const Device* _device = nullptr;
};

NAMESPACE_VULKAN_END