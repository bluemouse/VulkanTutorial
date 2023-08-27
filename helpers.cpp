#include "helpers.h"

#include <iostream>
#include <cassert>
#include <cstring>
#include <stdio.h>
#include <stdarg.h>

#include <execinfo.h>
#include <signal.h>
#include <unistd.h>

using std::cerr;
using std::endl;
using std::strlen;

namespace __helpers__ {

void not_tested(const char* func, const char* file, int line)
{
#ifdef _NDEBUG
  log("ERROR", "%s [file(ln): %s(%d)] is not tested", func, file, line);
#else
  log("WARNING", "%s [file(ln): %s(%d)] is not tested", func, file, line);
#endif
}

void not_implemented(const char* func, const char* file, int line)
{
  cerr << "Error: " << func << " is not implemented!" << endl
       << "  File(Line): " << file << " ("<< line << ")" << endl;
#ifndef _NDEBUG
  abort();
#endif
}

void assertion_fail(const char* expr, const char* file, int line, const char* msg, ...)
{
  cerr << "Error: assertion violation!" << endl
       << "  Condition: " << expr << endl
       << "  File(Line): " << file << " (" << line << ")" << endl;

  if (msg != nullptr) {
    size_t msg_len = strlen(msg) + 160;
    char* buffer = new char[msg_len];
    va_list args;
    va_start(args, msg);
    vsnprintf(buffer, msg_len, msg, args);
    va_end(args);
    cerr << "Message:" << buffer << endl;
    delete[] buffer;
  }
  log_backtraces();
  abort();
}

void warning_fail(const char* expr, const char* file, int line, const char* msg, ...)
{
  cerr << "Warning: check violation!" << endl
       << "  Condition: " << expr << endl
       << "  File(Line): " << file << " (" << line << ")" << endl;

  if (msg != 0) {
    size_t msg_len = strlen(msg) + 160;
    char* buffer = new char[msg_len];
    va_list args;
    va_start(args, msg);
    vsnprintf(buffer, msg_len, msg, args);
    va_end(args);
    cerr << "Message:" << buffer << endl;
    delete[] buffer;
  }
}

void log(const char* tag, const char* msg, ...)
{
  size_t msg_len = strlen(msg) + 160;
  char* buffer = new char[msg_len];
  va_list args;
  va_start(args, msg);
  vsnprintf(buffer, msg_len, msg, args);
  va_end(args);
  if (tag != nullptr) {
    fprintf(stderr, "%s: ", tag);
  }
  fprintf(stderr, "%s\n", buffer);
  delete[] buffer;
}

void log(const char* file, int line, const char* tag, const char* msg, ...)
{
  size_t msg_len = strlen(msg) + 160;
  char* buffer = new char[msg_len];
  va_list args;
  va_start(args, msg);
  vsnprintf(buffer, msg_len, msg, args);
  va_end(args);

  fprintf(stderr, "%s [%s Ln %i]: ", tag, file, line);
  fprintf(stderr, "%s\n", buffer);
  delete[] buffer;
}

void log_backtraces()
{
  void *array[10];
  size_t num_traces = backtrace(array, 10);
  char** traces = backtrace_symbols(array, num_traces);
  for (size_t i = 0; i < num_traces; ++i) {
    fprintf(stderr, "%s\n", traces[i]);
  }
  free(traces);
}

} //namespace __helpers__
