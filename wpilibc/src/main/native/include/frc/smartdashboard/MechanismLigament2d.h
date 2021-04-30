// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include <networktables/NetworkTableEntry.h>
#include <units/angle.h>

#include "frc/smartdashboard/MechanismObject2d.h"
#include "frc/util/Color8Bit.h"

namespace frc {

/**
 * Ligament node on a Mechanism2d.
 *
 * @see Mechanism2d
 */
class MechanismLigament2d : public MechanismObject2d {
 public:
  MechanismLigament2d(const wpi::Twine& name, double length,
                      units::degree_t angle, double lineWidth = 6,
                      const frc::Color8Bit& color = {235, 137, 52});

  /**
   * Set the ligament color.
   *
   * @param color the color of the line
   */
  void SetColor(const frc::Color8Bit& color);

  /**
   * Set the ligament's length.
   *
   * @param length the line length
   */
  void SetLength(double length);

  /**
   * Get the ligament length.
   *
   * @return the line length
   */
  double GetLength();

  /**
   * Set the ligament's angle relative to its parent.
   *
   * @param degrees the angle
   */
  void SetAngle(units::degree_t angle);

  /**
   * Get the ligament's angle relative to its parent.
   *
   * @return the angle
   */
  double GetAngle();

  /**
   * Set the line thickness.
   *
   * @param weight the line thickness
   */
  void SetLineWeight(double lineWidth);

 protected:
  void UpdateEntries(std::shared_ptr<NetworkTable> table) override;

 private:
  void Flush();
  double m_length;
  nt::NetworkTableEntry m_lengthEntry;
  double m_angle;
  nt::NetworkTableEntry m_angleEntry;
  double m_weight;
  nt::NetworkTableEntry m_weightEntry;
  char m_color[10];
  nt::NetworkTableEntry m_colorEntry;
};
}  // namespace frc
