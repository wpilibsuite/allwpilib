/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <vector>

#include <Eigen/Core>

#include "frc/controller/PeriodVariantPlant.h"
#include "frc/controller/StateSpaceControllerCoeffs.h"

namespace frc {

/**
 * Contains the controller coefficients and logic for a state-space controller.
 *
 * State-space controllers generally use the control law u = -Kx. The
 * feedforward used is u_ff = K_ff * (r_k+1 - A * r_k).
 *
 * For more on the underlying math, read
 * https://file.tavsys.net/control/state-space-guide.pdf.
 */
template <int States, int Inputs, int Outputs>
class PeriodVariantController {
 public:
  /**
   * Constructs a controller with the given coefficients and plant.
   *
   * @param controllerCoeffs Controller coefficients.
   * @param plant            The plant used for the feedforward calculation.
   */
  PeriodVariantController(const StateSpaceControllerCoeffs<
                              States, Inputs, Outputs>& controllerCoeffs,
                          PeriodVariantPlant<States, Inputs, Outputs>& plant);

  PeriodVariantController(PeriodVariantController&&) = default;
  PeriodVariantController& operator=(PeriodVariantController&&) = default;

  /**
   * Enables controller.
   */
  void Enable();

  /**
   * Disables controller, zeroing controller output U.
   */
  void Disable();

  /**
   * Returns the controller matrix K.
   */
  const Eigen::Matrix<double, Inputs, States>& K() const;

  /**
   * Returns an element of the controller matrix K.
   *
   * @param i Row of K.
   * @param j Column of K.
   */
  double K(int i, int j) const;

  /**
   * Returns the feedforward controller matrix Kff.
   */
  const Eigen::Matrix<double, Inputs, States>& Kff() const;

  /**
   * Returns an element of the feedforward controller matrix Kff.
   *
   * @param i Row of Kff.
   * @param j Column of Kff.
   */
  double Kff(int i, int j) const;

  /**
   * Returns the reference vector r.
   */
  const Eigen::Matrix<double, States, 1>& R() const;

  /**
   * Returns an element of the reference vector r.
   *
   * @param i Row of r.
   */
  double R(int i) const;

  /**
   * Returns the control input vector u.
   */
  const Eigen::Matrix<double, Inputs, 1>& U() const;

  /**
   * Returns an element of the control input vector u.
   *
   * @param i Row of u.
   */
  double U(int i) const;

  /**
   * Resets the controller.
   */
  void Reset();

  /**
   * Update controller without setting a new reference.
   *
   * @param x The current state x.
   */
  void Update(const Eigen::Matrix<double, States, 1>& x);

  /**
   * Set a new reference and update the controller.
   *
   * @param x     The current state x.
   * @param nextR The next reference vector r.
   */
  void Update(const Eigen::Matrix<double, States, 1>& x,
              const Eigen::Matrix<double, States, 1>& nextR);

  /**
   * Adds the given coefficients to the controller for gain scheduling.
   *
   * @param coefficients Controller coefficients.
   */
  void AddCoefficients(
      const StateSpaceControllerCoeffs<States, Inputs, Outputs>& coefficients);

  /**
   * Returns the controller coefficients with the given index.
   *
   * @param index Index of coefficients.
   */
  const StateSpaceControllerCoeffs<States, Inputs, Outputs>& GetCoefficients(
      int index) const;

  /**
   * Returns the current controller coefficients.
   */
  const StateSpaceControllerCoeffs<States, Inputs, Outputs>& GetCoefficients()
      const;

  /**
   * Sets the current controller to be "index", clamped to be within range. This
   * can be used for gain scheduling.
   *
   * @param index The controller index.
   */
  void SetIndex(int index);

  /**
   * Returns the current controller index.
   */
  int GetIndex() const;

 private:
  PeriodVariantPlant<States, Inputs, Outputs>* m_plant;

  bool m_enabled = false;

  /**
   * Current reference.
   */
  Eigen::Matrix<double, States, 1> m_r;

  /**
   * Computed controller output after being capped.
   */
  Eigen::Matrix<double, Inputs, 1> m_u;

  std::vector<StateSpaceControllerCoeffs<States, Inputs, Outputs>>
      m_coefficients;
  int m_index = 0;

  void CapU();
};

}  // namespace frc

#include "frc/controller/PeriodVariantController.inc"
