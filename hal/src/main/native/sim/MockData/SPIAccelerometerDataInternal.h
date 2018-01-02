/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <atomic>
#include <memory>

#include <support/mutex.h>

#include "MockData/NotifyListenerVector.h"
#include "MockData/SPIAccelerometerData.h"

namespace hal {
class SPIAccelerometerData {
 public:
  int32_t RegisterActiveCallback(HAL_NotifyCallback callback, void* param,
                                 HAL_Bool initialNotify);
  void CancelActiveCallback(int32_t uid);
  void InvokeActiveCallback(HAL_Value value);
  HAL_Bool GetActive();
  void SetActive(HAL_Bool active);

  int32_t RegisterRangeCallback(HAL_NotifyCallback callback, void* param,
                                HAL_Bool initialNotify);
  void CancelRangeCallback(int32_t uid);
  void InvokeRangeCallback(HAL_Value value);
  int32_t GetRange();
  void SetRange(int32_t range);

  int32_t RegisterXCallback(HAL_NotifyCallback callback, void* param,
                            HAL_Bool initialNotify);
  void CancelXCallback(int32_t uid);
  void InvokeXCallback(HAL_Value value);
  double GetX();
  void SetX(double x);

  int32_t RegisterYCallback(HAL_NotifyCallback callback, void* param,
                            HAL_Bool initialNotify);
  void CancelYCallback(int32_t uid);
  void InvokeYCallback(HAL_Value value);
  double GetY();
  void SetY(double y);

  int32_t RegisterZCallback(HAL_NotifyCallback callback, void* param,
                            HAL_Bool initialNotify);
  void CancelZCallback(int32_t uid);
  void InvokeZCallback(HAL_Value value);
  double GetZ();
  void SetZ(double z);

  virtual void ResetData();

 private:
  wpi::mutex m_registerMutex;
  std::atomic<HAL_Bool> m_active{false};
  std::shared_ptr<NotifyListenerVector> m_activeCallbacks = nullptr;
  std::atomic<int32_t> m_range{0};
  std::shared_ptr<NotifyListenerVector> m_rangeCallbacks = nullptr;
  std::atomic<double> m_x{0.0};
  std::shared_ptr<NotifyListenerVector> m_xCallbacks = nullptr;
  std::atomic<double> m_y{0.0};
  std::shared_ptr<NotifyListenerVector> m_yCallbacks = nullptr;
  std::atomic<double> m_z{0.0};
  std::shared_ptr<NotifyListenerVector> m_zCallbacks = nullptr;
};
extern SPIAccelerometerData* SimSPIAccelerometerData;
}  // namespace hal
