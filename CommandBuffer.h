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

  void allocate(const CommandPool& commandPool);
  void free();

  using Recorder = std::function<void(const CommandBuffer& buffer)>;
  void recordCommand(const Recorder& recorder) const { recordCommand(recorder, false); }
  void executeCommand(const Recorder& recorder, const std::vector<Semaphore*>& waits = {},
                      const std::vector<Semaphore*>& signals = {}, const Fence& fence = {}) const;

  void recordSingleTimeCommand(const Recorder& recorder) const { recordCommand(recorder, true); }
  void executeSingleTimeCommand(const Recorder& recorder, const std::vector<Semaphore*>& waits = {},
                                const std::vector<Semaphore*>& signals = {},
                                const Fence& fence = {}) const;

  void waitIdle() const;

  void reset();

  operator VkCommandBuffer() const { return _buffer; }
  operator const VkCommandBuffer*() const { return &_buffer; }

 private:
  void recordCommand(const Recorder& recorder, bool singleTime) const;
  void executeCommand(const std::vector<Semaphore*>& waits, const std::vector<Semaphore*>& signals,
                      const Fence& fence) const;

 private:
  VkCommandBuffer _buffer = VK_NULL_HANDLE;

  const CommandPool* _pool = nullptr;
};

NAMESPACE_VULKAN_END