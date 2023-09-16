#pragma once

#include <vulkan/vulkan.h>

#include "Buffer.h"

NAMESPACE_VULKAN_BEGIN

class Device;

class IndexBuffer : private Buffer {
 public:
  IndexBuffer() = default;
  IndexBuffer(const Device& device, VkDeviceSize size);

  // Transfer the ownership from `rhs` to `this`
  IndexBuffer(IndexBuffer&& rhs) = default;
  IndexBuffer& operator=(IndexBuffer&& rhs) noexcept(false) = default;

  void create(const Device& device, VkDeviceSize size);
  using Buffer::destroy;

  using Buffer::allocate;
  using Buffer::free;

  using Buffer::operator VkBuffer;
  using Buffer::size;
};

NAMESPACE_VULKAN_END