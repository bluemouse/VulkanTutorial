#pragma once

#include "helpers_debug.h"

#include <vulkan/vulkan.h>

#define MG_VERIFY_VKCMD(cmd) if (cmd != VK_SUCCESS) { throw std::runtime_error("Command failed: " #cmd " [" __FILE__": Ln " MG_STRINGIZE(__LINE__) "]"); }
#define MG_VERIFY_VKCMD_MSG(cmd, error_msg) if (cmd != VK_SUCCESS) { throw std::runtime_error(error_msg); }
