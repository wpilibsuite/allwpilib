// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/mutex.h>

#include "hal/ChipObject.h"
#include "hal/Types.h"

namespace hal {

/**
 * Proxy class for directly manipulating the DIO pins.
 *
 * This class is not copyable or movable, and should never be used
 * outside of the UnsafeManipulateDIO callback.
 */
struct DIOSetProxy {
  DIOSetProxy(const DIOSetProxy&) = delete;
  DIOSetProxy(DIOSetProxy&&) = delete;
  DIOSetProxy& operator=(const DIOSetProxy&) = delete;
  DIOSetProxy& operator=(DIOSetProxy&&) = delete;

  void SetOutputMode(int32_t* status) {
    m_dio->writeOutputEnable(m_setOutputDirReg, status);
  }

  void SetInputMode(int32_t* status) {
    m_dio->writeOutputEnable(m_unsetOutputDirReg, status);
  }

  void SetOutputTrue(int32_t* status) {
    m_dio->writeDO(m_setOutputStateReg, status);
  }

  void SetOutputFalse(int32_t* status) {
    m_dio->writeDO(m_unsetOutputStateReg, status);
  }

  tDIO::tOutputEnable m_setOutputDirReg;
  tDIO::tOutputEnable m_unsetOutputDirReg;
  tDIO::tDO m_setOutputStateReg;
  tDIO::tDO m_unsetOutputStateReg;
  tDIO* m_dio;
};
namespace detail {
wpi::mutex& UnsafeGetDIOMutex();
tDIO* UnsafeGetDigialSystem();
int32_t ComputeDigitalMask(HAL_DigitalHandle handle, int32_t* status);
}  // namespace detail

/**
 * Unsafe digital output set function
 * This function can be used to perform fast and determinstically set digital
 * outputs. This function holds the DIO lock, so calling anyting other then
 * functions on the Proxy object passed as a parameter can deadlock your
 * program.
 *
 * @param handle the HAL digital handle of the pin to toggle.
 * @param status status check
 * @param func   A functor taking a ref to a DIOSetProxy object.
 */
template <typename Functor>
void UnsafeManipulateDIO(HAL_DigitalHandle handle, int32_t* status,
                         Functor func) {
  auto port = digitalChannelHandles->Get(handle, HAL_HandleEnum::DIO);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  wpi::mutex& dioMutex = detail::UnsafeGetDIOMutex();
  tDIO* dSys = detail::UnsafeGetDigialSystem();
  auto mask = detail::ComputeDigitalMask(handle, status);
  if (*status != 0) {
    return;
  }
  std::scoped_lock lock(dioMutex);

  tDIO::tOutputEnable enableOE = dSys->readOutputEnable(status);
  enableOE.value |= mask;
  auto disableOE = enableOE;
  disableOE.value &= ~mask;
  tDIO::tDO enableDO = dSys->readDO(status);
  enableDO.value |= mask;
  auto disableDO = enableDO;
  disableDO.value &= ~mask;

  DIOSetProxy dioData{enableOE, disableOE, enableDO, disableDO, dSys};
  func(dioData);
}

}  // namespace hal
