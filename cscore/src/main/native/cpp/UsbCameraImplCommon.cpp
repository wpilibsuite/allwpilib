// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "cscore_c.h"  // NOLINT(build/include_order)

#include "c_util.h"
#include "cscore_cpp.h"

using namespace cs;

static void ConvertToC(CS_UsbCameraInfo* out, const UsbCameraInfo& in) {
  out->dev = in.dev;
  out->path = ConvertToC(in.path);
  out->name = ConvertToC(in.name);
  out->otherPaths = static_cast<char**>(
      wpi::safe_malloc(in.otherPaths.size() * sizeof(char*)));
  out->otherPathsCount = in.otherPaths.size();
  for (size_t i = 0; i < in.otherPaths.size(); ++i) {
    out->otherPaths[i] = cs::ConvertToC(in.otherPaths[i]);
  }
  out->vendorId = in.vendorId;
  out->productId = in.productId;
}

static void FreeUsbCameraInfo(CS_UsbCameraInfo* info) {
  std::free(info->path);
  std::free(info->name);
  for (int i = 0; i < info->otherPathsCount; ++i) {
    std::free(info->otherPaths[i]);
  }
  std::free(info->otherPaths);
}

extern "C" {

CS_Source CS_CreateUsbCameraDev(const char* name, int dev, CS_Status* status) {
  return cs::CreateUsbCameraDev(name, dev, status);
}

CS_Source CS_CreateUsbCameraPath(const char* name, const char* path,
                                 CS_Status* status) {
  return cs::CreateUsbCameraPath(name, path, status);
}

void CS_SetUsbCameraPath(CS_Source source, const char* path,
                         CS_Status* status) {
  cs::SetUsbCameraPath(source, path, status);
}

char* CS_GetUsbCameraPath(CS_Source source, CS_Status* status) {
  return ConvertToC(cs::GetUsbCameraPath(source, status));
}

CS_UsbCameraInfo* CS_GetUsbCameraInfo(CS_Source source, CS_Status* status) {
  auto info = cs::GetUsbCameraInfo(source, status);
  if (*status != CS_OK) {
    return nullptr;
  }
  CS_UsbCameraInfo* out = static_cast<CS_UsbCameraInfo*>(
      wpi::safe_malloc(sizeof(CS_UsbCameraInfo)));
  ConvertToC(out, info);
  return out;
}

CS_UsbCameraInfo* CS_EnumerateUsbCameras(int* count, CS_Status* status) {
  auto cameras = cs::EnumerateUsbCameras(status);
  CS_UsbCameraInfo* out = static_cast<CS_UsbCameraInfo*>(
      wpi::safe_malloc(cameras.size() * sizeof(CS_UsbCameraInfo)));
  *count = cameras.size();
  for (size_t i = 0; i < cameras.size(); ++i) {
    ConvertToC(&out[i], cameras[i]);
  }
  return out;
}

void CS_FreeEnumeratedUsbCameras(CS_UsbCameraInfo* cameras, int count) {
  if (!cameras) {
    return;
  }
  for (int i = 0; i < count; ++i) {
    FreeUsbCameraInfo(&cameras[i]);
  }
  std::free(cameras);
}

void CS_FreeUsbCameraInfo(CS_UsbCameraInfo* info) {
  if (!info) {
    return;
  }
  FreeUsbCameraInfo(info);
  std::free(info);
}

}  // extern "C"
