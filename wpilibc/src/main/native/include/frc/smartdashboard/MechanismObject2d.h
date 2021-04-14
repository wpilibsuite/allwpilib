// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <map>
#include <memory>

#include <wpi/StringMap.h>

#include <networktables/NetworkTable.h>

namespace frc {

class MechanismObject2d {
  friend class MechanismRoot2d;
 protected:
  explicit MechanismObject2d(const wpi::Twine& name);
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
  const std::string& GetName() const;

  bool operator==(MechanismObject2d& rhs);

  /**
   * Append a Mechanism object that is based on this one.
   *
   * @param object the object to add.
   * @return the object given as a parameter, useful for variable assignments and call chaining.
   */
  template<typename T>
  T& Append(const T& object) {
      if (m_objects.count(object.GetName())) {
        //   throw
      }
      m_objects.try_emplace(object.GetName(), object);
  }

 private:
  std::string m_name;
  wpi::StringMap<MechanismObject2d> m_objects;
  std::shared_ptr<NetworkTable> m_table;
  mutable wpi::mutex m_mutex;
  void Update(std::shared_ptr<NetworkTable> table);

};
}