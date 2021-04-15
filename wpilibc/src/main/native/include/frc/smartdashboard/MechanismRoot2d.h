// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <map>

#include <wpi/Twine.h>
#include "MechanismObject2d.h"
#include <networktables/NetworkTableEntry.h>

namespace frc {

class MechanismRoot2d : public MechanismObject2d {
  friend class Mechanism2d;
  struct private_init {};
  friend std::unique_ptr<frc::MechanismRoot2d,std::default_delete<frc::MechanismRoot2d>> std::make_unique<MechanismRoot2d, const wpi::Twine&, double&, double&>(const wpi::Twine &,double &,double &);
  static constexpr char kPosition[] = "pos";
 
 public:
  MechanismRoot2d(const wpi::Twine& name, double x, double y, const private_init& tag);
  MechanismRoot2d(const frc::MechanismRoot2d& rhs);

  /**
   * Set the root's position.
   *
   * @param x new x coordinate
   * @param y new y coordinate
   */
  void SetPosition(double x, double y);

  /**
   * Retrieve the object's name.
   * 
   * @return the object's name relative to its parent.
   */
  const std::string& GetName() const {
    return MechanismObject2d::GetName();
  }

  /**
   * Append a Mechanism object that is based on this root.
   *
   * @param name the name of the new object.
   * @param args constructor arguments of the object type.
   * @return the constructed and appended object, useful for variable assignments and call chaining.
   * @throw if an object with the given name already exists.
   */
  // template<typename T, typename... Args>
  // T* Append(wpi::StringRef name, Args&&... args) {
  //   return MechanismObject2d::Append<T>(name, std::forward<Args>(args)...);
  // }

  private:
  void UpdateEntries(std::shared_ptr<NetworkTable> table) override;
  inline void Flush();
  double m_x;
  double m_y;
  nt::NetworkTableEntry m_posEntry;
};
}