#include "Image.h"

#include "Device.h"

NAMESPACE_VULKAN_BEGIN

Image::Image(const Device& device,
             VkFormat format,
             VkExtent2D extent,
             const ImageCreateInfoOverride& override) {
  create(device, format, extent, override);
}

Image::Image(const Device& device,
             VkFormat format,
             VkExtent2D extent,
             VkMemoryPropertyFlags properties,
             const ImageCreateInfoOverride& override)
    : Image(device, format, extent, override) {
  allocate(properties);
}

Image::~Image() noexcept(false) {
  if (isAllocated()) {
    free();
  }
}

Image::Image(VkImage image, VkFormat format, VkExtent2D extent)
    : _image(image), _format(format), _extent(extent), _external(true) {
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

void Image::moveFrom(Image& rhs) {
  MI_VERIFY(!isCreated());
  _image = rhs._image;
  _memory = rhs._memory;
  _format = rhs._format;
  _extent = rhs._extent;
  _device = rhs._device;
  _external = rhs._external;

  rhs._image = VK_NULL_HANDLE;
  rhs._memory = VK_NULL_HANDLE;
  rhs._format = VK_FORMAT_UNDEFINED;
  rhs._extent = {0, 0};
  rhs._device = nullptr;
  rhs._external = false;
}

void Image::create(const Device& device,
                   VkFormat format,
                   VkExtent2D extent,
                   const ImageCreateInfoOverride& override) {
  MI_VERIFY(!isExternal());
  MI_VERIFY(!isCreated());
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

  if (override) {
    override(&imageInfo);
  }

  MI_VERIFY_VKCMD(vkCreateImage(device, &imageInfo, nullptr, &_image));
}

void Image::destroy() {
  MI_VERIFY(!isExternal());
  MI_VERIFY(isCreated());

  if (isAllocated()) {
    free();
  }
  vkDestroyImage(*_device, _image, nullptr);

  _image = VK_NULL_HANDLE;
  _format = VK_FORMAT_UNDEFINED;
  _extent = {0, 0};
  _device = nullptr;
}

void Image::allocate(VkMemoryPropertyFlags properties) {
  MI_VERIFY(!isExternal());
  MI_VERIFY(!isAllocated());
  _memory = DeviceMemory::make();

  VkMemoryRequirements requirements;
  vkGetImageMemoryRequirements(*_device, _image, &requirements);
  _memory->allocate(*_device, properties, requirements);

  vkBindImageMemory(*_device, _image, *_memory.get(), 0);
}

void Image::free() {
  MI_VERIFY(!isExternal());
  MI_VERIFY(isAllocated());
  _memory = nullptr;
}

void Image::bind(const DeviceMemory::Ptr& memory, VkDeviceSize offset) {
  MI_VERIFY(!isExternal());
  MI_VERIFY(isCreated());
  MI_VERIFY(memory != _memory);
  if (isAllocated()) {
    free();
  }
  _memory = memory;
  vkBindImageMemory(*_device, _image, *_memory.get(), offset);
}

void* Image::map() {
  MI_VERIFY(!isExternal());
  MI_VERIFY(isAllocated());
  return _memory->map();
}

void* Image::map(VkDeviceSize offset, VkDeviceSize size) {
  MI_VERIFY(!isExternal());
  MI_VERIFY(isAllocated());
  return _memory->map(offset, size);
}

void Image::unmap() {
  MI_VERIFY(!isExternal());
  MI_VERIFY(isAllocated());
  _memory->unmap();
}

NAMESPACE_VULKAN_END
