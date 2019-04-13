/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "hal/Extensions.h"

#include <wpi/SmallString.h>
#include <wpi/StringRef.h>

#include "hal/HAL.h"

#if defined(WIN32) || defined(_WIN32)
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#if defined(WIN32) || defined(_WIN32)
#define DELIM ';'
#define HTYPE HMODULE
#define DLOPEN(a) LoadLibrary(a)
#define DLSYM GetProcAddress
#define DLCLOSE FreeLibrary
#else
#define DELIM ':'
#define HTYPE void*
#define PREFIX "lib"
#define DLOPEN(a) dlopen(a, RTLD_LAZY)
#define DLSYM dlsym
#define DLCLOSE dlclose
#endif

namespace hal {
namespace init {
void InitializeExtensions() {}
}  // namespace init
}  // namespace hal

extern "C" {

int HAL_LoadOneExtension(const char* library) {
  int rc = 1;  // It is expected and reasonable not to find an extra simulation
  HTYPE handle = DLOPEN(library);
#if !defined(WIN32) && !defined(_WIN32)
  if (!handle) {
    wpi::SmallString<128> libraryName("lib");
    libraryName += library;
#if defined(__APPLE__)
    libraryName += ".dylib";
#else
    libraryName += ".so";
#endif
    handle = DLOPEN(libraryName.c_str());
  }
#endif
  if (!handle) return rc;

  auto init = reinterpret_cast<halsim_extension_init_func_t*>(
      DLSYM(handle, "HALSIM_InitExtension"));

  if (init) rc = (*init)();

  if (rc != 0) DLCLOSE(handle);
  return rc;
}

int HAL_LoadExtensions(void) {
  int rc = 1;
  wpi::SmallVector<wpi::StringRef, 2> libraries;
  const char* e = std::getenv("HALSIM_EXTENSIONS");
  if (!e) return rc;
  wpi::StringRef env{e};
  env.split(libraries, DELIM, -1, false);
  for (auto& libref : libraries) {
    wpi::SmallString<128> library(libref);
    rc = HAL_LoadOneExtension(library.c_str());
    if (rc < 0) break;
  }
  return rc;
}

}  // extern "C"
