#include "Sampler.h"
#include "Device.h"

#include "helpers_vulkan.h"

using namespace Vulkan;

Sampler::Sampler(const Device& device,
                 VkFilter magFilter, VkFilter minFilter,
                 VkSamplerAddressMode mode) {
  create(device, magFilter, minFilter, mode);
}

Sampler::~Sampler() {
  if (_sampler != VK_NULL_HANDLE) {
    destroy();
  }
}

void Sampler::create(const Device& device,
                   VkFilter magFilter, VkFilter minFilter,
                   VkSamplerAddressMode mode) {
  if (_sampler != VK_NULL_HANDLE) {
    throw std::runtime_error("Vulkan sampler has been initialized already!");
  }
  _device = &device;

  VkPhysicalDeviceProperties properties{};
  vkGetPhysicalDeviceProperties(device.physicalDevice(), &properties);

  VkSamplerCreateInfo samplerInfo{};
  samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  samplerInfo.magFilter = magFilter;
  samplerInfo.minFilter = minFilter;
  samplerInfo.addressModeU = mode;
  samplerInfo.addressModeV = mode;
  samplerInfo.addressModeW = mode;
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
  if (_sampler == VK_NULL_HANDLE) {
    throw std::runtime_error("Vulkan null sampler cannot be released!");
  }

  vkDestroySampler(*_device, _sampler, nullptr);

  _device = nullptr;
  _sampler = VK_NULL_HANDLE;
}
