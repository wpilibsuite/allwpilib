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
  friend class Mechanism2d;
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
  template<typename T, typename... Args>
  T* Append(wpi::StringRef name, Args&&... args) {
    if (m_objects.count(name)) {
      // throw or return?
    }
    std::unique_ptr<T> ptr = std::make_unique<T>(name, std::forward<Args>(args)...);
    m_objects.try_emplace<std::unique_ptr<MechanismObject2d>>(name, std::move(ptr));
    ptr->Update(m_table);
    return ptr.get();
  }

 private:
  std::string m_name;
  wpi::StringMap<std::unique_ptr<MechanismObject2d>> m_objects;
  std::shared_ptr<NetworkTable> m_table;
  mutable wpi::mutex m_mutex;
  void Update(std::shared_ptr<NetworkTable> table);

};
}