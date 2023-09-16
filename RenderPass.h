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

  // Transfer the ownership from `rhs` to `this`
  RenderPass(RenderPass&& rhs) noexcept;
  RenderPass& operator=(RenderPass&& rhs) noexcept(false);

  void create(const Device& device, VkFormat format);
  void destroy();

  operator VkRenderPass() const { return _renderPass; }

  [[nodiscard]] bool isCreated() const { return _renderPass != VK_NULL_HANDLE; }

private:
  void moveFrom(RenderPass& rhs);

 private:
  VkRenderPass _renderPass = VK_NULL_HANDLE;

  const Device* _device = nullptr;
};

NAMESPACE_VULKAN_END