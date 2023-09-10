#include "ShaderModule.h"
#include "Device.h"

#include "helpers_vulkan.h"

#include <vector>
#include <fstream>

namespace {
std::vector<char> readFile(const std::string &filename) {
  std::ifstream file(filename, std::ios::ate | std::ios::binary);

  if (!file.is_open()) {
    std::string msg = std::string("Failed to open file: ") + filename + "!";
    throw std::runtime_error(msg);
  }

  size_t fileSize = (size_t)file.tellg();
  std::vector<char> buffer(fileSize);

  file.seekg(0);
  file.read(buffer.data(), fileSize);

  file.close();

  return buffer;
};
} //namespace

using namespace Vulkan;

ShaderModule::ShaderModule(const Device& device, const char* shaderFile)
  : _device(&device) {
  auto code = readFile(shaderFile);

  VkShaderModuleCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = code.size();
  createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

  MI_VERIFY_VKCMD(vkCreateShaderModule(device, &createInfo, nullptr, &_shader));
}

ShaderModule::~ShaderModule() {
  vkDestroyShaderModule(*_device, _shader, nullptr);
}

