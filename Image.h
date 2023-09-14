#pragma once

#include <vulkan/vulkan.h>

#include "helpers_vulkan.h"

NAMESPACE_VULKAN_BEGIN

class Device;

class Image {
 public:
  Image() = default;
  Image(const Device& device, VkFormat format, VkExtent2D extent);
  Image(VkImage image, VkFormat format, VkExtent2D extent);
  ~Image() noexcept(false);

  // Transfer the ownership from `rhs` to `this`
  Image(Image&& rhs) noexcept;
  Image& operator=(Image&& rhs) noexcept(false);

  void allocate(const Device& device, VkFormat format, VkExtent2D extent);
  void free();

  operator VkImage() const { return _image; }

  [[nodiscard]] VkFormat format() const { return _format; }

  [[nodiscard]] VkExtent2D extent() const { return _extent; }
  [[nodiscard]] uint32_t width() const { return _extent.width; }
  [[nodiscard]] uint32_t height() const { return _extent.height; }

 private:
  uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;
  void moveFrom(Image& rhs);

  bool isOwner() const { return _memory != VK_NULL_HANDLE; }
  bool isAllocated() const { return isOwner() && _image != VK_NULL_HANDLE; }

 private:
  VkImage _image = VK_NULL_HANDLE;
  VkDeviceMemory _memory = VK_NULL_HANDLE;

  VkFormat _format = VK_FORMAT_UNDEFINED;
  VkExtent2D _extent = {0, 0};

  const Device* _device = nullptr;
};

NAMESPACE_VULKAN_END