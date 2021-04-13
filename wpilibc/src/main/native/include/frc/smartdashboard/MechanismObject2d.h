// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <map>
#include <memory>

#include <wpi/Twine.h>

#include <networktables/NetworkTable.h>

namespace frc {

class MechanismObject2d {
  friend class MechanismRoot2d;
 protected:
  explicit MechanismObject2d(wpi::Twine& name) : m_name{name} {}
  /**
   * Update all entries with new ones from a new table.
   *
   * @param table the new table.
   */
  virtual void UpdateEntries(std::shared_ptr<NetworkTable> table) = 0;
 public:
  /**
   * Retrieve the object's name.
   * 
   * @return the object's name relative to its parent.
   */
  wpi::Twine& GetName() const;

  bool operator==(MechanismObject2d& rhs);

  /**
   * Append a Mechanism object that is based on this one.
   *
   * @param object the object to add.
   * @return the object given as a parameter, useful for variable assignments and call chaining.
   */
  template<typename T>
  T Append(const T& object) const {
      if (m_objects.containsKey(object.getName())) {
        //   throw
      }
      m_objects[object.GetName()] = object;
  }

 private:
  wpi::Twine& m_name;
  std::map<wpi::Twine&, MechanismObject2d> m_objects;
  std::shared_ptr<NetworkTable> m_table;
  mutable wpi::mutex m_mutex;
  void Update(std::shared_ptr<NetworkTable> table);

};
}