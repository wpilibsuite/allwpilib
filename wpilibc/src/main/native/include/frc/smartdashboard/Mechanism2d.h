// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string>

#include <networktables/DoubleArrayTopic.h>
#include <networktables/NTSendable.h>
#include <networktables/NetworkTable.h>
#include <networktables/StringTopic.h>
#include <wpi/StringMap.h>
#include <wpi/mutex.h>
#include <wpi/sendable/SendableHelper.h>

#include "frc/smartdashboard/MechanismRoot2d.h"
#include "frc/util/Color8Bit.h"

namespace frc {

/**
 * Visual 2D representation of arms, elevators, and general mechanisms through
 * a node-based API.
 *
 * A Mechanism2d object is published and contains at least one root node. A root
 * is the anchor point of other nodes (such as ligaments). Other nodes are
 * recursively appended based on other nodes.
 *
 * Except for the Mechanism2d container object, none of the objects should be
 * passed or interacted with by value! Obtain pointers from factory methods such
 * as Mechanism2d.GetRoot() and MechanismObject2d.Append<>(). The Mechanism2d
 * container object owns the root nodes, and each node internally owns the nodes
 * based on it. Beware not to let the Mechanism2d object out of scope - all
 * nodes will be recursively destructed!
 *
 * @see MechanismObject2d
 * @see MechanismLigament2d
 * @see MechanismRoot2d
 */
class Mechanism2d : public nt::NTSendable,
                    public wpi::SendableHelper<Mechanism2d> {
 public:
  /**
   * Create a new Mechanism2d with the given dimensions and background color.
   *
   * The dimensions represent the canvas that all the nodes are drawn on. The
   * default color is dark blue.
   *
   * @param width the width
   * @param height the height
   * @param backgroundColor the background color
   */
  Mechanism2d(double width, double height,
              const Color8Bit& backgroundColor = {0, 0, 32});

  /**
   * Get or create a root in this Mechanism2d with the given name and
   * position.
   *
   * <p>If a root with the given name already exists, the given x and y
   * coordinates are not used.
   *
   * @param name the root name
   * @param x the root x coordinate
   * @param y the root y coordinate
   * @return a new root object, or the existing one with the given name.
   */
  MechanismRoot2d* GetRoot(std::string_view name, double x, double y);

  /**
   * Set the Mechanism2d background color.
   *
   * @param color the new background color
   */
  void SetBackgroundColor(const Color8Bit& color);

  void InitSendable(nt::NTSendableBuilder& builder) override;

 private:
  double m_width;
  double m_height;
  std::string m_color;
  mutable wpi::mutex m_mutex;
  std::shared_ptr<nt::NetworkTable> m_table;
  wpi::StringMap<std::unique_ptr<MechanismRoot2d>> m_roots;
  nt::DoubleArrayPublisher m_dimsPub;
  nt::StringPublisher m_colorPub;
};
}  // namespace frc
