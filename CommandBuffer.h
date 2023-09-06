#pragma once

#include <functional>

#include <vulkan/vulkan.h>

namespace Vulkan {

class Device;
class CommandPool;

class CommandBuffer {
public:
  CommandBuffer() = default;
  CommandBuffer(const CommandPool& commandPool);
  ~CommandBuffer();

  void allocate(const CommandPool& commandPool);
  void free();

  void recordCommand(const std::function<void(const CommandBuffer& buffer)>& command) const;
  void executeCommand(const std::function<void(const CommandBuffer& buffer)>& command,
                      std::vector<VkSemaphore> waits = {}, std::vector<VkSemaphore> signals = {},
                      VkFence fence = VK_NULL_HANDLE) const;

  void recordSingleTimeCommand(const std::function<void(const CommandBuffer& buffer)>& command) const;
  void executeSingleTimeCommand(const std::function<void(const CommandBuffer& buffer)>& command, bool blocking = true) const;

  void reset();

  operator VkCommandBuffer() const { return _buffer; }
  operator const VkCommandBuffer*() const { return &_buffer; }

private:
  VkCommandBuffer _buffer = VK_NULL_HANDLE;

  const CommandPool* _pool = nullptr;
};

} //namespace Vulkan