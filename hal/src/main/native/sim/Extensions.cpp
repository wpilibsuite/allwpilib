// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hal/Extensions.h"

#include <cstdio>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "wpi/util/StringExtras.hpp"
#include "wpi/util/fs.hpp"
#include "wpi/util/print.hpp"
#include "wpi/util/spinlock.hpp"

#if defined(WIN32) || defined(_WIN32)
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#if defined(WIN32) || defined(_WIN32)
#define DELIM ';'
#define HTYPE HMODULE
#define DLOPEN(a) LoadLibraryA(a)
#define DLSYM GetProcAddress
#define DLCLOSE FreeLibrary
#define DLERROR fmt::format("error #{}", GetLastError())
#else
#define DELIM ':'
#define HTYPE void*
#define PREFIX "lib"
#define DLOPEN(a) dlopen(a, RTLD_LAZY)
#define DLSYM dlsym
#define DLCLOSE dlclose
#define DLERROR dlerror()
#endif

static wpi::util::recursive_spinlock gExtensionRegistryMutex;
static std::vector<std::pair<const char*, void*>> gExtensionRegistry;
static std::vector<std::pair<void*, void (*)(void*, const char*, void*)>>
    gExtensionListeners;

namespace wpi::hal::init {
void InitializeExtensions() {}
}  // namespace wpi::hal::init

static bool& GetShowNotFoundMessage() {
  static bool showMsg = true;
  return showMsg;
}

extern "C" {

int HAL_LoadOneExtension(const char* library) {
  int rc = 1;  // It is expected and reasonable not to find an extra simulation
  wpi::util::print("HAL Extensions: Attempting to load: {}\n",
                   fs::path{library}.stem().string());
  std::fflush(stdout);
  HTYPE handle = DLOPEN(library);
#if !defined(WIN32) && !defined(_WIN32)
  if (!handle) {
#if defined(__APPLE__)
    auto libraryName = fmt::format("lib{}.dylib", library);
#else
    auto libraryName = fmt::format("lib{}.so", library);
#endif
    wpi::util::print(
        "HAL Extensions: Load failed: {}\nTrying modified name: {}\n", DLERROR,
        fs::path{libraryName}.stem().string());
    std::fflush(stdout);
    handle = DLOPEN(libraryName.c_str());
  }
#endif
  if (!handle) {
    wpi::util::print("HAL Extensions: Failed to load library: {}\n", DLERROR);
    std::fflush(stdout);
    return rc;
  }

  auto init = reinterpret_cast<halsim_extension_init_func_t*>(
      DLSYM(handle, "HALSIM_InitExtension"));

  if (init) {
    rc = (*init)();
  }

  if (rc != 0) {
    std::puts("HAL Extensions: Failed to load extension");
    std::fflush(stdout);
    DLCLOSE(handle);
  } else {
    std::puts("HAL Extensions: Successfully loaded extension");
    std::fflush(stdout);
  }
  return rc;
}

int HAL_LoadExtensions(void) {
  int rc = 1;
  const char* e = std::getenv("HALSIM_EXTENSIONS");
  if (!e) {
    if (GetShowNotFoundMessage()) {
      std::puts("HAL Extensions: No extensions found");
      std::fflush(stdout);
    }
    return rc;
  }
  wpi::util::split(e, DELIM, -1, false, [&](auto library) {
    if (rc >= 0) {
      rc = HAL_LoadOneExtension(std::string(library).c_str());
    }
  });
  return rc;
}

void HAL_RegisterExtension(const char* name, void* data) {
  std::scoped_lock lock(gExtensionRegistryMutex);
  gExtensionRegistry.emplace_back(name, data);
  for (auto&& listener : gExtensionListeners) {
    listener.second(listener.first, name, data);
  }
}

void HAL_RegisterExtensionListener(void* param,
                                   void (*func)(void*, const char* name,
                                                void* data)) {
  std::scoped_lock lock(gExtensionRegistryMutex);
  gExtensionListeners.emplace_back(param, func);
  for (auto&& extension : gExtensionRegistry) {
    func(param, extension.first, extension.second);
  }
}

void HAL_SetShowExtensionsNotFoundMessages(HAL_Bool showMessage) {
  GetShowNotFoundMessage() = showMessage;
}

}  // extern "C"
