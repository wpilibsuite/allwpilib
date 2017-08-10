/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2012-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "LiveWindow/LiveWindow.h"

#include <algorithm>

#include "llvm/SmallString.h"
#include "llvm/raw_ostream.h"
#include "networktables/NetworkTable.h"

using namespace frc;

/**
 * Get an instance of the LiveWindow main class.
 *
 * This is a singleton to guarantee that there is only a single instance
 * regardless of how many times GetInstance is called.
 */
LiveWindow* LiveWindow::GetInstance() {
  static LiveWindow instance;
  return &instance;
}

/**
 * LiveWindow constructor.
 *
 * Allocate the necessary tables.
 */
LiveWindow::LiveWindow() : m_scheduler(Scheduler::GetInstance()) {
  m_liveWindowTable = NetworkTable::GetTable("LiveWindow");
  m_statusTable = m_liveWindowTable->GetSubTable("~STATUS~");
}

/**
 * Change the enabled status of LiveWindow.
 *
 * If it changes to enabled, start livewindow running otherwise stop it
 */
void LiveWindow::SetEnabled(bool enabled) {
  if (m_enabled == enabled) return;
  if (enabled) {
    if (m_firstTime) {
      InitializeLiveWindowComponents();
      m_firstTime = false;
    }
    m_scheduler->SetEnabled(false);
    m_scheduler->RemoveAll();
    for (auto& elem : m_components) {
      elem.first->StartLiveWindowMode();
    }
  } else {
    for (auto& elem : m_components) {
      elem.first->StopLiveWindowMode();
    }
    m_scheduler->SetEnabled(true);
  }
  m_enabled = enabled;
  m_statusTable->PutBoolean("LW Enabled", m_enabled);
}

/**
 * @name AddSensor(subsystem, name, component)
 *
 * Add a Sensor associated with the subsystem and call it by the given name.
 *
 * @param subsystem The subsystem this component is part of.
 * @param name      The name of this component.
 * @param component A LiveWindowSendable component that represents a sensor.
 */
//@{
/**
 * @brief Use a STL smart pointer to share ownership of component.
 */
void LiveWindow::AddSensor(const std::string& subsystem,
                           const std::string& name,
                           std::shared_ptr<LiveWindowSendable> component) {
  m_components[component].subsystem = subsystem;
  m_components[component].name = name;
  m_components[component].isSensor = true;
}

/**
 * @brief Pass a reference to LiveWindow and retain ownership of the component.
 */
void LiveWindow::AddSensor(const std::string& subsystem,
                           const std::string& name,
                           LiveWindowSendable& component) {
  AddSensor(subsystem, name, std::shared_ptr<LiveWindowSendable>(
                                 &component, [](LiveWindowSendable*) {}));
}

/**
 * @brief Use a raw pointer to the LiveWindow.
 * @deprecated Prefer smart pointers or references.
 */
void LiveWindow::AddSensor(const std::string& subsystem,
                           const std::string& name,
                           LiveWindowSendable* component) {
  AddSensor(subsystem, name, std::shared_ptr<LiveWindowSendable>(
                                 component, NullDeleter<LiveWindowSendable>()));
}
//@}

/**
 * @name AddActuator(subsystem, name, component)
 *
 * Add an Actuator associated with the subsystem and call it by the given name.
 *
 * @param subsystem The subsystem this component is part of.
 * @param name      The name of this component.
 * @param component A LiveWindowSendable component that represents a actuator.
 */
//@{
/**
 * @brief Use a STL smart pointer to share ownership of component.
 */
void LiveWindow::AddActuator(const std::string& subsystem,
                             const std::string& name,
                             std::shared_ptr<LiveWindowSendable> component) {
  m_components[component].subsystem = subsystem;
  m_components[component].name = name;
  m_components[component].isSensor = false;
}

/**
 * @brief Pass a reference to LiveWindow and retain ownership of the component.
 */
void LiveWindow::AddActuator(const std::string& subsystem,
                             const std::string& name,
                             LiveWindowSendable& component) {
  AddActuator(subsystem, name, std::shared_ptr<LiveWindowSendable>(
                                   &component, [](LiveWindowSendable*) {}));
}

/**
 * @brief Use a raw pointer to the LiveWindow.
 * @deprecated Prefer smart pointers or references.
 */
void LiveWindow::AddActuator(const std::string& subsystem,
                             const std::string& name,
                             LiveWindowSendable* component) {
  AddActuator(subsystem, name,
              std::shared_ptr<LiveWindowSendable>(
                  component, NullDeleter<LiveWindowSendable>()));
}
//@}

/**
 * Meant for internal use in other WPILib classes.
 */
void LiveWindow::AddSensor(std::string type, int channel,
                           LiveWindowSendable* component) {
  llvm::SmallString<128> buf;
  llvm::raw_svector_ostream oss(buf);
  oss << type << "[" << channel << "]";
  AddSensor("Ungrouped", oss.str(), component);
  std::shared_ptr<LiveWindowSendable> component_stl(
      component, NullDeleter<LiveWindowSendable>());
  if (std::find(m_sensors.begin(), m_sensors.end(), component_stl) ==
      m_sensors.end())
    m_sensors.push_back(component_stl);
}

/**
 * Meant for internal use in other WPILib classes.
 */
void LiveWindow::AddActuator(std::string type, int channel,
                             LiveWindowSendable* component) {
  llvm::SmallString<128> buf;
  llvm::raw_svector_ostream oss(buf);
  oss << type << "[" << channel << "]";
  AddActuator("Ungrouped", oss.str(),
              std::shared_ptr<LiveWindowSendable>(component,
                                                  [](LiveWindowSendable*) {}));
}

/**
 * Meant for internal use in other WPILib classes.
 */
void LiveWindow::AddActuator(std::string type, int module, int channel,
                             LiveWindowSendable* component) {
  llvm::SmallString<128> buf;
  llvm::raw_svector_ostream oss(buf);
  oss << type << "[" << module << "," << channel << "]";
  AddActuator("Ungrouped", oss.str(),
              std::shared_ptr<LiveWindowSendable>(component,
                                                  [](LiveWindowSendable*) {}));
}

/**
 * Tell all the sensors to update (send) their values.
 *
 * Actuators are handled through callbacks on their value changing from the
 * SmartDashboard widgets.
 */
void LiveWindow::UpdateValues() {
  for (auto& elem : m_sensors) {
    elem->UpdateTable();
  }
}

/**
 * This method is called periodically to cause the sensors to send new values
 * to the SmartDashboard.
 */
void LiveWindow::Run() {
  if (m_enabled) {
    UpdateValues();
  }
}

/**
 * Initialize all the LiveWindow elements the first time we enter LiveWindow
 * mode. By holding off creating the NetworkTable entries, it allows them to be
 * redefined before the first time in LiveWindow mode. This allows default
 * sensor and actuator values to be created that are replaced with the custom
 * names from users calling addActuator and addSensor.
 */
void LiveWindow::InitializeLiveWindowComponents() {
  for (auto& elem : m_components) {
    std::shared_ptr<LiveWindowSendable> component = elem.first;
    LiveWindowComponent c = elem.second;
    std::string subsystem = c.subsystem;
    std::string name = c.name;
    m_liveWindowTable->GetSubTable(subsystem)->PutString("~TYPE~",
                                                         "LW Subsystem");
    std::shared_ptr<ITable> table(
        m_liveWindowTable->GetSubTable(subsystem)->GetSubTable(name));
    table->PutString("~TYPE~", component->GetSmartDashboardType());
    table->PutString("Name", name);
    table->PutString("Subsystem", subsystem);
    component->InitTable(table);
    if (c.isSensor) {
      m_sensors.push_back(component);
    }
  }
}
