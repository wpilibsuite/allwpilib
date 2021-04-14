// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <map>

#include "networktables/NetworkTableEntry.h"
#include "frc/smartdashboard/Sendable.h"
#include "frc/smartdashboard/SendableHelper.h"
#include "frc/util/Color8Bit.h"

#include "MechanismRoot2d.h"

namespace frc {

class Mechanism2d : public Sendable, public SendableHelper<Mechanism2d> {
  static constexpr char* kBackgroundColor{"backgroundColor"};
 public:
  /**
   * Create a new Mechanism2d window with the given dimensions.
   *
   * @param width the window width
   * @param height the window height
   */
  Mechanism2d(double width, double height);

  /**
   * Get or create a root in this Mechanism2d window with the given name and position.
   *
   * <p>If a root with the given name already exists, the given x and y coordinates are not used.
   *
   * @param name the root name
   * @param x the root x coordinate
   * @param y the root y coordinate
   * @return a new root object, or the existing one with the given name.
   */
  MechanismRoot2d& GetRoot(const wpi::StringRef name, double x, double y);

  /**
   * Set the Mechanism2d window background color.
   *
   * @param color the new background color
   */
  void SetBackgroundColor(const Color8Bit& color);

  void InitSendable(SendableBuilder& builder) override;

  private:
  double m_dims[2];
  std::string m_color;
  mutable wpi::mutex m_mutex;
  std::shared_ptr<nt::NetworkTable> m_table;
  wpi::StringMap<MechanismRoot2d> m_roots;
};
}