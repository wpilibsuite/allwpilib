// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpigui_openurl.h"

#if _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#include <Windows.h>
#include <shellapi.h>
#else
#include <unistd.h>
#endif

#include <string>

void wpi::gui::OpenURL(const std::string& url) {
#ifdef _WIN32
  ShellExecuteA(nullptr, "open", url.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
#else
#ifdef __APPLE__
  static constexpr const char* opencmd = "open";
#else
  static constexpr const char* opencmd = "xdg-open";
#endif
  // If we forked into the child process, run execlp(), which replaces the
  // current process image
  if (fork() == 0) {
    execlp(opencmd, opencmd, url.c_str(), static_cast<const char*>(nullptr));
  }
#endif
}
