// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include <networktables/NetworkTableEntry.h>
#include <wpi/Twine.h>

#include "MechanismObject2d.h"

namespace frc {

/**
 * Root Mechanism2d node.
 *
 * Do not create objects of this class directly! Obtain pointers from the
 * Mechanism2d.GetRoot() factory method.
 */
class MechanismRoot2d : private MechanismObject2d {
  friend class Mechanism2d;
  struct private_init {};

 public:
  MechanismRoot2d(const wpi::Twine& name, double x, double y,
                  const private_init& tag);
  MechanismRoot2d(const MechanismRoot2d& rhs);
  MechanismRoot2d& operator=(MechanismRoot2d&& rhs);

  /**
   * Set the root's position.
   *
   * @param x new x coordinate
   * @param y new y coordinate
   */
  void SetPosition(double x, double y);

  using MechanismObject2d::GetName;

  using MechanismObject2d::Append;

 private:
  void UpdateEntries(std::shared_ptr<NetworkTable> table) override;
  inline void Flush();
  double m_x;
  double m_y;
  nt::NetworkTableEntry m_posEntry;
};
}  // namespace frc
