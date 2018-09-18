/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>

#include <networktables/NetworkTable.h>
#include <networktables/NetworkTableEntry.h>
#include <wpi/Twine.h>

#include "frc/buttons/Button.h"

namespace frc {

class NetworkButton : public Button {
 public:
  NetworkButton(const wpi::Twine& tableName, const wpi::Twine& field);
  NetworkButton(std::shared_ptr<nt::NetworkTable> table,
                const wpi::Twine& field);
  virtual ~NetworkButton() = default;

  NetworkButton(NetworkButton&&) = default;
  NetworkButton& operator=(NetworkButton&&) = default;

  virtual bool Get();

 private:
  nt::NetworkTableEntry m_entry;
};

}  // namespace frc
