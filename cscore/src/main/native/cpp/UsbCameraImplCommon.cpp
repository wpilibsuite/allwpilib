// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "cscore_c.h"  // NOLINT(build/include_order)

#include <wpi/MemAlloc.h>

#include "c_util.h"
#include "cscore_cpp.h"

using namespace cs;

static void ConvertToC(CS_UsbCameraInfo* out, const UsbCameraInfo& in) {
  out->dev = in.dev;
  cs::ConvertToC(&out->path, in.path);
  cs::ConvertToC(&out->name, in.name);
  out->otherPaths = WPI_AllocateStringArray(in.otherPaths.size());
  out->otherPathsCount = in.otherPaths.size();
  for (size_t i = 0; i < in.otherPaths.size(); ++i) {
    cs::ConvertToC(&out->otherPaths[i], in.otherPaths[i]);
  }
  out->vendorId = in.vendorId;
  out->productId = in.productId;
}

static void FreeUsbCameraInfo(CS_UsbCameraInfo* info) {
  WPI_FreeString(&info->path);
  WPI_FreeString(&info->name);
  for (int i = 0; i < info->otherPathsCount; ++i) {
    WPI_FreeString(&info->otherPaths[i]);
  }
}

extern "C" {

CS_Source CS_CreateUsbCameraDev(const struct WPI_String* name, int dev,
                                CS_Status* status) {
  return cs::CreateUsbCameraDev(wpi::to_string_view(name), dev, status);
}

CS_Source CS_CreateUsbCameraPath(const struct WPI_String* name,
                                 const struct WPI_String* path,
                                 CS_Status* status) {
  return cs::CreateUsbCameraPath(wpi::to_string_view(name),
                                 wpi::to_string_view(path), status);
}

void CS_SetUsbCameraPath(CS_Source source, const struct WPI_String* path,
                         CS_Status* status) {
  cs::SetUsbCameraPath(source, wpi::to_string_view(path), status);
}

void CS_GetUsbCameraPath(CS_Source source, WPI_String* path,
                         CS_Status* status) {
  ConvertToC(path, cs::GetUsbCameraPath(source, status));
}

void CS_GetUsbCameraInfo(CS_Source source, CS_UsbCameraInfo* info,
                         CS_Status* status) {
  auto info_cpp = cs::GetUsbCameraInfo(source, status);
  ConvertToC(info, info_cpp);
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
}

}  // extern "C"
