/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <mockdata/SimDataValue.h>

namespace hal {
class ADXRS450_SpiGyroWrapper {
 public:
  explicit ADXRS450_SpiGyroWrapper(int port);
  virtual ~ADXRS450_SpiGyroWrapper();

  bool GetInitialized() const;

  void HandleRead(uint8_t* buffer, uint32_t count);
  void HandleAutoReceiveData(uint32_t* buffer, int32_t numToRead,
                             int32_t& outputCount);

  int32_t RegisterAngleCallback(HAL_NotifyCallback callback, void* param,
                                HAL_Bool initialNotify) {
    return m_angle.RegisterCallback(callback, param, initialNotify);
  }
  void CancelAngleCallback(int32_t uid) { m_angle.CancelCallback(uid); }
  double GetAngle() { return m_angle; }
  void SetAngle(double angle);

  virtual void ResetData();

 private:
  int m_port;
  int m_readCallbackId;
  int m_autoReceiveReadCallbackId;

  HAL_SIMDATAVALUE_DEFINE_NAME(Angle)

  SimDataValue<double, MakeDouble, GetAngleName> m_angle{0.0};
  double m_angleDiff = 0.0;

  static const double kAngleLsb;
  // The maximum difference that can fit inside of the shifted and masked data
  // field, per transaction
  static const double kMaxAngleDeltaPerMessage;
  static const int kPacketSize;
};
}  // namespace hal
