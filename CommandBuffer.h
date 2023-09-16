#pragma once

#include <vulkan/vulkan.h>

#include <functional>
#include <vector>

#include "Fence.h"
#include "Semaphore.h"
#include "helpers_vulkan.h"

NAMESPACE_VULKAN_BEGIN

class Device;
class CommandPool;

class CommandBuffer {
 public:
  CommandBuffer() = default;
  CommandBuffer(const CommandPool& commandPool);
  ~CommandBuffer();

  // Transfer the ownership from `rhs` to `this`
  CommandBuffer(CommandBuffer&& rhs) noexcept;
  CommandBuffer& operator=(CommandBuffer&& rhs) noexcept(false);

  void allocate(const CommandPool& commandPool);
  void free();

  using Recorder = std::function<void(const CommandBuffer& buffer)>;
  // Record the commands played by `recorder` into this command buffer
  void recordCommand(const Recorder& recorder) const { recordCommand(recorder, false); }
  // Execute the commands currently recorded in this command buffer
  void executeCommand(const std::vector<Semaphore*>& waits = {},
                      const std::vector<Semaphore*>& signals = {},
                      const Fence& fence = {}) const;
  // Record and  the commands played by `recorder` into this command buffer and then execute them
  void executeCommand(const Recorder& recorder,
                      const std::vector<Semaphore*>& waits = {},
                      const std::vector<Semaphore*>& signals = {},
                      const Fence& fence = {}) const;

  void recordSingleTimeCommand(const Recorder& recorder) const { recordCommand(recorder, true); }
  void executeSingleTimeCommand(const Recorder& recorder,
                                const std::vector<Semaphore*>& waits = {},
                                const std::vector<Semaphore*>& signals = {},
                                const Fence& fence = {}) const;

  void waitIdle() const;

  void reset();

  operator VkCommandBuffer() const { return _buffer; }
  operator const VkCommandBuffer*() const { return &_buffer; }

  [[nodiscard]] bool isAllocated() const { return _buffer != VK_NULL_HANDLE; }


 private:
  void recordCommand(const Recorder& recorder, bool singleTime) const;
  void moveFrom(CommandBuffer& rhs);

 private:
  VkCommandBuffer _buffer = VK_NULL_HANDLE;

  const CommandPool* _pool = nullptr;
};

NAMESPACE_VULKAN_END