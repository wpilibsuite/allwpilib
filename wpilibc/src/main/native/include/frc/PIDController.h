/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>
#include <string>

#include <wpi/deprecated.h>
#include <wpi/mutex.h>

#include "frc/Base.h"
#include "frc/Controller.h"
#include "frc/Notifier.h"
#include "frc/PIDBase.h"
#include "frc/PIDSource.h"
#include "frc/Timer.h"

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
  /**
   * Allocate a PID object with the given constants for P, I, D.
   *
   * @param Kp     the proportional coefficient
   * @param Ki     the integral coefficient
   * @param Kd     the derivative coefficient
   * @param source The PIDSource object that is used to get values
   * @param output The PIDOutput object that is set to the output value
   * @param period the loop time for doing calculations in seconds. This
   *               particularly affects calculations of the integral and
   *               differental terms. The default is 0.05 (50ms).
   */
  WPI_DEPRECATED("Use frc2::PIDController class instead.")
  PIDController(double p, double i, double d, PIDSource* source,
                PIDOutput* output, double period = 0.05);

  /**
   * Allocate a PID object with the given constants for P, I, D.
   *
   * @param Kp     the proportional coefficient
   * @param Ki     the integral coefficient
   * @param Kd     the derivative coefficient
   * @param source The PIDSource object that is used to get values
   * @param output The PIDOutput object that is set to the output value
   * @param period the loop time for doing calculations in seconds. This
   *               particularly affects calculations of the integral and
   *               differental terms. The default is 0.05 (50ms).
   */
  WPI_DEPRECATED("Use frc2::PIDController class instead.")
  PIDController(double p, double i, double d, double f, PIDSource* source,
                PIDOutput* output, double period = 0.05);

  /**
   * Allocate a PID object with the given constants for P, I, D.
   *
   * @param Kp     the proportional coefficient
   * @param Ki     the integral coefficient
   * @param Kd     the derivative coefficient
   * @param source The PIDSource object that is used to get values
   * @param output The PIDOutput object that is set to the output value
   * @param period the loop time for doing calculations in seconds. This
   *               particularly affects calculations of the integral and
   *               differental terms. The default is 0.05 (50ms).
   */
  WPI_DEPRECATED("Use frc2::PIDController class instead.")
  PIDController(double p, double i, double d, PIDSource& source,
                PIDOutput& output, double period = 0.05);

  /**
   * Allocate a PID object with the given constants for P, I, D.
   *
   * @param Kp     the proportional coefficient
   * @param Ki     the integral coefficient
   * @param Kd     the derivative coefficient
   * @param source The PIDSource object that is used to get values
   * @param output The PIDOutput object that is set to the output value
   * @param period the loop time for doing calculations in seconds. This
   *               particularly affects calculations of the integral and
   *               differental terms. The default is 0.05 (50ms).
   */
  WPI_DEPRECATED("Use frc2::PIDController class instead.")
  PIDController(double p, double i, double d, double f, PIDSource& source,
                PIDOutput& output, double period = 0.05);

  ~PIDController() override;

  /**
   * Begin running the PIDController.
   */
  void Enable() override;

  /**
   * Stop running the PIDController, this sets the output to zero before
   * stopping.
   */
  void Disable() override;

  /**
   * Set the enabled state of the PIDController.
   */
  void SetEnabled(bool enable);

  /**
   * Return true if PIDController is enabled.
   */
  bool IsEnabled() const;

  /**
   * Reset the previous error, the integral term, and disable the controller.
   */
  void Reset() override;

  void InitSendable(SendableBuilder& builder) override;

 private:
  std::unique_ptr<Notifier> m_controlLoop;
};

}  // namespace frc
