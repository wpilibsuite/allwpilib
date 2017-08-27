/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <list>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include <HAL/cpp/priority_mutex.h>

#include "Commands/Command.h"
#include "ErrorBase.h"
#include "SmartDashboard/NamedSendable.h"
#include "SmartDashboard/SmartDashboard.h"
#include "networktables/NetworkTable.h"

namespace frc {

class ButtonScheduler;
class Subsystem;

class Scheduler : public ErrorBase, public NamedSendable {
 public:
  static Scheduler* GetInstance();

  void AddCommand(Command* command);
  void AddButton(ButtonScheduler* button);
  void RegisterSubsystem(Subsystem* subsystem);
  void Run();
  void Remove(Command* command);
  void RemoveAll();
  void ResetAll();
  void SetEnabled(bool enabled);

  void UpdateTable();
  std::string GetSmartDashboardType() const;
  void InitTable(std::shared_ptr<ITable> subTable);
  std::shared_ptr<ITable> GetTable() const;
  std::string GetName() const;
  std::string GetType() const;

 private:
  Scheduler();
  virtual ~Scheduler() = default;

  void ProcessCommandAddition(Command* command);

  Command::SubsystemSet m_subsystems;
  hal::priority_mutex m_buttonsLock;
  typedef std::vector<ButtonScheduler*> ButtonVector;
  ButtonVector m_buttons;
  typedef std::vector<Command*> CommandVector;
  hal::priority_mutex m_additionsLock;
  CommandVector m_additions;
  typedef std::set<Command*> CommandSet;
  CommandSet m_commands;
  bool m_adding = false;
  bool m_enabled = true;
  std::vector<std::string> commands;
  std::vector<double> ids;
  std::vector<double> toCancel;
  std::shared_ptr<ITable> m_table;
  bool m_runningCommandsChanged = false;
};

}  // namespace frc
