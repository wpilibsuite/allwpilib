// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <map>

#include <wpi/Twine.h>
#include "MechanismObject2d.h"

namespace frc {

class MechanismRoot2d {
  friend class Mechanism2d;
  static constexpr char* kPosition {"pos"};
 
 public:
  wpi::Twine& GetName();

  /**
   * Set the root's position.
   *
   * @param x new x coordinate
   * @param y new y coordinate
   */
  void SetPosition(double x, double y);

  /**
   * Append a Mechanism object that is based on this root.
   *
   * @param object the object to add.
   * @return the object given as a parameter, useful for variable assignments and call chaining.
   */
  template<typename T>
  T Append(const std::shared_ptr<T> object) const {
      if (m_objects.count(object.getName())) {
        //   throw
      }
      m_objects[object.GetName()] = object;
  }

  private:
  MechanismRoot2d(wpi::Twine& name, double x, double y);
  void Update(std::shared_ptr<NetworkTable> table);
  inline void Flush() const;
  std::string m_name;
  std::shared_ptr<NetworkTable> m_table;
  wpi::StringMap<MechanismRoot2d> m_objects;
  mutable wpi::mutex m_mutex;
  double m_pos[2];

};
}