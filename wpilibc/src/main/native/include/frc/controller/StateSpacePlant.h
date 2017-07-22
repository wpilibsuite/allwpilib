/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <vector>

#include <Eigen/Core>

#include "frc/controller/StateSpacePlantCoeffs.h"

namespace frc {

/**
 * A plant defined using state-space notation.
 *
 * A plant is a mathematical model of a system's dynamics.
 *
 * For more on the underlying math, read
 * https://file.tavsys.net/control/state-space-guide.pdf.
 */
template <int States, int Inputs, int Outputs>
class StateSpacePlant {
 public:
  /**
   * Constructs a plant with the given coefficients.
   *
   * @param plantCoeffs Plant coefficients.
   */
  explicit StateSpacePlant(
      const StateSpacePlantCoeffs<States, Inputs, Outputs>& plantCoeffs);

  virtual ~StateSpacePlant() = default;

  StateSpacePlant(StateSpacePlant&&) = default;
  StateSpacePlant& operator=(StateSpacePlant&&) = default;

  /**
   * Returns the system matrix A.
   */
  const Eigen::Matrix<double, States, States>& A() const;

  /**
   * Returns an element of the system matrix A.
   *
   * @param i Row of A.
   * @param j Column of A.
   */
  double A(int i, int j) const;

  /**
   * Returns the input matrix B.
   */
  const Eigen::Matrix<double, States, Inputs>& B() const;

  /**
   * Returns an element of the input matrix B.
   *
   * @param i Row of B.
   * @param j Column of B.
   */
  double B(int i, int j) const;

  /**
   * Returns the output matrix C.
   */
  const Eigen::Matrix<double, Outputs, States>& C() const;

  /**
   * Returns an element of the output matrix C.
   *
   * @param i Row of C.
   * @param j Column of C.
   */
  double C(int i, int j) const;

  /**
   * Returns the feedthrough matrix D.
   */
  const Eigen::Matrix<double, Outputs, Inputs>& D() const;

  /**
   * Returns an element of the feedthrough matrix D.
   *
   * @param i Row of D.
   * @param j Column of D.
   */
  double D(int i, int j) const;

  /**
   * Returns the current state X.
   */
  const Eigen::Matrix<double, States, 1>& X() const;

  /**
   * Returns an element of the current state x.
   *
   * @param i Row of x.
   */
  double X(int i) const;

  /**
   * Returns the current measurement vector y.
   */
  const Eigen::Matrix<double, Outputs, 1>& Y() const;

  /**
   * Returns an element of the current measurement vector y.
   *
   * @param i Row of y.
   */
  double Y(int i) const;

  /**
   * Set the initial state x.
   *
   * @param x The initial state.
   */
  void SetX(const Eigen::Matrix<double, States, 1>& x);

  /**
   * Set an element of the initial state x.
   *
   * @param i     Row of x.
   * @param value Value of element of x.
   */
  void SetX(int i, double value);

  /**
   * Set the current measurement y.
   *
   * @param y The current measurement.
   */
  void SetY(const Eigen::Matrix<double, Outputs, 1>& y);

  /**
   * Set an element of the current measurement y.
   *
   * @param i     Row of y.
   * @param value Value of element of y.
   */
  void SetY(int i, double value);

  /**
   * Adds the given coefficients to the plant for gain scheduling.
   *
   * @param coefficients Plant coefficients.
   */
  void AddCoefficients(
      const StateSpacePlantCoeffs<States, Inputs, Outputs>& coefficients);

  /**
   * Returns the plant coefficients with the given index.
   *
   * @param index Index of coefficients.
   */
  const StateSpacePlantCoeffs<States, Inputs, Outputs>& GetCoefficients(
      int index) const;

  /**
   * Returns the current plant coefficients.
   */
  const StateSpacePlantCoeffs<States, Inputs, Outputs>& GetCoefficients() const;

  /**
   * Sets the current plant to be "index", clamped to be within range. This can
   * be used for gain scheduling to make the current model match changed plant
   * behavior.
   *
   * @param index The plant index.
   */
  void SetIndex(int index);

  /**
   * Returns the current plant index.
   */
  int GetIndex() const;

  /**
   * Resets the plant.
   */
  void Reset();

  /**
   * Computes the new x and y given the control input.
   *
   * @param x The current state.
   * @param u The control input.
   */
  void Update(const Eigen::Matrix<double, States, 1>& x,
              const Eigen::Matrix<double, Inputs, 1>& u);

  /**
   * Computes the new x given the old x and the control input.
   *
   * This is used by state observers directly to run updates based on state
   * estimate.
   *
   * @param x The current state.
   * @param u The control input.
   */
  Eigen::Matrix<double, States, 1> CalculateX(
      const Eigen::Matrix<double, States, 1>& x,
      const Eigen::Matrix<double, Inputs, 1>& u) const;

  /**
   * Computes the new y given the control input.
   *
   * This is used by state observers directly to run updates based on state
   * estimate.
   *
   * @param x The current state.
   * @param u The control input.
   */
  Eigen::Matrix<double, Outputs, 1> CalculateY(
      const Eigen::Matrix<double, States, 1>& x,
      const Eigen::Matrix<double, Inputs, 1>& u) const;

 protected:
  // These are accessible from non-templated subclasses.
  static constexpr int kStates = States;
  static constexpr int kInputs = Inputs;
  static constexpr int kOutputs = Outputs;

 private:
  Eigen::Matrix<double, States, 1> m_X;
  Eigen::Matrix<double, Outputs, 1> m_Y;

  std::vector<StateSpacePlantCoeffs<States, Inputs, Outputs>> m_coefficients;
  int m_index = 0;
};

}  // namespace frc

#include "frc/controller/StateSpacePlant.inc"
