// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <vector>

#include "wpi/hal/simulation/AddressableLEDData.h"
#include "wpi/hal/simulation/SimCallbackRegistry.h"
#include "wpi/hal/simulation/SimDataValue.h"
#include "wpi/util/spinlock.hpp"

namespace hal {
class AddressableLEDData {
  HAL_SIMDATAVALUE_DEFINE_NAME(Initialized)
  HAL_SIMDATAVALUE_DEFINE_NAME(Start)
  HAL_SIMDATAVALUE_DEFINE_NAME(Length)

 public:
  SimDataValue<HAL_Bool, HAL_MakeBoolean, GetInitializedName> initialized{
      false};
  SimDataValue<int32_t, HAL_MakeInt, GetStartName> start{0};
  SimDataValue<int32_t, HAL_MakeInt, GetLengthName> length{0};

  void ResetData();
};
extern AddressableLEDData* SimAddressableLEDData;

class AddressableLEDDataBuffer {
  HAL_SIMDATAVALUE_DEFINE_NAME(Data)

  wpi::recursive_spinlock m_dataMutex;
  HAL_AddressableLEDData m_data[HAL_kAddressableLEDMaxLength];

 public:
  void SetData(int32_t start, int32_t len, const HAL_AddressableLEDData* d);
  int32_t GetData(int32_t start, int32_t len, HAL_AddressableLEDData* d);
  SimCallbackRegistry<HAL_ConstBufferCallback, GetDataName> data;
};
extern AddressableLEDDataBuffer* SimAddressableLEDDataBuffer;
}  // namespace hal
