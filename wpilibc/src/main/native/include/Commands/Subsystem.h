/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>
#include <string>

#include "ErrorBase.h"
#include "SmartDashboard/NamedSendable.h"

namespace frc {

class Command;

class Subsystem : public ErrorBase, public NamedSendable {
  friend class Scheduler;

 public:
  explicit Subsystem(const std::string& name);
  virtual ~Subsystem() = default;

  void SetDefaultCommand(Command* command);
  Command* GetDefaultCommand();
  void SetCurrentCommand(Command* command);
  Command* GetCurrentCommand() const;
  virtual void Periodic();
  virtual void InitDefaultCommand();

 private:
  void ConfirmCommand();

  Command* m_currentCommand = nullptr;
  bool m_currentCommandChanged = true;
  Command* m_defaultCommand = nullptr;
  std::string m_name;
  bool m_initializedDefaultCommand = false;

 public:
  std::string GetName() const override;
  void InitTable(std::shared_ptr<ITable> subtable) override;
  std::shared_ptr<ITable> GetTable() const override;
  std::string GetSmartDashboardType() const override;

 protected:
  std::shared_ptr<ITable> m_table;
};

}  // namespace frc
