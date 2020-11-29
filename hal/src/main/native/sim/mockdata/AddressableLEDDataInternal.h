/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <vector>

#include <wpi/spinlock.h>

#include "hal/simulation/AddressableLEDData.h"
#include "hal/simulation/SimCallbackRegistry.h"
#include "hal/simulation/SimDataValue.h"
#include "hal/simulation/SimDisplayName.h"

namespace hal {
class AddressableLEDData {
  HAL_SIMDATAVALUE_DEFINE_NAME(Initialized)
  HAL_SIMDATAVALUE_DEFINE_NAME(OutputPort)
  HAL_SIMDATAVALUE_DEFINE_NAME(Length)
  HAL_SIMDATAVALUE_DEFINE_NAME(Running)
  HAL_SIMDATAVALUE_DEFINE_NAME(Data)

  wpi::recursive_spinlock m_dataMutex;
  HAL_AddressableLEDData m_data[HAL_kAddressableLEDMaxLength];

 public:
  void SetData(const HAL_AddressableLEDData* d, int32_t len);
  int32_t GetData(HAL_AddressableLEDData* d);

  SimDisplayName displayName;
  SimDataValue<HAL_Bool, HAL_MakeBoolean, GetInitializedName> initialized{
      false};
  SimDataValue<int32_t, HAL_MakeInt, GetOutputPortName> outputPort{-1};
  SimDataValue<int32_t, HAL_MakeInt, GetLengthName> length{1};
  SimDataValue<HAL_Bool, HAL_MakeBoolean, GetRunningName> running{false};
  SimCallbackRegistry<HAL_ConstBufferCallback, GetDataName> data;

  void ResetData();
};
extern AddressableLEDData* SimAddressableLEDData;
}  // namespace hal
