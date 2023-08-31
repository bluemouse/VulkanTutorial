#pragma once

#include <vulkan/vulkan.h>

#include <vector>

namespace Vulkan {

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
  Pipeline(const Device& device, const RenderPass& renderPass,
           const Shader& vertShader, const Shader& fragShader,
           VkVertexInputBindingDescription bindingDescription,
           std::vector<VkVertexInputAttributeDescription> attributeDescriptions,
           VkDescriptorSetLayout descriptorSetLayout);
  ~Pipeline();

  void init(const Device& device, const RenderPass& renderPass,
            const Shader& vertShader, const Shader& fragShader,
            VkVertexInputBindingDescription bindingDescription,
            std::vector<VkVertexInputAttributeDescription> attributeDescriptions,
            VkDescriptorSetLayout descriptorSetLayout);
  void release();

  operator VkPipeline() const { return _pipeline; }
  VkPipelineLayout layout() const { return _layout; }

private:
  const Device* _device = nullptr;

  VkPipeline _pipeline = VK_NULL_HANDLE;
  VkPipelineLayout _layout;
};

} //namespace Vulkan