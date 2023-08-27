#pragma once

#include <stdexcept>

#define STRINGIZE(x) STRINGIZE2(x)
#define STRINGIZE2(x) #x

#define VK_VERIFY(cmd) if (cmd != VK_SUCCESS) { throw std::runtime_error("Command failed: " #cmd " [" __FILE__": Ln " STRINGIZE(__LINE__) "]"); }
#define VK_VERIFY_MSG(cmd, error_msg) if (cmd != VK_SUCCESS) { throw std::runtime_error(error_msg); }


#if defined(_NDEBUG) || defined(NDEBUG)
# define NOT_TESTED(function_name) __helpers__::not_tested( # function_name, __FILE__, __LINE__)
# define NOT_IMPLEMENTED(function_name) __helpers__::not_implemented( # function_name, __FILE__, __LINE__)
# define LOG(...) ((void)0)
# define LOG_ERROR(...) ((void)0)
# define LOG_WARNING(...) ((void)0)
# define LOG_INFO(...) ((void)0)
# define LOG_DEBUG(...) ((void)0)
#else
# define NOT_TESTED(function_name) __helpers__::not_tested( # function_name, __FILE__, __LINE__)
# define NOT_IMPLEMENTED(function_name) __helpers__::not_implemented( # function_name, __FILE__, __LINE__)
# define LOG(...) __helpers__::log("", __VA_ARGS__)
# define LOG_ERROR(...) __helpers__::log("ERROR", __VA_ARGS__)
# define LOG_WARNING(...) __helpers__::log("WARNING", __VA_ARGS__)
# define LOG_INFO(...) __helpers__::log("INFO", __VA_ARGS__)
# define LOG_DEBUG(...) __helpers__::log(__FILE__, __LINE__, "DEBUG", __VA_ARGS__)
#endif

#if defined(_NO_ASSERTIONS) || defined(_NDEBUG) || defined(NDEBUG)
# define ASSERT(EX) ((void)0)
# define ASSERT_MSG(EX,...) ((void)0)
# define DEBUG_CODE(CODE)
# define VERIFY(EX) (EX)
#else
# define ASSERT(EX) \
  ((EX) ? ((void)0) : __helpers__::assertion_fail( # EX , __FILE__, __LINE__, nullptr))
# define ASSERT_MSG(EX,...) \
  ((EX) ? ((void)0) : __helpers__::assertion_fail( # EX , __FILE__, __LINE__, __VA_ARGS__))
# define DEBUG_CODE(CODE) CODE
# define VERIFY(EX) \
  ((EX) ? ((void)0) : __helpers__::assertion_fail( # EX , __FILE__, __LINE__, nullptr))
#endif

#if defined(_NO_WARNINGS) || defined(_NDEBUG) || defined(NDEBUG)
# define WARNING(EX) ((void)0)
# define WARNING_MSG(EX,...) ((void)0)
#else
# define WARNING(EX) \
  ((EX) ? ((void)0) : __helpers__::warning_fail( # EX , __FILE__, __LINE__, 0))
# define WARNING_MSG(EX,...) \
  ((EX) ? ((void)0) : __helpers__::warning_fail( # EX , __FILE__, __LINE__, __VA_ARGS__))
#endif

#if defined(_NO_RELEASE_LOG)
# define RELEASE_LOG_INFO(...) ((void)0)
#else
# define RELEASE_LOG_INFO(...) __helpers__::log("INFO", __VA_ARGS__)
#endif

// Functions
namespace __helpers__
{
  void not_tested(const char* func, const char* file, int line);
  void not_implemented(const char* func, const char* file, int line);
  void assertion_fail(const char* expr, const char* file, int line, const char* msg, ...);
  void warning_fail(const char* expr, const char* file, int line, const char* msg, ...);

  void log(const char* tag, const char* msg, ...);
  void log(const char* file, int line, const char* tag, const char* msg, ...);

  void log_backtraces();
} //namespace __helpers__
