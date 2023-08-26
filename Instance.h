#pragma once

#include <vector>

#include <vulkan/vulkan.h>

namespace Vulkan {

class Instance {
public:
  Instance() = default;
  Instance(int versionMajor, int versionMinor,
           std::vector<const char*> extensions,
           const char* name = nullptr,
           bool enableValidation = false);
  ~Instance();

  void init(int versionMajor, int versionMinor,
            std::vector<const char*> extensions,
            const char* name = nullptr,
            bool enableValidation = false);
  void release();

  operator VkInstance() const { return _instance; }

  bool isValidationLayersEnabled() const { return _enableValidation; }

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

  bool _enableValidation = false;
  VkDebugUtilsMessengerEXT _debugMessenger;
};

} //namespace Vulkan