// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

// clang-format off
#include "wpi/cs/cscore_c.h"
// clang-format on

#include "c_util.hpp"
#include "wpi/cs/cscore_cpp.hpp"
#include "wpi/util/MemAlloc.hpp"

using namespace wpi::cs;

static void ConvertToC(CS_UsbCameraInfo* out, const UsbCameraInfo& in) {
  out->dev = in.dev;
  wpi::cs::ConvertToC(&out->path, in.path);
  wpi::cs::ConvertToC(&out->name, in.name);
  out->otherPaths = WPI_AllocateStringArray(in.otherPaths.size());
  out->otherPathsCount = in.otherPaths.size();
  for (size_t i = 0; i < in.otherPaths.size(); ++i) {
    wpi::cs::ConvertToC(&out->otherPaths[i], in.otherPaths[i]);
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
  return wpi::cs::CreateUsbCameraDev(wpi::util::to_string_view(name), dev,
                                     status);
}

CS_Source CS_CreateUsbCameraPath(const struct WPI_String* name,
                                 const struct WPI_String* path,
                                 CS_Status* status) {
  return wpi::cs::CreateUsbCameraPath(wpi::util::to_string_view(name),
                                      wpi::util::to_string_view(path), status);
}

void CS_SetUsbCameraPath(CS_Source source, const struct WPI_String* path,
                         CS_Status* status) {
  wpi::cs::SetUsbCameraPath(source, wpi::util::to_string_view(path), status);
}

void CS_GetUsbCameraPath(CS_Source source, WPI_String* path,
                         CS_Status* status) {
  ConvertToC(path, wpi::cs::GetUsbCameraPath(source, status));
}

void CS_GetUsbCameraInfo(CS_Source source, CS_UsbCameraInfo* info,
                         CS_Status* status) {
  auto info_cpp = wpi::cs::GetUsbCameraInfo(source, status);
  ConvertToC(info, info_cpp);
}

CS_UsbCameraInfo* CS_EnumerateUsbCameras(int* count, CS_Status* status) {
  auto cameras = wpi::cs::EnumerateUsbCameras(status);
  CS_UsbCameraInfo* out = static_cast<CS_UsbCameraInfo*>(
      wpi::util::safe_malloc(cameras.size() * sizeof(CS_UsbCameraInfo)));
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
