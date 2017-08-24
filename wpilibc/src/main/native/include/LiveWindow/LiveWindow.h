/*----------------------------------------------------------------------------*/
/* Copyright (c) 2012-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "Commands/Scheduler.h"
#include "LiveWindow/LiveWindowSendable.h"
#include "tables/ITable.h"

namespace frc {

struct LiveWindowComponent {
  std::string subsystem;
  std::string name;
  bool isSensor = false;

  LiveWindowComponent() = default;
  LiveWindowComponent(std::string subsystem, std::string name, bool isSensor) {
    this->subsystem = subsystem;
    this->name = name;
    this->isSensor = isSensor;
  }
};

/**
 * The LiveWindow class is the public interface for putting sensors and
 * actuators
 * on the LiveWindow.
 */
class LiveWindow {
 public:
  static LiveWindow* GetInstance();
  void Run();
  void AddSensor(const std::string& subsystem, const std::string& name,
                 LiveWindowSendable* component);
  void AddSensor(const std::string& subsystem, const std::string& name,
                 LiveWindowSendable& component);
  void AddSensor(const std::string& subsystem, const std::string& name,
                 std::shared_ptr<LiveWindowSendable> component);
  void AddActuator(const std::string& subsystem, const std::string& name,
                   LiveWindowSendable* component);
  void AddActuator(const std::string& subsystem, const std::string& name,
                   LiveWindowSendable& component);
  void AddActuator(const std::string& subsystem, const std::string& name,
                   std::shared_ptr<LiveWindowSendable> component);

  void AddSensor(std::string type, int channel, LiveWindowSendable* component);
  void AddActuator(std::string type, int channel,
                   LiveWindowSendable* component);
  void AddActuator(std::string type, int module, int channel,
                   LiveWindowSendable* component);

  bool IsEnabled() const { return m_enabled; }
  void SetEnabled(bool enabled);

 protected:
  LiveWindow();
  virtual ~LiveWindow() = default;

 private:
  void UpdateValues();
  void Initialize();
  void InitializeLiveWindowComponents();

  std::vector<std::shared_ptr<LiveWindowSendable>> m_sensors;
  std::map<std::shared_ptr<LiveWindowSendable>, LiveWindowComponent>
      m_components;

  std::shared_ptr<ITable> m_liveWindowTable;
  std::shared_ptr<ITable> m_statusTable;

  Scheduler* m_scheduler;

  bool m_enabled = false;
  bool m_firstTime = true;
};

}  // namespace frc
