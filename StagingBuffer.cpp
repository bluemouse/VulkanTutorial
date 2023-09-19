#include "StagingBuffer.h"

#include <cstring>

#include "Device.h"
#include "CommandBuffer.h"
#include "Image.h"

NAMESPACE_VULKAN_BEGIN

StagingBuffer::StagingBuffer(const Device& device, VkDeviceSize size) {
  create(device, size);
}

void StagingBuffer::create(const Device& device, VkDeviceSize size) {
  Buffer::create(device, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
  Buffer::allocate(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

void StagingBuffer::copyFromHost(const void* src, VkDeviceSize size) {
  void* data = map(0, size);
  std::memcpy(data, src, size);
  unmap();
}

void StagingBuffer::copyToBuffer(const CommandBuffer& commandBuffer,
                                 Buffer& dst,
                                 const VkBufferCopy& roi,
                                 bool waitForFinish) {
  commandBuffer.executeSingleTimeCommand([this, &dst, &roi](const CommandBuffer& cmdBuffer) {
    vkCmdCopyBuffer(cmdBuffer, *this, dst, 1, &roi);
  });

  if (waitForFinish) {
    commandBuffer.waitIdle();
  }
}

void StagingBuffer::copyToBuffer(const CommandBuffer& commandBuffer,
                                 Buffer& dst,
                                 VkDeviceSize size,
                                 bool waitForFinish) {
  copyToBuffer(commandBuffer, dst, {0, 0, size}, waitForFinish);
}

void StagingBuffer::copyToImage(const CommandBuffer& commandBuffer,
                                Image& dst,
                                const VkBufferImageCopy& roi,
                                bool waitForFinish) {
  commandBuffer.executeSingleTimeCommand([this, &dst, &roi](const CommandBuffer& commandBuffer) {
    vkCmdCopyBufferToImage(
        commandBuffer, *this, dst, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &roi);
  });

  if (waitForFinish) {
    commandBuffer.waitIdle();
  }
}

void StagingBuffer::copyToImage(const CommandBuffer& commandBuffer,
                                Image& dst,
                                uint32_t width,
                                uint32_t height,
                                bool waitForFinish) {
  copyToImage(commandBuffer,
              dst,
              {0, 0, 0, {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1}, {0, 0, 0}, {width, height, 1}},
              waitForFinish);
}

NAMESPACE_VULKAN_END
