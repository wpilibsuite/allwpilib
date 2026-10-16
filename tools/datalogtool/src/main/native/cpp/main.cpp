// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <print>
#include <string_view>
#include <vector>

#include "App.hpp"

#ifndef RUNNING_IMGUI_TESTS
#ifdef _WIN32
int __stdcall WinMain(void* hInstance, void* hPrevInstance, char* pCmdLine,
                      int nCmdShow) {
  int argc = __argc;
  char** argv = __argv;
#else
int main(int argc, char** argv) {
#endif
  std::string_view saveDir;
  std::vector<std::string_view> filenames;
  bool parseOptions = true;
  for (int i = 1; i < argc; ++i) {
    std::string_view arg = argv[i];
    if (parseOptions && arg == "--save-dir") {
      if (++i == argc) {
        std::println(stderr, "--save-dir requires a directory argument");
        return 1;
      }
      saveDir = argv[i];
    } else if (parseOptions && arg == "--") {
      parseOptions = false;
    } else {
      filenames.emplace_back(arg);
    }
  }

  Application(saveDir, filenames);

  return 0;
}
#endif
