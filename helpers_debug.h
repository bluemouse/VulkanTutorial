#pragma once

#include <stdexcept>

#define MG_STRINGIZE(x) MG_STRINGIZE2(x)
#define MG_STRINGIZE2(x) #x

#define MG_VERIFY(condition) if (!(condition)) { throw std::runtime_error("Condition failed: " #condition " [" __FILE__": Ln " MG_STRINGIZE(__LINE__) "]"); }
#define MG_VERIFY_MSG(condition) if (!(condition)) { throw std::runtime_error(error_msg); }

#if defined(_NDEBUG) || defined(NDEBUG)
# define MG_NOT_TESTED(function_name) __helpers_debug__::not_tested( # function_name, __FILE__, __LINE__)
# define MG_NOT_IMPLEMENTED(function_name) __helpers_debug__::not_implemented( # function_name, __FILE__, __LINE__)
# define MG_LOG(...) ((void)0)
# define MG_LOG_ERROR(...) ((void)0)
# define MG_LOG_WARNING(...) ((void)0)
# define MG_LOG_INFO(...) ((void)0)
# define MG_LOG_DEBUG(...) ((void)0)
#else
# define MG_NOT_TESTED(function_name) __helpers_debug__::not_tested( # function_name, __FILE__, __LINE__)
# define MG_NOT_IMPLEMENTED(function_name) __helpers_debug__::not_implemented( # function_name, __FILE__, __LINE__)
# define MG_LOG(...) __helpers_debug__::log("", __VA_ARGS__)
# define MG_LOG_ERROR(...) __helpers_debug__::log("ERROR", __VA_ARGS__)
# define MG_LOG_WARNING(...) __helpers_debug__::log("WARNING", __VA_ARGS__)
# define MG_LOG_INFO(...) __helpers_debug__::log("INFO", __VA_ARGS__)
# define MG_LOG_DEBUG(...) __helpers_debug__::log(__FILE__, __LINE__, "DEBUG", __VA_ARGS__)
#endif

#if defined(_NO_ASSERTIONS) || defined(_NDEBUG) || defined(NDEBUG)
# define MG_ASSERT(EX) ((void)0)
# define MG_ASSERT_MSG(EX,...) ((void)0)
# define MG_DEBUG_CODE(CODE)
#else
# define MG_ASSERT(EX) \
  ((EX) ? ((void)0) : __helpers_debug__::assertion_fail( # EX , __FILE__, __LINE__, nullptr))
# define MG_ASSERT_MSG(EX,...) \
  ((EX) ? ((void)0) : __helpers_debug__::assertion_fail( # EX , __FILE__, __LINE__, __VA_ARGS__))
# define MG_DEBUG_CODE(CODE) CODE
#endif

#if defined(_NO_WARNINGS) || defined(_NDEBUG) || defined(NDEBUG)
# define MG_WARNING(EX) ((void)0)
# define MG_WARNING_MSG(EX,...) ((void)0)
#else
# define MG_WARNING(EX) \
  ((EX) ? ((void)0) : __helpers_debug__::warning_fail( # EX , __FILE__, __LINE__, 0))
# define MG_WARNING_MSG(EX,...) \
  ((EX) ? ((void)0) : __helpers_debug__::warning_fail( # EX , __FILE__, __LINE__, __VA_ARGS__))
#endif

// Functions
namespace __helpers_debug__
{
  void not_tested(const char* func, const char* file, int line);
  void not_implemented(const char* func, const char* file, int line);
  void assertion_fail(const char* expr, const char* file, int line, const char* msg, ...);
  void warning_fail(const char* expr, const char* file, int line, const char* msg, ...);

  void log(const char* tag, const char* msg, ...);
  void log(const char* file, int line, const char* tag, const char* msg, ...);

  void log_backtraces();
} //namespace __helpers_debug__
