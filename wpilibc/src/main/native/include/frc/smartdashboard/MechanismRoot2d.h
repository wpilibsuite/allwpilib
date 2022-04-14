// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string_view>

#include <networktables/NetworkTableEntry.h>

#include "MechanismObject2d.h"

namespace frc {

/**
 * Root Mechanism2d node.
 *
 * A root is the anchor point of other nodes (such as ligaments).
 *
 * Do not create objects of this class directly! Obtain pointers from the
 * Mechanism2d.GetRoot() factory method.
 *
 * <p>Append other nodes by using Append().
 */
class MechanismRoot2d : private MechanismObject2d {
  friend class Mechanism2d;
  struct private_init {};

 public:
  MechanismRoot2d(std::string_view name, double x, double y,
                  const private_init&);

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
  void UpdateEntries(std::shared_ptr<nt::NetworkTable> table) override;
  inline void Flush();
  double m_x;
  double m_y;
  nt::NetworkTableEntry m_xEntry;
  nt::NetworkTableEntry m_yEntry;
};
}  // namespace frc
