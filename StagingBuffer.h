#pragma once

#include <vulkan/vulkan.h>

#include "Buffer.h"

NAMESPACE_VULKAN_BEGIN

class Device;
class CommandBuffer;
class Image;

class StagingBuffer : public Buffer {
 public:
  StagingBuffer() = default;
  StagingBuffer(const Device& device, VkDeviceSize size);

  // Transfer the ownership from `rhs` to `this`
  StagingBuffer(StagingBuffer&& rhs) = default;
  StagingBuffer& operator=(StagingBuffer&& rhs) noexcept(false) = default;

  void create(const Device& device, VkDeviceSize size);

  void copyFromHost(const void* src, VkDeviceSize size);

  void copyToBuffer(const CommandBuffer& commandBuffer,
                    Buffer& dst,
                    const VkBufferCopy& roi,
                    bool waitForFinish = true);
  void copyToBuffer(const CommandBuffer& commandBuffer,
                    Buffer& dst,
                    VkDeviceSize size,
                    bool waitForFinish = true);

  void copyToImage(const CommandBuffer& commandBuffer,
                   Image& dst,
                   const VkBufferImageCopy& roi,
                   bool waitForFinish = true);
  void copyToImage(const CommandBuffer& commandBuffer,
                   Image& dst,
                   uint32_t width,
                   uint32_t height,
                   bool waitForFinish = true);
};

NAMESPACE_VULKAN_END