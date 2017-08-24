/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "ErrorBase.h"
#include "GamepadBase.h"

namespace frc {

class DriverStation;

/**
 * Handle input from Xbox 360 or Xbox One controllers connected to the Driver
 * Station.
 *
 * This class handles Xbox input that comes from the Driver Station. Each time a
 * value is requested the most recent value is returend. There is a single class
 * instance for each controller and the mapping of ports to hardware buttons
 * depends on the code in the Driver Station.
 */
class XboxController : public GamepadBase, public ErrorBase {
 public:
  explicit XboxController(int port);
  virtual ~XboxController() = default;

  XboxController(const XboxController&) = delete;
  XboxController& operator=(const XboxController&) = delete;

  double GetX(JoystickHand hand) const override;
  double GetY(JoystickHand hand) const override;

  bool GetBumper(JoystickHand hand) const override;
  bool GetStickButton(JoystickHand hand) const override;

  virtual double GetTriggerAxis(JoystickHand hand) const;

  bool GetAButton() const;
  bool GetBButton() const;
  bool GetXButton() const;
  bool GetYButton() const;
  bool GetBackButton() const;
  bool GetStartButton() const;

 private:
  DriverStation& m_ds;
};

}  // namespace frc
