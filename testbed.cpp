#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
#include <set>
#include <stdexcept>
#include <vector>

#include "Buffer.h"
#include "CommandBuffer.h"
#include "CommandPool.h"
#include "DescriptorPool.h"
#include "DescriptorSet.h"
#include "DescriptorSetLayout.h"
#include "Device.h"
#include "Fence.h"
#include "Image.h"
#include "ImageView.h"
#include "IndexBuffer.h"
#include "Instance.h"
#include "PhysicalDevice.h"
#include "Pipeline.h"
#include "RenderPass.h"
#include "Sampler.h"
#include "Semaphore.h"
#include "ShaderModule.h"
#include "StagingBuffer.h"
#include "Swapchain.h"
#include "UniformBuffer.h"
#include "VertexBuffer.h"
#include "helpers_vulkan.h"

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

const int MAX_FRAMES_IN_FLIGHT = 2;

const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};

struct Vertex {
  glm::vec2 pos;
  glm::vec3 color;
  glm::vec2 texCoord;

  static VkVertexInputBindingDescription getBindingDescription() {
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
  }

  static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() {
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions(3);

    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex, pos);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex, color);

    attributeDescriptions[2].binding = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

    return attributeDescriptions;
  }
};

struct UniformBufferObject {
  alignas(16) glm::mat4 model;
  alignas(16) glm::mat4 view;
  alignas(16) glm::mat4 proj;
};

const std::vector<Vertex> vertices = {{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
                                      {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
                                      {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
                                      {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}};

const std::vector<uint16_t> indices = {0, 1, 2, 2, 3, 0};

class HelloTriangleApplication {
 public:
  void run() {
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
  }

 private:
  GLFWwindow *window;

  Vulkan::Instance _instance;

  Vulkan::PhysicalDevice _physicalDevice;
  Vulkan::Device _device;
  Vulkan::Swapchain _swapchain;

  Vulkan::RenderPass _renderPass;
  Vulkan::DescriptorSetLayout _descriptorSetLayout;
  Vulkan::Pipeline _graphicsPipeline;

  Vulkan::CommandPool _commandPool;

  Vulkan::Image _textureImage;
  Vulkan::ImageView _textureImageView;
  Vulkan::Sampler _textureSampler;

  Vulkan::VertexBuffer _vertexBuffer;
  Vulkan::IndexBuffer _indexBuffer;

  std::vector<Vulkan::UniformBuffer> _uniformBuffers;
  std::vector<void *> _uniformBuffersMapped;

  Vulkan::DescriptorPool _descriptorPool;
  std::vector<Vulkan::DescriptorSet> _descriptorSets;

  std::vector<Vulkan::CommandBuffer> _commandBuffers;

  std::vector<Vulkan::Semaphore> _imageAvailableSemaphores;
  std::vector<Vulkan::Semaphore> _renderFinishedSemaphores;
  std::vector<Vulkan::Fence> _inFlightFences;
  uint32_t currentFrame = 0;

  bool framebufferResized = false;

  void initWindow() {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
  }

  static void framebufferResizeCallback(GLFWwindow *window, int width, int height) {
    auto app = reinterpret_cast<HelloTriangleApplication *>(glfwGetWindowUserPointer(window));
    app->framebufferResized = true;
  }

  void initVulkan() {
    createInstance();
    pickPhysicalDevice();
    createLogicalDevice();
    createSwapChain();
    createRenderPass();
    createDescriptorSetLayout();
    createGraphicsPipeline();
    createFramebuffers();
    createCommandPool();
    createTextureImage();
    createTextureImageView();
    createTextureSampler();
    createVertexBuffer();
    createIndexBuffer();
    createUniformBuffers();
    createDescriptorPool();
    createDescriptorSets();
    createCommandBuffers();
    createSyncObjects();
  }

  void mainLoop() {
    while (glfwWindowShouldClose(window) == 0) {
      glfwPollEvents();
      drawFrame();
    }

    vkDeviceWaitIdle(_device);
  }

  void cleanup() {
    _swapchain.destroy();

    _graphicsPipeline.destroy();
    _renderPass.destroy();

    _uniformBuffers.clear();

    _descriptorSets.clear();
    _descriptorPool.destroy();

    _textureSampler.destroy();
    _textureImageView.destroy();
    _textureImage.free();

    _descriptorSetLayout.destroy();

    _indexBuffer.free();
    _vertexBuffer.free();

    _renderFinishedSemaphores.clear();
    _imageAvailableSemaphores.clear();
    _inFlightFences.clear();

    _commandBuffers.clear();
    _commandPool.destroy();
    _device.destroy();
    _physicalDevice.reset();
    _instance.destroy();

    glfwDestroyWindow(window);

    glfwTerminate();
  }

  void recreateSwapChain() {
    int width = 0, height = 0;
    glfwGetFramebufferSize(window, &width, &height);
    while (width == 0 || height == 0) {
      glfwGetFramebufferSize(window, &width, &height);
      glfwWaitEvents();
    }

    vkDeviceWaitIdle(_device);

    _swapchain.destroy();

    createSwapChain();
    createFramebuffers();
  }

  void createInstance() {
    uint32_t extensionCount = 0;
    const char **extensions = glfwGetRequiredInstanceExtensions(&extensionCount);

    // Create Vulkan 1.0 instance
    _instance.create(1, 0, {extensions, extensions + extensionCount}, enableValidationLayers);

    VkSurfaceKHR surface;
    MI_VERIFY_VKCMD(glfwCreateWindowSurface(_instance, window, nullptr, &surface));

    _instance.setSurface(surface);
  }

  void pickPhysicalDevice() {
    _physicalDevice.instantiate(_instance,
                                [this](VkPhysicalDevice d) -> bool { return isDeviceSuitable(d); });
  }

  void createLogicalDevice() { _device.create(_physicalDevice, deviceExtensions); }

  void createSwapChain() {
    auto chooseSwapSurfaceFormatFuc =
        [this](const std::vector<VkSurfaceFormatKHR> &formats) -> VkSurfaceFormatKHR {
      return chooseSwapSurfaceFormat(formats);
    };
    auto chooseSwapPresentModeFunc =
        [this](const std::vector<VkPresentModeKHR> &modes) -> VkPresentModeKHR {
      return chooseSwapPresentMode(modes);
    };
    auto chooseSwapExtentFuc = [this](const VkSurfaceCapabilitiesKHR &caps) -> VkExtent2D {
      return chooseSwapExtent(caps);
    };
    _swapchain.create(
        _device, chooseSwapSurfaceFormatFuc, chooseSwapPresentModeFunc, chooseSwapExtentFuc);
  }

  void createRenderPass() { _renderPass.create(_device, _swapchain.imageFormat()); }

  void createDescriptorSetLayout() {
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.pImmutableSamplers = nullptr;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkDescriptorSetLayoutBinding samplerLayoutBinding{};
    samplerLayoutBinding.binding = 1;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.pImmutableSamplers = nullptr;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    _descriptorSetLayout.create(_device, {uboLayoutBinding, samplerLayoutBinding});
  }

  void createGraphicsPipeline() {
    Vulkan::ShaderModule vertShaderModule{_device, "shaders/vert.spv"};
    Vulkan::ShaderModule fragShaderModule{_device, "shaders/frag.spv"};

    _graphicsPipeline.create(_device,
                             _renderPass,
                             {vertShaderModule, "main"},
                             {fragShaderModule, "main"},
                             Vertex::getBindingDescription(),
                             Vertex::getAttributeDescriptions(),
                             _descriptorSetLayout);
  }

  void createFramebuffers() { _swapchain.createFramebuffers(_renderPass); }

  void createCommandPool() {
    _commandPool.create(_device, _physicalDevice.queueFamilies().graphicsIndex());
  }

  void createTextureImage() {
    int texWidth, texHeight, texChannels;
    stbi_uc *pixels =
        stbi_load("textures/texture.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = texWidth * texHeight * 4;

    if (pixels == nullptr) {
      throw std::runtime_error("failed to load texture image!");
    }

    Vulkan::StagingBuffer stagingBuffer(_device, imageSize);
    stagingBuffer.copyFromHost(pixels, (size_t)imageSize);

    stbi_image_free(pixels);

    _textureImage.allocate(_device,
                           VK_FORMAT_R8G8B8A8_SRGB,
                           {static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight)});

    transitionImageLayout(_textureImage,
                          VK_FORMAT_R8G8B8A8_SRGB,
                          VK_IMAGE_LAYOUT_UNDEFINED,
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    copyBufferToImage(stagingBuffer,
                      _textureImage,
                      static_cast<uint32_t>(texWidth),
                      static_cast<uint32_t>(texHeight));
    transitionImageLayout(_textureImage,
                          VK_FORMAT_R8G8B8A8_SRGB,
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                          VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
  }

  void createTextureImageView() { _textureImageView.create(_device, _textureImage); }

  void createTextureSampler() {
    _textureSampler.create(
        _device, VK_SAMPLER_ADDRESS_MODE_REPEAT, {VK_FILTER_LINEAR, VK_FILTER_LINEAR});
  }

  void transitionImageLayout(VkImage image,
                             VkFormat format,
                             VkImageLayout oldLayout,
                             VkImageLayout newLayout) {
    using Vulkan::CommandBuffer;
    CommandBuffer commandBuffer{_commandPool};
    commandBuffer.executeSingleTimeCommand(
        [image, oldLayout, newLayout](const CommandBuffer &buffer) {
          VkImageMemoryBarrier barrier{};
          barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
          barrier.oldLayout = oldLayout;
          barrier.newLayout = newLayout;
          barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
          barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
          barrier.image = image;
          barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
          barrier.subresourceRange.baseMipLevel = 0;
          barrier.subresourceRange.levelCount = 1;
          barrier.subresourceRange.baseArrayLayer = 0;
          barrier.subresourceRange.layerCount = 1;

          VkPipelineStageFlags sourceStage;
          VkPipelineStageFlags destinationStage;

          if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
              newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
          } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
                     newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
          } else {
            throw std::invalid_argument("unsupported layout transition!");
          }

          vkCmdPipelineBarrier(
              buffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
        });
    commandBuffer.waitIdle();
  }

  void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) {
    using Vulkan::CommandBuffer;
    CommandBuffer commandBuffer{_commandPool};
    commandBuffer.executeSingleTimeCommand(
        [buffer, image, width, height](const CommandBuffer &commandBuffer) {
          VkBufferImageCopy region{};
          region.bufferOffset = 0;
          region.bufferRowLength = 0;
          region.bufferImageHeight = 0;
          region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
          region.imageSubresource.mipLevel = 0;
          region.imageSubresource.baseArrayLayer = 0;
          region.imageSubresource.layerCount = 1;
          region.imageOffset = {0, 0, 0};
          region.imageExtent = {width, height, 1};

          vkCmdCopyBufferToImage(
              commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
        });
    commandBuffer.waitIdle();
  }

  void createVertexBuffer() {
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    Vulkan::StagingBuffer stagingBuffer(_device, bufferSize);
    stagingBuffer.copyFromHost(vertices.data(), (size_t)bufferSize);

    _vertexBuffer.allocate(_device, bufferSize);

    copyBuffer(stagingBuffer, _vertexBuffer, bufferSize);
  }

  void createIndexBuffer() {
    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    Vulkan::StagingBuffer stagingBuffer(_device, bufferSize);
    stagingBuffer.copyFromHost(indices.data(), (size_t)bufferSize);

    _indexBuffer.allocate(_device, bufferSize);

    copyBuffer(stagingBuffer, _indexBuffer, bufferSize);
  }

  void createUniformBuffers() {
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    _uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    _uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
      _uniformBuffers[i].allocate(_device, bufferSize);
      _uniformBuffersMapped[i] = _uniformBuffers[i].map();
    }
  }

  void createDescriptorPool() {
    std::vector<VkDescriptorPoolSize> poolSizes{2};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    _descriptorPool.create(_device, poolSizes, MAX_FRAMES_IN_FLIGHT);
  }

  void createDescriptorSets() {
    _descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
      _descriptorSets[i].allocate(_descriptorPool, _descriptorSetLayout);

      VkDescriptorBufferInfo bufferInfo{};
      bufferInfo.buffer = _uniformBuffers[i];
      bufferInfo.offset = 0;
      bufferInfo.range = sizeof(UniformBufferObject);

      VkDescriptorImageInfo imageInfo{};
      imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
      imageInfo.imageView = _textureImageView;
      imageInfo.sampler = _textureSampler;

      std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

      descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      descriptorWrites[0].dstSet = _descriptorSets[i];
      descriptorWrites[0].dstBinding = 0;
      descriptorWrites[0].dstArrayElement = 0;
      descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      descriptorWrites[0].descriptorCount = 1;
      descriptorWrites[0].pBufferInfo = &bufferInfo;

      descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      descriptorWrites[1].dstSet = _descriptorSets[i];
      descriptorWrites[1].dstBinding = 1;
      descriptorWrites[1].dstArrayElement = 0;
      descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      descriptorWrites[1].descriptorCount = 1;
      descriptorWrites[1].pImageInfo = &imageInfo;

      vkUpdateDescriptorSets(_device,
                             static_cast<uint32_t>(descriptorWrites.size()),
                             descriptorWrites.data(),
                             0,
                             nullptr);
    }
  }

  void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
    using Vulkan::CommandBuffer;
    CommandBuffer commandBuffer{_commandPool};
    commandBuffer.executeSingleTimeCommand(
        [srcBuffer, dstBuffer, size](const CommandBuffer &buffer) {
          VkBufferCopy copyRegion{};
          copyRegion.size = size;
          vkCmdCopyBuffer(buffer, srcBuffer, dstBuffer, 1, &copyRegion);
        });
    commandBuffer.waitIdle();
  }

  void createCommandBuffers() {
    _commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    for (auto &buffer : _commandBuffers) {
      buffer.allocate(_commandPool);
    }
  }

  void createSyncObjects() {
    _imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    _renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    _inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
      _imageAvailableSemaphores[i].create(_device);
      _renderFinishedSemaphores[i].create(_device);
      _inFlightFences[i].create(_device);
    }
  }

  void updateUniformBuffer(uint32_t currentImage) {
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time =
        std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    auto extent = _swapchain.imageExtent();

    UniformBufferObject ubo{};
    ubo.model =
        glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = glm::lookAt(
        glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj =
        glm::perspective(glm::radians(45.0f), extent.width / (float)extent.height, 0.1f, 10.0f);
    ubo.proj[1][1] *= -1;

    memcpy(_uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
  }

  void drawFrame() {
    vkWaitForFences(_device, 1, _inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(_device,
                                            _swapchain,
                                            UINT64_MAX,
                                            _imageAvailableSemaphores[currentFrame],
                                            VK_NULL_HANDLE,
                                            &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
      recreateSwapChain();
      return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
      throw std::runtime_error("failed to acquire swap chain image!");
    }

    updateUniformBuffer(currentFrame);

    vkResetFences(_device, 1, _inFlightFences[currentFrame]);

    _commandBuffers[currentFrame].reset();
    _commandBuffers[currentFrame].executeCommand(
        [this, imageIndex](const Vulkan::CommandBuffer &commandBuffer) {
          const auto &framebuffer = _swapchain.framebuffer(imageIndex);
          auto extent = framebuffer.extent();

          VkRenderPassBeginInfo renderPassInfo{};
          renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
          renderPassInfo.renderPass = _renderPass;
          renderPassInfo.framebuffer = framebuffer;
          renderPassInfo.renderArea.offset = {0, 0};
          renderPassInfo.renderArea.extent = extent;

          VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
          renderPassInfo.clearValueCount = 1;
          renderPassInfo.pClearValues = &clearColor;

          vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
          vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _graphicsPipeline);

          VkViewport viewport{};
          viewport.x = 0.0f;
          viewport.y = 0.0f;
          viewport.width = (float)extent.width;
          viewport.height = (float)extent.height;
          viewport.minDepth = 0.0f;
          viewport.maxDepth = 1.0f;
          vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

          VkRect2D scissor{};
          scissor.offset = {0, 0};
          scissor.extent = extent;
          vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

          VkBuffer vertexBuffers[] = {_vertexBuffer};
          VkDeviceSize offsets[] = {0};
          vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
          vkCmdBindIndexBuffer(commandBuffer, _indexBuffer, 0, VK_INDEX_TYPE_UINT16);
          vkCmdBindDescriptorSets(commandBuffer,
                                  VK_PIPELINE_BIND_POINT_GRAPHICS,
                                  _graphicsPipeline.layout(),
                                  0,
                                  1,
                                  _descriptorSets[currentFrame],
                                  0,
                                  nullptr);

          vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

          vkCmdEndRenderPass(commandBuffer);
        },
        {&_imageAvailableSemaphores[currentFrame]},
        {&_renderFinishedSemaphores[currentFrame]},
        _inFlightFences[currentFrame]);

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = _renderFinishedSemaphores[currentFrame];

    VkSwapchainKHR swapChains[] = {_swapchain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(_device.presentQueue(), &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
      framebufferResized = false;
      recreateSwapChain();
    } else if (result != VK_SUCCESS) {
      throw std::runtime_error("failed to present swap chain image!");
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
  }

  VkSurfaceFormatKHR chooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR> &availableFormats) {
    for (const auto &availableFormat : availableFormats) {
      if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
          availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
        return availableFormat;
      }
    }

    return availableFormats[0];
  }

  VkPresentModeKHR chooseSwapPresentMode(
      const std::vector<VkPresentModeKHR> &availablePresentModes) {
    for (const auto &availablePresentMode : availablePresentModes) {
      if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
        return availablePresentMode;
      }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
  }

  VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
      return capabilities.currentExtent;
    } else {
      int width, height;
      glfwGetFramebufferSize(window, &width, &height);

      VkExtent2D actualExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

      actualExtent.width = std::clamp(
          actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
      actualExtent.height = std::clamp(actualExtent.height,
                                       capabilities.minImageExtent.height,
                                       capabilities.maxImageExtent.height);

      return actualExtent;
    }
  }

  SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device) {
    auto surface = _instance.surface();
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

    if (formatCount != 0) {
      details.formats.resize(formatCount);
      vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
      details.presentModes.resize(presentModeCount);
      vkGetPhysicalDeviceSurfacePresentModesKHR(
          device, surface, &presentModeCount, details.presentModes.data());
    }

    return details;
  }

  bool isDeviceSuitable(VkPhysicalDevice device) {
    auto queueFamilies = Vulkan::PhysicalDevice::findQueueFamilies(device, _instance.surface());
    bool isQueueFamiliesComplete = queueFamilies.graphics && queueFamilies.present;

    bool extensionsSupported = checkDeviceExtensionSupport(device);

    bool swapChainAdequate = false;
    if (extensionsSupported) {
      SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
      swapChainAdequate =
          !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

    return isQueueFamiliesComplete && extensionsSupported && swapChainAdequate &&
           (supportedFeatures.samplerAnisotropy != 0u);
  }

  bool checkDeviceExtensionSupport(VkPhysicalDevice device) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(
        device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto &extension : availableExtensions) {
      requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
  }
};

int main() {
  HelloTriangleApplication app;

  try {
    app.run();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
