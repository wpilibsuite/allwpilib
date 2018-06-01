/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>
#include <string>

#include <wpi/deprecated.h>
#include <wpi/mutex.h>

#include "Base.h"
#include "Controller.h"
#include "Filters/LinearDigitalFilter.h"
#include "Notifier.h"
#include "PIDBase.h"
#include "PIDSource.h"
#include "Timer.h"

namespace frc {

class PIDOutput;

/**
 * Class implements a PID Control Loop.
 *
 * Creates a separate thread which reads the given PIDSource and takes care of
 * the integral calculations, as well as writing the given PIDOutput.
 *
 * This feedback controller runs in discrete time, so time deltas are not used
 * in the integral and derivative calculations. Therefore, the sample rate
 * affects the controller's behavior for a given set of PID constants.
 */
class PIDController : public PIDBase, public Controller {
 public:
  PIDController(double Kp, double Ki, double Kd, PIDSource* source,
                PIDOutput* output, double period = 0.05);
  PIDController(double Kp, double Ki, double Kd, double Kv, PIDSource* source,
                PIDOutput* output, double period = 0.05);
  PIDController(double Kp, double Ki, double Kd, double Kv, double Ka,
                PIDSource* source, PIDOutput* output, double period = 0.05);
  PIDController(double Kp, double Ki, double Kd, PIDSource& source,
                PIDOutput& output, double period = 0.05);
  PIDController(double Kp, double Ki, double Kd, double Kv, PIDSource& source,
                PIDOutput& output, double period = 0.05);
  PIDController(double Kp, double Ki, double Kd, double Kv, double Ka,
                PIDSource& source, PIDOutput& output, double period = 0.05);
  ~PIDController() override;

  PIDController(const PIDController&) = delete;
  PIDController& operator=(const PIDController) = delete;

  void Enable() override;
  void Disable() override;
  void SetEnabled(bool enable);
  bool IsEnabled() const;

  double CalculateFeedForward() override;

  void Reset() override;

  void InitSendable(SendableBuilder& builder) override;

 private:
  std::unique_ptr<Notifier> m_controlLoop;
  double m_period;
};

}  // namespace frc
