#pragma once

#include <vulkan/vulkan.h>

namespace Vulkan {

class Device;
class CommandPool;
class Framebuffer;
class Pipeline;
class RenderPass;

class CommandBuffer {
public:
  CommandBuffer() = default;
  ~CommandBuffer() = default;

  void allocate(const Device& device, const CommandPool& commandPool);
  void reset();
  void record(const Framebuffer& framebuffer, const Pipeline& pipeline, const RenderPass& renderPass,
              VkBuffer vertexBuffer, VkBuffer indexBuffer, uint32_t numIndices,
              VkDescriptorSet descriptorSet);

  operator VkCommandBuffer() const { return _buffer; }

private:
  VkCommandBuffer _buffer = VK_NULL_HANDLE;
};

} //namespace Vulkan