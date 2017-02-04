/**
 ******************************************************************************
 * Xenia : Xbox 360 Emulator Research Project                                 *
 ******************************************************************************
 * Copyright 2015 Ben Vanik. All rights reserved.                             *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

#include <gflags/gflags.h>

#include <cstdlib>
#include <cstring>
#include <locale>
#include <string>
#include <vector>

#define CATCH_CONFIG_RUNNER
#include "third_party/catch/include/catch.hpp"

namespace xe {

bool has_console_attached() { return true; }

// Used in console mode apps; automatically picked based on subsystem.
int Main(int argc, char* argv[]) {
  google::SetUsageMessage(std::string("usage: ..."));
  google::SetVersionString("1.0");

  // Parse flags; this may delete some of them.
  google::ParseCommandLineFlags(&argc, &argv, true);

  // Run Catch.
  int result = Catch::Session().run(argc, argv);

  google::ShutDownCommandLineFlags();
  return result;
}

}  // namespace xe

#if _WIN32
extern "C" int main(int argc, wchar_t* argv[]) {
  // Setup COM on the main thread.
  // NOTE: this may fail if COM has already been initialized - that's OK.
  CoInitializeEx(nullptr, COINIT_MULTITHREADED);

  // Convert all args to narrow, as gflags doesn't support wchar.
  int argca = argc;
  char** argva = (char**)alloca(sizeof(char*) * argca);
  for (int n = 0; n < argca; n++) {
    size_t len = wcslen(argv[n]);
    argva[n] = (char*)alloca(len + 1);
    std::wcstombs(argva[n], argv[n], len + 1);
  }
  return xe::Main(argc, argva);
}
#else
extern "C" int main(int argc, char* argv[]) { return xe::Main(argc, argv); }
#endif  // _WIN32
