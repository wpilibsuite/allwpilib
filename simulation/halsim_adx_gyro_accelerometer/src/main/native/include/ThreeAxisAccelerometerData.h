/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <atomic>
#include <memory>

#include <mockdata/NotifyListenerVector.h>
#include <wpi/mutex.h>

namespace hal {
class ThreeAxisAccelerometerData {
 public:
  ThreeAxisAccelerometerData();
  virtual ~ThreeAxisAccelerometerData();

  virtual bool GetInitialized() const = 0;

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

 protected:
  wpi::mutex m_registerMutex;
  std::atomic<double> m_x{0.0};
  std::shared_ptr<NotifyListenerVector> m_xCallbacks = nullptr;
  std::atomic<double> m_y{0.0};
  std::shared_ptr<NotifyListenerVector> m_yCallbacks = nullptr;
  std::atomic<double> m_z{0.0};
  std::shared_ptr<NotifyListenerVector> m_zCallbacks = nullptr;
};
}  // namespace hal
