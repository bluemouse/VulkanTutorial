#pragma once

#include <vulkan/vulkan.h>

#include <functional>
#include <vector>

namespace Vulkan {

class Instance {
 public:
  using AppInfoOverride = std::function<void(VkApplicationInfo*)>;
  using InstanceCreateInfoOverride = std::function<void(VkInstanceCreateInfo*)>;
  using DebugUtilsMessengerCreateInfoOverride =
      std::function<void(VkDebugUtilsMessengerCreateInfoEXT*)>;

  Instance() = default;
  Instance(const AppInfoOverride& appInfoOverride,
           const InstanceCreateInfoOverride& instanceCreateInfoOverride,
           const DebugUtilsMessengerCreateInfoOverride& debugUtilsMessengerCreateInfoOverride =
               DebugUtilsMessengerCreateInfoOverride());
  Instance(int versionMajor, int versionMinor, std::vector<const char*> extensions,
           bool enableValidation = false);
  ~Instance();

  void create(
      const AppInfoOverride& appInfoOverride = AppInfoOverride(),
      const InstanceCreateInfoOverride& instanceCreateInfoOverride = InstanceCreateInfoOverride(),
      const DebugUtilsMessengerCreateInfoOverride& debugUtilsMessengerCreateInfoOverride =
          DebugUtilsMessengerCreateInfoOverride());
  void create(int versionMajor, int versionMinor, std::vector<const char*> extensions,
              bool enableValidation = false);
  // TODO we should be able to create the surface using functions such
  //      as vkCreateWin32SurfaceKHR, vkCreateWin32SurfaceKHR and
  //      vkCreateXcbSurfaceKHR. For now, we'll just take the surface
  //      created by the caller.
  void setSurface(VkSurfaceKHR surface) { _surface = surface; }

  void destroy();

  operator VkInstance() const { return _instance; }
  VkSurfaceKHR surface() const { return _surface; }

  bool isValidationLayersEnabled() const { return !_layers.empty(); }
  const std::vector<const char*>& layers() const { return _layers; }

  using DebugCallback = std::function<VkBool32(VkDebugUtilsMessageSeverityFlagBitsEXT,
                                               VkDebugUtilsMessageTypeFlagsEXT,
                                               const VkDebugUtilsMessengerCallbackDataEXT*)>;
  void setDebugCallback(const DebugCallback& callback);

 private:
  bool checkLayerSupport(const std::vector<const char*>& layers) const;
  void initDefaultDebugCallback();

  static VKAPI_ATTR VkBool32 VKAPI_CALL
  VkDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                  VkDebugUtilsMessageTypeFlagsEXT messageType,
                  const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

 private:
  VkInstance _instance = VK_NULL_HANDLE;
  VkSurfaceKHR _surface = VK_NULL_HANDLE;

  VkDebugUtilsMessengerEXT _debugMessenger = VK_NULL_HANDLE;

  std::vector<const char*> _layers;

  DebugCallback _debugCallback;
};

} // namespace Vulkan