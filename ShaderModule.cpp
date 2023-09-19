#include "ShaderModule.h"

#include <fstream>
#include <vector>

#include "Device.h"

namespace {
std::vector<char> readFile(const std::string& filename) {
  std::ifstream inputFile(filename, std::ios::binary | std::ios::ate);

  MI_VERIFY_MSG(inputFile, "Failed to open file '%s'", filename.c_str());

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

ShaderModule::ShaderModule(const Device& device, size_t codeSize, const char* codes) {
  create(device, codeSize, codes);
}

ShaderModule::ShaderModule(const Device& device, const char* shaderFile) {
  create(device, shaderFile);
}

ShaderModule::~ShaderModule() {
  if (isCreated()) {
    destroy();
  }
}

void ShaderModule::create(const Device& device, size_t codeSize, const char* codes) {
  MI_VERIFY(!isCreated());
  _device = &device;

  VkShaderModuleCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = codeSize;
  createInfo.pCode = reinterpret_cast<const uint32_t*>(codes);

  MI_VERIFY_VKCMD(vkCreateShaderModule(device, &createInfo, nullptr, &_shader));
}

void ShaderModule::create(const Device& device, const char* shaderFile) {
  auto code = readFile(shaderFile);
  create(device, code.size(), code.data());
}

void ShaderModule::destroy() {
  MI_VERIFY(isCreated());
  vkDestroyShaderModule(*_device, _shader, nullptr);

  _shader = VK_NULL_HANDLE;
  _device = nullptr;
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
  MI_VERIFY(!isCreated());
  _shader = rhs._shader;
  _device = rhs._device;

  rhs._shader = VK_NULL_HANDLE;
  rhs._device = nullptr;
}
NAMESPACE_VULKAN_END
