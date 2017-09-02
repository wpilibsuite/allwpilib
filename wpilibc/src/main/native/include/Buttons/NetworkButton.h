/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>

#include <llvm/StringRef.h>

#include "Buttons/Button.h"
#include "networktables/NetworkTable.h"
#include "networktables/NetworkTableEntry.h"

namespace frc {

class NetworkButton : public Button {
 public:
  NetworkButton(llvm::StringRef tableName, llvm::StringRef field);
  NetworkButton(std::shared_ptr<nt::NetworkTable> table, llvm::StringRef field);
  virtual ~NetworkButton() = default;

  virtual bool Get();

 private:
  nt::NetworkTableEntry m_entry;
};

}  // namespace frc
