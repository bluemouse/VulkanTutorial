#pragma once

#include <vulkan/vulkan.h>

#include <vector>

namespace Vulkan {

class Instance {
public:
  Instance() = default;
  Instance(int versionMajor, int versionMinor,
           std::vector<const char*> extensions,
           const char* name = nullptr,
           bool enableValidation = false);
  ~Instance();

  void create(int versionMajor, int versionMinor,
            std::vector<const char*> extensions,
            const char* name = nullptr,
            bool enableValidation = false);
  //TODO we should be able to create the surface using functions such
  //     as vkCreateWin32SurfaceKHRvkCreateWin32SurfaceKHR and
  //     vkCreateXcbSurfaceKHR. For now, we'll just take the surface
  //     created by glfw.
  void initSurface(VkSurfaceKHR surface);

  void destroy();

  operator VkInstance() const { return _instance; }
  VkSurfaceKHR surface() const { return _surface; }

  bool isValidationLayersEnabled() const { return _enableValidation; }
  static const std::vector<const char*> validationLayers;

private:
  bool checkValidationLayerSupport() const;
  void populateDebugMessengerCreateInfo(
      VkDebugUtilsMessengerCreateInfoEXT &createInfo) const;
  void setupDebugMessenger();

  static VkResult CreateDebugUtilsMessengerEXT(
      VkInstance instance,
      const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
      const VkAllocationCallbacks *pAllocator,
      VkDebugUtilsMessengerEXT *pDebugMessenger);
  static void
  DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                VkDebugUtilsMessengerEXT debugMessenger,
                                const VkAllocationCallbacks *pAllocator);

private:
  VkInstance _instance = VK_NULL_HANDLE;
  VkSurfaceKHR _surface = VK_NULL_HANDLE;

  bool _enableValidation = false;
  VkDebugUtilsMessengerEXT _debugMessenger;
};

} //namespace Vulkan