/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "hal/SimDevice.h"

#include <wpi/SmallString.h>
#include <wpi/raw_ostream.h>

#include "HALInitializer.h"
#include "mockdata/SimDeviceDataInternal.h"

using namespace hal;

namespace hal {
namespace init {
void InitializeSimDevice() {}
}  // namespace init
}  // namespace hal

extern "C" {

HAL_SimDeviceHandle HAL_CreateSimDevice(const char* name) {
  hal::init::CheckInit();
  return SimSimDeviceData->CreateDevice(name);
}

void HAL_FreeSimDevice(HAL_SimDeviceHandle handle) {
  SimSimDeviceData->FreeDevice(handle);
}

HAL_SimValueHandle HAL_CreateSimValue(HAL_SimDeviceHandle device,
                                      const char* name, int32_t direction,
                                      const struct HAL_Value* initialValue) {
  return SimSimDeviceData->CreateValue(device, name, direction, 0, nullptr,
                                       nullptr, *initialValue);
}

HAL_SimValueHandle HAL_CreateSimValueEnum(HAL_SimDeviceHandle device,
                                          const char* name, int32_t direction,
                                          int32_t numOptions,
                                          const char** options,
                                          int32_t initialValue) {
  return SimSimDeviceData->CreateValue(device, name, direction, numOptions,
                                       options, nullptr,
                                       HAL_MakeEnum(initialValue));
}

HAL_SimValueHandle HAL_CreateSimValueEnumDouble(
    HAL_SimDeviceHandle device, const char* name, int32_t direction,
    int32_t numOptions, const char** options, const double* optionValues,
    int32_t initialValue) {
  return SimSimDeviceData->CreateValue(device, name, direction, numOptions,
                                       options, optionValues,
                                       HAL_MakeEnum(initialValue));
}

void HAL_GetSimValue(HAL_SimValueHandle handle, struct HAL_Value* value) {
  *value = SimSimDeviceData->GetValue(handle);
}

void HAL_SetSimValue(HAL_SimValueHandle handle, const struct HAL_Value* value) {
  SimSimDeviceData->SetValue(handle, *value);
}

hal::SimDevice::SimDevice(const char* name, int index) {
  wpi::SmallString<128> fullname;
  wpi::raw_svector_ostream os(fullname);
  os << name << '[' << index << ']';

  m_handle = HAL_CreateSimDevice(fullname.c_str());
}

hal::SimDevice::SimDevice(const char* name, int index, int channel) {
  wpi::SmallString<128> fullname;
  wpi::raw_svector_ostream os(fullname);
  os << name << '[' << index << ',' << channel << ']';

  m_handle = HAL_CreateSimDevice(fullname.c_str());
}

}  // extern "C"
