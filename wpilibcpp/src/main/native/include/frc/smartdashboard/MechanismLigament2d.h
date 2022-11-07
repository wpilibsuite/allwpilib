// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string_view>

#include <networktables/DoubleTopic.h>
#include <networktables/StringTopic.h>
#include <units/angle.h>

#include "frc/smartdashboard/MechanismObject2d.h"
#include "frc/util/Color8Bit.h"

namespace frc {

/**
 * Ligament node on a Mechanism2d.
 *
 * A ligament can have its length changed (like an elevator) or angle changed,
 * like an arm.
 *
 * @see Mechanism2d
 */
class MechanismLigament2d : public MechanismObject2d {
 public:
  MechanismLigament2d(std::string_view name, double length,
                      units::degree_t angle, double lineWidth = 6,
                      const frc::Color8Bit& color = {235, 137, 52});

  /**
   * Set the ligament color.
   *
   * @param color the color of the line
   */
  void SetColor(const Color8Bit& color);

  /**
   * Get the ligament color.
   *
   * @return the color of the line
   */
  Color8Bit GetColor();

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
   * @param angle the angle
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
   * @param lineWidth the line thickness
   */
  void SetLineWeight(double lineWidth);

  /**
   * Get the line thickness.
   *
   * @return the line thickness
   */
  double GetLineWeight();

 protected:
  void UpdateEntries(std::shared_ptr<nt::NetworkTable> table) override;

 private:
  nt::StringPublisher m_typePub;
  double m_length;
  nt::DoubleEntry m_lengthEntry;
  double m_angle;
  nt::DoubleEntry m_angleEntry;
  double m_weight;
  nt::DoubleEntry m_weightEntry;
  char m_color[10];
  nt::StringEntry m_colorEntry;
};
}  // namespace frc
