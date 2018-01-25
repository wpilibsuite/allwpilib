/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>
#include <string>

#include <support/mutex.h>

#include "Sendable.h"

namespace frc {

class SendableBase : public Sendable {
 public:
  explicit SendableBase(bool addLiveWindow = true);
  ~SendableBase() override;

  using Sendable::SetName;

  std::string GetName() const final;
  void SetName(const llvm::Twine& name) final;
  std::string GetSubsystem() const final;
  void SetSubsystem(const llvm::Twine& subsystem) final;

 protected:
  void AddChild(std::shared_ptr<Sendable> child);
  void AddChild(void* child);

  void SetName(const llvm::Twine& moduleType, int channel);
  void SetName(const llvm::Twine& moduleType, int moduleNumber, int channel);

 private:
  mutable wpi::mutex m_mutex;
  std::string m_name;
  std::string m_subsystem = "Ungrouped";
};

}  // namespace frc
