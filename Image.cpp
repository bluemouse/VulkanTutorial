#include "Image.h"
#include "Device.h"

#include "helpers.h"

using namespace Vulkan;

Image::Image(const Device& device, VkFormat format, VkExtent2D extent) {
  init(device, format, extent);
}

Image::Image(VkImage image, VkFormat format, VkExtent2D extent)
: _image(image), _format(format), _extent(extent) {
}

Image::~Image() {
  if (isAllocated()) {
    release();
  }
}

void Image::init(const Device& device, VkFormat format, VkExtent2D extent) {
  if (isAllocated()) {
    throw std::runtime_error("Vulkan image has been initialized and its memory allocated already!");
  }
  _device = &device;

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

  VK_VERIFY(vkCreateImage(device, &imageInfo, nullptr, &_image));

  VkMemoryRequirements memRequirements;
  vkGetImageMemoryRequirements(device, _image, &memRequirements);

  VkMemoryAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex =
      findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  VK_VERIFY(vkAllocateMemory(device, &allocInfo, nullptr, &_memory));

  vkBindImageMemory(device, _image, _memory, 0);
}

void Image::release() {
  if (!isAllocated()) {
    throw std::runtime_error("Vulkan null image and its memory cannot be released!");
  }

  vkDestroyImage(*_device, _image, nullptr);
  vkFreeMemory(*_device, _memory, nullptr);

  _device = nullptr;
  _image = VK_NULL_HANDLE;
  _memory = VK_NULL_HANDLE;
}

uint32_t Image::findMemoryType(uint32_t typeFilter,
                               VkMemoryPropertyFlags properties) const {
  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(_device->physicalDevice(), &memProperties);

  for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
    if ((typeFilter & (1 << i)) &&
        (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
      return i;
    }
  }
  throw std::runtime_error("Failed to find suitable memory type!");
}
