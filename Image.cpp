#include "Image.h"

#include "Device.h"

NAMESPACE_VULKAN_BEGIN

Image::Image(const Device& device, VkFormat format, VkExtent2D extent) {
  allocate(device, format, extent);
}

Image::Image(VkImage image, VkFormat format, VkExtent2D extent)
    : _image(image), _format(format), _extent(extent) {
}

Image::~Image() noexcept(false) {
  if (isAllocated()) {
    free();
  }
}

Image::Image(Image&& rhs) noexcept {
  moveFrom(rhs);
}

Image& Image::operator=(Image&& rhs) noexcept(false) {
  if (this == &rhs) {
    moveFrom(rhs);
  }
  return *this;
}

void Image::allocate(const Device& device, VkFormat format, VkExtent2D extent) {
  if (isAllocated()) {
    throw std::runtime_error("Vulkan image has been initialized and its memory allocated already!");
  }
  _device = &device;
  _format = format;
  _extent = extent;

  VkImageCreateInfo imageInfo{};
  imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  imageInfo.imageType = VK_IMAGE_TYPE_2D;
  imageInfo.extent.width = extent.width;
  imageInfo.extent.height = extent.height;
  imageInfo.extent.depth = 1;
  imageInfo.mipLevels = 1;
  imageInfo.arrayLayers = 1;
  imageInfo.format = format;
  imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
  imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
  imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
  imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  MI_VERIFY_VKCMD(vkCreateImage(device, &imageInfo, nullptr, &_image));

  VkMemoryRequirements memRequirements;
  vkGetImageMemoryRequirements(device, _image, &memRequirements);

  VkMemoryAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex =
      findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  MI_VERIFY_VKCMD(vkAllocateMemory(device, &allocInfo, nullptr, &_memory));

  vkBindImageMemory(device, _image, _memory, 0);
}

void Image::free() {
  if (!isAllocated()) {
    throw std::runtime_error("Vulkan null image and its memory cannot be released!");
  }

  vkDestroyImage(*_device, _image, nullptr);
  vkFreeMemory(*_device, _memory, nullptr);

  _image = VK_NULL_HANDLE;
  _memory = VK_NULL_HANDLE;
  _device = nullptr;
}

void Image::moveFrom(Image& rhs) {
  MI_VERIFY(_image == VK_NULL_HANDLE);
  _image = rhs._image;
  _memory = rhs._memory;
  _format = rhs._format;
  _extent = rhs._extent;
  _device = rhs._device;

  rhs._image = VK_NULL_HANDLE;
  rhs._memory = VK_NULL_HANDLE;
  rhs._format = VK_FORMAT_UNDEFINED;
  rhs._extent = {0, 0};
  rhs._device = nullptr;
}

uint32_t Image::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const {
  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(_device->physicalDevice(), &memProperties);

  uint32_t idx = 0;
  for (auto& memType : memProperties.memoryTypes) {
    if (((typeFilter & (1 << idx)) != 0u) && (memType.propertyFlags & properties) == properties) {
      return idx;
    }
    ++idx;
  }
  throw std::runtime_error("Failed to find suitable memory type!");
}

NAMESPACE_VULKAN_END
