#include "Sampler.h"

#include "Device.h"

NAMESPACE_VULKAN_BEGIN

Sampler::Sampler(const Device& device, VkSamplerAddressMode addressMode, Filter filter) {
  create(device, addressMode, filter);
}

Sampler::~Sampler() {
  if (isCreated()) {
    destroy();
  }
}

Sampler::Sampler(Sampler&& rhs) noexcept {
  moveFrom(rhs);
}

Sampler& Sampler::operator=(Sampler&& rhs) noexcept(false) {
  if (this != &rhs) {
    moveFrom(rhs);
  }
  return *this;
}

void Sampler::moveFrom(Sampler& rhs) {
  MI_VERIFY(!isCreated());
  _sampler = rhs._sampler;
  _device = rhs._device;

  rhs._sampler = VK_NULL_HANDLE;
  rhs._device = nullptr;
}

void Sampler::create(const Device& device, VkSamplerAddressMode addressMode, Filter filter) {
  MI_VERIFY(!isCreated());
  _device = &device;

  VkPhysicalDeviceProperties properties{};
  vkGetPhysicalDeviceProperties(device.physicalDevice(), &properties);

  VkSamplerCreateInfo samplerInfo{};
  samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  samplerInfo.magFilter = filter.mag;
  samplerInfo.minFilter = filter.min;
  samplerInfo.addressModeU = addressMode;
  samplerInfo.addressModeV = addressMode;
  samplerInfo.addressModeW = addressMode;
  samplerInfo.anisotropyEnable = VK_TRUE;
  samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
  samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
  samplerInfo.unnormalizedCoordinates = VK_FALSE;
  samplerInfo.compareEnable = VK_FALSE;
  samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
  samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

  MI_VERIFY_VKCMD(vkCreateSampler(device, &samplerInfo, nullptr, &_sampler));
}

void Sampler::destroy() {
  MI_VERIFY(isCreated());
  vkDestroySampler(*_device, _sampler, nullptr);

  _device = nullptr;
  _sampler = VK_NULL_HANDLE;
}

NAMESPACE_VULKAN_END
