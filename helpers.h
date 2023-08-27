#pragma once

#define STRINGIZE(x) STRINGIZE2(x)
#define STRINGIZE2(x) #x

#define VK_VERIFY(cmd) if (cmd != VK_SUCCESS) { throw std::runtime_error("Command failed: " #cmd " [" __FILE__": Ln " STRINGIZE(__LINE__) "]"); }
#define VK_VERIFY_MSG(cmd, error_msg) if (cmd != VK_SUCCESS) { throw std::runtime_error(error_msg); }
