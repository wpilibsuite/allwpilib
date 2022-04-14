// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "camerasupport.h"

#ifdef _WIN32
#include "Windows.h"
#include "delayimp.h"
#pragma comment(lib, "delayimp.lib")
static int CheckDelayException(int exception_value) {
  if (exception_value ==
          VcppException(ERROR_SEVERITY_ERROR, ERROR_MOD_NOT_FOUND) ||
      exception_value ==
          VcppException(ERROR_SEVERITY_ERROR, ERROR_PROC_NOT_FOUND)) {
    // This example just executes the handler.
    return EXCEPTION_EXECUTE_HANDLER;
  }
  // Don't attempt to handle other errors
  return EXCEPTION_CONTINUE_SEARCH;
}
static bool TryDelayLoadAllImports(LPCSTR szDll) {
  __try {
    HRESULT hr = __HrLoadAllImportsForDll(szDll);
    if (FAILED(hr)) {
      return false;
    }
  } __except (CheckDelayException(GetExceptionCode())) {
    return false;
  }
  return true;
}
namespace glass {
bool HasCameraSupport() {
  bool hasCameraSupport = false;
  hasCameraSupport = TryDelayLoadAllImports("MF.dll");
  if (hasCameraSupport) {
    hasCameraSupport = TryDelayLoadAllImports("MFPlat.dll");
  }
  if (hasCameraSupport) {
    hasCameraSupport = TryDelayLoadAllImports("MFReadWrite.dll");
  }
  return hasCameraSupport;
}
}  // namespace glass
#else
namespace glass {
bool HasCameraSupport() {
  return true;
}
}  // namespace glass
#endif
