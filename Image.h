#pragma once

#include <vulkan/vulkan.h>

namespace Vulkan {

class Device;

class Image {
public:
  Image() = default;
  Image(const Device& device, VkFormat format, VkExtent2D extent);
  Image(VkImage image, VkFormat format, VkExtent2D extent);
  ~Image();

  void allocate(const Device& device, VkFormat format, VkExtent2D extent);
  void free();

  operator VkImage() const { return _image; }

  VkFormat format() const { return _format; }

  VkExtent2D extent() const { return _extent; }
  uint32_t width() const { return _extent.width; }
  uint32_t height() const { return _extent.height; }

private:
  uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;
  bool isOwner() const { return _memory != VK_NULL_HANDLE; }
  bool isAllocated() const { return isOwner() && _image != VK_NULL_HANDLE; }

private:
  const Device* _device;

  VkImage _image = VK_NULL_HANDLE;
  VkFormat _format;
  VkExtent2D _extent;

  VkDeviceMemory _memory = VK_NULL_HANDLE;
};

} //namespace Vulkan