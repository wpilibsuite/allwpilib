/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>
#include <set>
#include <string>
#include <vector>

#include <networktables/NetworkTableEntry.h>
#include <support/mutex.h>

#include "Commands/Command.h"
#include "ErrorBase.h"
#include "SmartDashboard/SendableBase.h"

namespace frc {

class ButtonScheduler;
class Subsystem;

class Scheduler : public ErrorBase, public SendableBase {
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

  void InitSendable(SendableBuilder& builder) override;

 private:
  Scheduler();
  ~Scheduler() override = default;

  void ProcessCommandAddition(Command* command);

  Command::SubsystemSet m_subsystems;
  wpi::mutex m_buttonsMutex;
  typedef std::vector<ButtonScheduler*> ButtonVector;
  ButtonVector m_buttons;
  typedef std::vector<Command*> CommandVector;
  wpi::mutex m_additionsMutex;
  CommandVector m_additions;
  typedef std::set<Command*> CommandSet;
  CommandSet m_commands;
  bool m_adding = false;
  bool m_enabled = true;
  std::vector<std::string> commands;
  std::vector<double> ids;
  std::vector<double> toCancel;
  nt::NetworkTableEntry m_namesEntry;
  nt::NetworkTableEntry m_idsEntry;
  nt::NetworkTableEntry m_cancelEntry;
  bool m_runningCommandsChanged = false;
};

}  // namespace frc
