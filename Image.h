#pragma once

#include <vulkan/vulkan.h>

#include "DeviceMemory.h"
#include "helpers_vulkan.h"

NAMESPACE_VULKAN_BEGIN

class Device;

class Image {
 public:
  using ImageCreateInfoOverride = std::function<void(VkImageCreateInfo*)>;

 public:
  Image() = default;
  Image(const Device& device,
        VkFormat format,
        VkExtent2D extent,
        const ImageCreateInfoOverride& override = {});
  Image(const Device& device,
        VkFormat format,
        VkExtent2D extent,
        VkMemoryPropertyFlags properties,
        const ImageCreateInfoOverride& override = {});
  ~Image() noexcept(false);

  Image(VkImage image, VkFormat format, VkExtent2D extent); // special use by Swapchain

  // Transfer the ownership from `rhs` to `this`
  Image(Image&& rhs) noexcept;
  Image& operator=(Image&& rhs) noexcept(false);

  void create(const Device& device,
              VkFormat format,
              VkExtent2D extent,
              const ImageCreateInfoOverride& override = {});
  void destroy();

  void allocate(VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  void free();

  void bind(const DeviceMemory::Ptr& memory, VkDeviceSize offset = 0);

  operator VkImage() const { return _image; }

  [[nodiscard]] VkFormat format() const { return _format; }

  [[nodiscard]] VkExtent2D extent() const { return _extent; }
  [[nodiscard]] uint32_t width() const { return _extent.width; }
  [[nodiscard]] uint32_t height() const { return _extent.height; }

  [[nodiscard]] bool isCreated() const { return _image != VK_NULL_HANDLE; }
  [[nodiscard]] bool isAllocated() const {
    return isCreated() && (_memory && _memory->isAllocated());
  }
  [[nodiscard]] bool isMapped() const { return isAllocated() && _memory->isMapped(); }

 private:
  uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;
  void moveFrom(Image& rhs);

  bool isExternal() const { return _external; }

 private:
  VkImage _image = VK_NULL_HANDLE;

  VkFormat _format = VK_FORMAT_UNDEFINED;
  VkExtent2D _extent = {0, 0};

  DeviceMemory::Ptr _memory;

  const Device* _device = nullptr;

  bool _external = false;
};

NAMESPACE_VULKAN_END