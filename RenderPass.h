#pragma once

#include <vulkan/vulkan.h>

namespace Vulkan {

class Device;

class RenderPass {
public:
  RenderPass() = default;
  RenderPass(const Device& device, VkFormat format);
  ~RenderPass();

  void init(const Device& device, VkFormat format);
  void release();

  operator VkRenderPass() const { return _renderPass; }

private:
  const Device* _device;

  VkRenderPass _renderPass = VK_NULL_HANDLE;
};

} //namespace Vulkan