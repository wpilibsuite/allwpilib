/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>

#include <HAL/Types.h>
#include <llvm/raw_ostream.h>

#include "ErrorBase.h"
#include "MotorSafety.h"
#include "SmartDashboard/SendableBase.h"

namespace frc {

class MotorSafetyHelper;

/**
 * Class for Spike style relay outputs.
 *
 * Relays are intended to be connected to spikes or similar relays. The relay
 * channels controls a pair of pins that are either both off, one on, the other
 * on, or both on. This translates into two spike outputs at 0v, one at 12v and
 * one at 0v, one at 0v and the other at 12v, or two spike outputs at 12V. This
 * allows off, full forward, or full reverse control of motors without variable
 * speed. It also allows the two channels (forward and reverse) to be used
 * independently for something that does not care about voltage polarity (like
 * a solenoid).
 */
class Relay : public MotorSafety, public ErrorBase, public SendableBase {
 public:
  enum Value { kOff, kOn, kForward, kReverse };
  enum Direction { kBothDirections, kForwardOnly, kReverseOnly };

  explicit Relay(int channel, Direction direction = kBothDirections);
  ~Relay() override;

  void Set(Value value);
  Value Get() const;
  int GetChannel() const;

  void SetExpiration(double timeout) override;
  double GetExpiration() const override;
  bool IsAlive() const override;
  void StopMotor() override;
  bool IsSafetyEnabled() const override;
  void SetSafetyEnabled(bool enabled) override;
  void GetDescription(llvm::raw_ostream& desc) const override;

  void InitSendable(SendableBuilder& builder) override;

 private:
  int m_channel;
  Direction m_direction;

  HAL_RelayHandle m_forwardHandle = HAL_kInvalidHandle;
  HAL_RelayHandle m_reverseHandle = HAL_kInvalidHandle;

  std::unique_ptr<MotorSafetyHelper> m_safetyHelper;
};

}  // namespace frc
