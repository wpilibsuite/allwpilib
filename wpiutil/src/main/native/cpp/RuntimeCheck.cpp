// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/RuntimeCheck.h"

#ifdef _WIN32
#include <cstdio>
#include <memory>

#include "Windows.h"
extern "C" int32_t WPI_IsRuntimeValid(uint32_t* foundMajor,
                                      uint32_t* foundMinor,
                                      uint32_t* expectedMajor,
                                      uint32_t* expectedMinor,
                                      WPI_String* runtimePath) {
  HMODULE msvcRuntimeModule = GetModuleHandleW(L"msvcp140.dll");
  if (!msvcRuntimeModule) {
    return 1;
  }
  HRSRC versionResource = FindResourceW(msvcRuntimeModule, MAKEINTRESOURCEW(1),
                                        MAKEINTRESOURCEW(16));
  if (!versionResource) {
    return 1;
  }
  HGLOBAL loadedVersion = LoadResource(msvcRuntimeModule, versionResource);
  if (!loadedVersion) {
    return 1;
  }
  // No need to unlock. Thats vestigial of windows before my time...
  LPVOID lockedResource = LockResource(loadedVersion);
  if (!lockedResource) {
    return 1;
  }
  DWORD resourceSize = SizeofResource(msvcRuntimeModule, versionResource);
  if (resourceSize == 0) {
    return 1;
  }
  std::unique_ptr<char[]> resourceBuffer =     // NOLINT
      std::make_unique<char[]>(resourceSize);  // NOLINT
  std::memcpy(resourceBuffer.get(), lockedResource, resourceSize);
  VS_FIXEDFILEINFO* fileInfo = nullptr;
  UINT fileInfoLen = 0;
  if (!VerQueryValueW(resourceBuffer.get(), L"\\",
                      reinterpret_cast<void**>(&fileInfo), &fileInfoLen)) {
    return 1;
  }
  *foundMajor = HIWORD(fileInfo->dwProductVersionMS);
  *foundMinor = LOWORD(fileInfo->dwProductVersionMS);
  *expectedMajor = 14;
  *expectedMinor = 40;
  bool ValidRuntime =
      *foundMajor != *expectedMajor || *foundMinor >= *expectedMinor;
  if (!ValidRuntime) {
    DWORD Size = MAX_PATH;
    char* ValidBuffer = WPI_AllocateString(runtimePath, Size);
    DWORD RetVal = GetModuleFileNameA(msvcRuntimeModule, ValidBuffer, Size);
    while (RetVal == Size) {
      Size *= 2;
      WPI_FreeString(runtimePath);
      ValidBuffer = WPI_AllocateString(runtimePath, Size);
      RetVal = GetModuleFileNameA(msvcRuntimeModule, ValidBuffer, Size);
    }
    runtimePath->len = RetVal;
  }
  return ValidRuntime ? 1 : 0;
}
#else
extern "C" int32_t WPI_IsRuntimeValid(uint32_t*, uint32_t*, uint32_t*,
                                      uint32_t*, WPI_String*) {
  return 1;
}
#endif
