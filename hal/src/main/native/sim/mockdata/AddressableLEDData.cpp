// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <algorithm>
#include <cstring>

#include "../PortsInternal.h"
#include "AddressableLEDDataInternal.h"

using namespace hal;

namespace hal::init {
void InitializeAddressableLEDData() {
  static AddressableLEDData sad[kNumAddressableLEDs];
  ::hal::SimAddressableLEDData = sad;
  static AddressableLEDDataBuffer buf;
  ::hal::SimAddressableLEDDataBuffer = &buf;
}
}  // namespace hal::init

AddressableLEDData* hal::SimAddressableLEDData;
AddressableLEDDataBuffer* hal::SimAddressableLEDDataBuffer;

void AddressableLEDData::ResetData() {
  initialized.Reset(false);
  start.Reset(0);
  length.Reset(0);
}

void AddressableLEDDataBuffer::SetData(int32_t start, int32_t len,
                                       const HAL_AddressableLEDData* d) {
  len = (std::min)(HAL_kAddressableLEDMaxLength, len - start);
  if (len < 0) {
    return;
  }
  {
    std::scoped_lock lock(m_dataMutex);
    std::memcpy(&m_data[start], d, len * sizeof(d[0]));
  }
  data(reinterpret_cast<const uint8_t*>(d), len * sizeof(d[0]));
}

int32_t AddressableLEDDataBuffer::GetData(int32_t start, int32_t len,
                                          HAL_AddressableLEDData* d) {
  len = (std::min)(HAL_kAddressableLEDMaxLength, len - start);
  if (len < 0) {
    return 0;
  }
  std::scoped_lock lock(m_dataMutex);
  if (d) {
    std::memcpy(d, &m_data[start], len * sizeof(d[0]));
  }
  return len;
}

extern "C" {

void HALSIM_ResetAddressableLEDData(int32_t index) {
  SimAddressableLEDData[index].ResetData();
}

int32_t HALSIM_GetAddressableLEDData(int32_t start, int32_t length,
                                     struct HAL_AddressableLEDData* data) {
  return SimAddressableLEDDataBuffer->GetData(start, length, data);
}

void HALSIM_SetAddressableLEDData(int32_t start, int32_t length,
                                  const struct HAL_AddressableLEDData* data) {
  SimAddressableLEDDataBuffer->SetData(start, length, data);
}

#define DEFINE_CAPI(TYPE, CAPINAME, LOWERNAME)                         \
  HAL_SIMDATAVALUE_DEFINE_CAPI(TYPE, HALSIM, AddressableLED##CAPINAME, \
                               SimAddressableLEDData, LOWERNAME)

DEFINE_CAPI(HAL_Bool, Initialized, initialized)
DEFINE_CAPI(int32_t, Start, start)
DEFINE_CAPI(int32_t, Length, length)

#undef DEFINE_CAPI
#define DEFINE_CAPI(TYPE, CAPINAME, LOWERNAME)                             \
  HAL_SIMCALLBACKREGISTRY_DEFINE_CAPI_NOINDEX(                             \
      TYPE, HALSIM, AddressableLED##CAPINAME, SimAddressableLEDDataBuffer, \
      LOWERNAME)

DEFINE_CAPI(HAL_ConstBufferCallback, Data, data)

#define REGISTER(NAME)                                                \
  SimAddressableLEDData[index].NAME.RegisterCallback(callback, param, \
                                                     initialNotify)

void HALSIM_RegisterAddressableLEDAllCallbacks(int32_t index,
                                               HAL_NotifyCallback callback,
                                               void* param,
                                               HAL_Bool initialNotify) {
  REGISTER(initialized);
  REGISTER(start);
  REGISTER(length);
}
}  // extern "C"
