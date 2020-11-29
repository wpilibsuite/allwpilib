/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <algorithm>
#include <cstring>

#include "../PortsInternal.h"
#include "AddressableLEDDataInternal.h"

using namespace hal;

namespace hal {
namespace init {
void InitializeAddressableLEDData() {
  static AddressableLEDData sad[kNumAddressableLEDs];
  ::hal::SimAddressableLEDData = sad;
}
}  // namespace init
}  // namespace hal

AddressableLEDData* hal::SimAddressableLEDData;

void AddressableLEDData::ResetData() {
  initialized.Reset(false);
  outputPort.Reset(-1);
  length.Reset(1);
  running.Reset(false);
  data.Reset();
  displayName.Reset();
}

void AddressableLEDData::SetData(const HAL_AddressableLEDData* d, int32_t len) {
  len = (std::min)(HAL_kAddressableLEDMaxLength, len);
  {
    std::scoped_lock lock(m_dataMutex);
    std::memcpy(m_data, d, len * sizeof(d[0]));
  }
  data(reinterpret_cast<const uint8_t*>(d), len * sizeof(d[0]));
}

int32_t AddressableLEDData::GetData(HAL_AddressableLEDData* d) {
  std::scoped_lock lock(m_dataMutex);
  int32_t len = length;
  if (d) std::memcpy(d, m_data, len * sizeof(d[0]));
  return len;
}

extern "C" {

int32_t HALSIM_FindAddressableLEDForChannel(int32_t channel) {
  for (int i = 0; i < kNumAddressableLEDs; ++i) {
    if (SimAddressableLEDData[i].initialized &&
        SimAddressableLEDData[i].outputPort == channel)
      return i;
  }
  return -1;
}

void HALSIM_ResetAddressableLEDData(int32_t index) {
  SimAddressableLEDData[index].ResetData();
}

const char* HALSIM_GetAddressableLEDDisplayName(int32_t index) {
  return SimAddressableLEDData[index].displayName.Get();
}
void HALSIM_SetAddressableLEDDisplayName(int32_t index,
                                         const char* displayName) {
  SimAddressableLEDData[index].displayName.Set(displayName);
}

int32_t HALSIM_GetAddressableLEDData(int32_t index,
                                     struct HAL_AddressableLEDData* data) {
  return SimAddressableLEDData[index].GetData(data);
}

void HALSIM_SetAddressableLEDData(int32_t index,
                                  const struct HAL_AddressableLEDData* data,
                                  int32_t length) {
  SimAddressableLEDData[index].SetData(data, length);
}

#define DEFINE_CAPI(TYPE, CAPINAME, LOWERNAME)                         \
  HAL_SIMDATAVALUE_DEFINE_CAPI(TYPE, HALSIM, AddressableLED##CAPINAME, \
                               SimAddressableLEDData, LOWERNAME)

DEFINE_CAPI(HAL_Bool, Initialized, initialized)
DEFINE_CAPI(int32_t, OutputPort, outputPort)
DEFINE_CAPI(int32_t, Length, length)
DEFINE_CAPI(HAL_Bool, Running, running)

#undef DEFINE_CAPI
#define DEFINE_CAPI(TYPE, CAPINAME, LOWERNAME)                                \
  HAL_SIMCALLBACKREGISTRY_DEFINE_CAPI(TYPE, HALSIM, AddressableLED##CAPINAME, \
                                      SimAddressableLEDData, LOWERNAME)

DEFINE_CAPI(HAL_ConstBufferCallback, Data, data)

#define REGISTER(NAME)                                                \
  SimAddressableLEDData[index].NAME.RegisterCallback(callback, param, \
                                                     initialNotify)

void HALSIM_RegisterAddressableLEDAllCallbacks(int32_t index,
                                               HAL_NotifyCallback callback,
                                               void* param,
                                               HAL_Bool initialNotify) {
  REGISTER(initialized);
  REGISTER(outputPort);
  REGISTER(length);
  REGISTER(running);
}
}  // extern "C"
