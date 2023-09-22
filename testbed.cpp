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

#include "Instance.h"
#include "PhysicalDevice.h"
#include "Surface.h"
#include "Swapchain.h"
#include "Device.h"
#include "Pipeline.h"
#include "RenderPass.h"
#include "CommandBuffer.h"
#include "CommandPool.h"
#include "VertexShader.h"
#include "FragmentShader.h"
#include "DescriptorPool.h"
#include "DescriptorSet.h"
#include "DescriptorSetLayout.h"
#include "Buffer.h"
#include "UniformBuffer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "StagingBuffer.h"
#include "Image.h"
#include "ImageView.h"
#include "Sampler.h"
#include "Semaphore.h"
#include "Fence.h"
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

struct Vertex {
  glm::vec2 pos;
  glm::vec3 color;
  glm::vec2 texCoord;
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

  Vulkan::Surface _surface;
  Vulkan::Device _device;
  Vulkan::Swapchain _swapchain;

  Vulkan::RenderPass _renderPass;
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
    _textureImage.destroy();

    _indexBuffer.destroy();
    _vertexBuffer.destroy();

    _renderFinishedSemaphores.clear();
    _imageAvailableSemaphores.clear();
    _inFlightFences.clear();

    _commandBuffers.clear();
    _commandPool.destroy();
    _device.destroy();
    _surface.destroy();
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
    _surface.create(_instance, surface);
  }

  void pickPhysicalDevice() {
    _instance.pickPhysicalDevice(_surface,
                                 [this](VkPhysicalDevice d) { return isDeviceSuitable(d); });
  }

  void createLogicalDevice() {
    const auto &queueFamilies = _instance.physicalDevice().queueFamilies();

    _device.create(_instance.physicalDevice(),
                   {queueFamilies.graphicsIndex(), queueFamilies.presentIndex()},
                   deviceExtensions);

    _device.initQueue("graphics", queueFamilies.graphicsIndex());
    _device.initQueue("present", queueFamilies.presentIndex());
  }

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
    _swapchain.create(_device,
                      _surface,
                      chooseSwapSurfaceFormatFuc,
                      chooseSwapPresentModeFunc,
                      chooseSwapExtentFuc);
  }

  void createRenderPass() { _renderPass.create(_device, _swapchain.imageFormat()); }

  void createGraphicsPipeline() {
    Vulkan::VertexShader vertShader{_device, "main", "shaders/vert.spv"};
    vertShader.addVertexInputBinding(0, sizeof(Vertex));
    vertShader.addVertexInputAttribute(0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, pos));
    vertShader.addVertexInputAttribute(1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color));
    vertShader.addVertexInputAttribute(2, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, texCoord));
    vertShader.addDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);

    Vulkan::FragmentShader fragShader{_device, "main", "shaders/frag.spv"};
    fragShader.addDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);

    _graphicsPipeline.create(_device, _renderPass, vertShader, fragShader);
  }

  void createFramebuffers() { _swapchain.createFramebuffers(_renderPass); }

  void createCommandPool() {
    _commandPool.create(_device, _instance.physicalDevice().queueFamilies().graphicsIndex());
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

    _textureImage.create(_device,
                         VK_FORMAT_R8G8B8A8_SRGB,
                         {static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight)});
    _textureImage.allocate();

    transitionImageLayout(_textureImage,
                          VK_FORMAT_R8G8B8A8_SRGB,
                          VK_IMAGE_LAYOUT_UNDEFINED,
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    stagingBuffer.copyToImage(Vulkan::CommandBuffer{_commandPool},
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
    _textureSampler.create(_device, {VK_FILTER_LINEAR}, {VK_SAMPLER_ADDRESS_MODE_REPEAT});
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

  void createVertexBuffer() {
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    Vulkan::StagingBuffer stagingBuffer(_device, bufferSize);
    stagingBuffer.copyFromHost(vertices.data(), (size_t)bufferSize);

    _vertexBuffer.create(_device, bufferSize);

    stagingBuffer.copyToBuffer(Vulkan::CommandBuffer{_commandPool}, _vertexBuffer, bufferSize);
  }

  void createIndexBuffer() {
    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    Vulkan::StagingBuffer stagingBuffer(_device, bufferSize);
    stagingBuffer.copyFromHost(indices.data(), (size_t)bufferSize);

    _indexBuffer.create(_device, bufferSize);

    stagingBuffer.copyToBuffer(Vulkan::CommandBuffer{_commandPool}, _indexBuffer, bufferSize);
  }

  void createUniformBuffers() {
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    _uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    _uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
      _uniformBuffers[i].create(_device, bufferSize);
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
    _descriptorSets.reserve(MAX_FRAMES_IN_FLIGHT);
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
      _descriptorSets.emplace_back(_descriptorPool, _graphicsPipeline.descriptorSetLayout());

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
      _inFlightFences[i].create(_device, true);
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

    result = vkQueuePresentKHR(_device.queue("present"), &presentInfo);

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

  bool isDeviceSuitable(VkPhysicalDevice device) {
    auto queueFamilies = Vulkan::PhysicalDevice::findQueueFamilies(device, _surface);
    bool isQueueFamiliesComplete = queueFamilies.graphics && queueFamilies.present;

    bool extensionsSupported = checkDeviceExtensionSupport(device);

    bool swapChainAdequate = false;
    if (extensionsSupported) {
      auto swapChainSupport = _surface.querySupports(device);
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
