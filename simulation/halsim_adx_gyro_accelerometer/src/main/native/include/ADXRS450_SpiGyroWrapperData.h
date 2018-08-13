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
class ADXRS450_SpiGyroWrapper {
 public:
  explicit ADXRS450_SpiGyroWrapper(int port);
  virtual ~ADXRS450_SpiGyroWrapper();

  void HandleRead(uint8_t* buffer, uint32_t count);
  void HandleAutoReceiveData(uint8_t* buffer, int32_t numToRead,
                             int32_t& outputCount);

  virtual void ResetData();

  int32_t RegisterAngleCallback(HAL_NotifyCallback callback, void* param,
                                HAL_Bool initialNotify);
  void CancelAngleCallback(int32_t uid);
  void InvokeAngleCallback(HAL_Value value);
  double GetAngle();
  void SetAngle(double angle);

 private:
  int m_port;
  int m_readCallbackId;
  int m_autoReceiveReadCallbackId;

  wpi::mutex m_registerMutex;
  wpi::mutex m_dataMutex;
  double m_angle = 0.0;
  double m_angleDiff = 0.0;
  std::shared_ptr<NotifyListenerVector> m_angleCallbacks = nullptr;

  static const double kAngleLsb;
  // The maximum difference that can fit inside of the shifted and masked data
  // field, per transaction
  static const double kMaxAngleDeltaPerMessage;
  static const int kPacketSize;
};
}  // namespace hal
