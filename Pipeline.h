#pragma once

#include <vulkan/vulkan.h>

#include <vector>

#include "helpers_vulkan.h"

NAMESPACE_VULKAN_BEGIN

class Device;
class RenderPass;
class ShaderModule;

class Pipeline {
 public:
  struct Shader {
    const ShaderModule& module;
    const char* entry;
  };

 public:
  Pipeline() = default;
  Pipeline(const Device& device,
           const RenderPass& renderPass,
           const Shader& vertShader,
           const Shader& fragShader,
           VkVertexInputBindingDescription bindingDescription,
           std::vector<VkVertexInputAttributeDescription> attributeDescriptions,
           VkDescriptorSetLayout descriptorSetLayout);
  ~Pipeline();

  // Transfer the ownership from `rhs` to `this`
  Pipeline(Pipeline&& rhs) noexcept;
  Pipeline& operator=(Pipeline&& rhs) noexcept(false);

  void create(const Device& device,
              const RenderPass& renderPass,
              const Shader& vertShader,
              const Shader& fragShader,
              VkVertexInputBindingDescription bindingDescription,
              std::vector<VkVertexInputAttributeDescription> attributeDescriptions,
              VkDescriptorSetLayout descriptorSetLayout);
  void destroy();

  operator VkPipeline() const { return _pipeline; }
  [[nodiscard]] VkPipelineLayout layout() const { return _layout; }

  [[nodiscard]] bool isCreated() const { return _pipeline != VK_NULL_HANDLE; }

 private:
  void moveFrom(Pipeline& rhs);

 private:
  VkPipeline _pipeline = VK_NULL_HANDLE;
  VkPipelineLayout _layout = VK_NULL_HANDLE;

  const Device* _device = nullptr;
};

NAMESPACE_VULKAN_END