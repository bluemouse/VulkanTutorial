#include "ShaderModule.h"

#include <fstream>
#include <vector>

#include "Device.h"

namespace {
std::vector<char> readFile(const std::string& filename) {
  std::ifstream inputFile(filename, std::ios::binary | std::ios::ate);

  if (!inputFile) {
    throw std::runtime_error(std::string("Failed to open file: ") + filename + "!");
  }

  std::streamsize fileSize = inputFile.tellg();
  inputFile.seekg(0, std::ios::beg);

  std::vector<char> charVector;
  charVector.reserve(fileSize);

  charVector.assign(std::istreambuf_iterator<char>(inputFile), {});

  inputFile.close();
  return charVector;
}

} // namespace

NAMESPACE_VULKAN_BEGIN

ShaderModule::ShaderModule(const Device& device, const char* shaderFile) : _device(&device) {
  auto code = readFile(shaderFile);

  VkShaderModuleCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = code.size();
  createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

  MI_VERIFY_VKCMD(vkCreateShaderModule(device, &createInfo, nullptr, &_shader));
}

ShaderModule::~ShaderModule() {
  vkDestroyShaderModule(*_device, _shader, nullptr);
}

ShaderModule::ShaderModule(ShaderModule&& rhs) noexcept {
  moveFrom(rhs);
}

ShaderModule& ShaderModule::operator=(ShaderModule&& rhs) noexcept(false) {
  if (this != &rhs) {
    moveFrom(rhs);
  }
  return *this;
}

void ShaderModule::moveFrom(ShaderModule& rhs) {
  MI_VERIFY(_shader == VK_NULL_HANDLE);
  _shader = rhs._shader;
  _device = rhs._device;

  rhs._shader = VK_NULL_HANDLE;
  rhs._device = nullptr;
}
NAMESPACE_VULKAN_END
