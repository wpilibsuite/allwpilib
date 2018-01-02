/*----------------------------------------------------------------------------*/
/* Copyright (c) 2012-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "LiveWindow/LiveWindow.h"

#include <algorithm>

#include <llvm/DenseMap.h>
#include <llvm/SmallString.h>
#include <llvm/raw_ostream.h>
#include <networktables/NetworkTable.h>
#include <networktables/NetworkTableEntry.h>
#include <networktables/NetworkTableInstance.h>
#include <support/mutex.h>

#include "Commands/Scheduler.h"
#include "SmartDashboard/SendableBuilderImpl.h"

using namespace frc;

using llvm::Twine;

struct LiveWindow::Impl {
  Impl();

  struct Component {
    std::shared_ptr<Sendable> sendable;
    Sendable* parent = nullptr;
    SendableBuilderImpl builder;
    bool firstTime = true;
    bool telemetryEnabled = true;
  };

  wpi::mutex mutex;

  llvm::DenseMap<void*, Component> components;

  std::shared_ptr<nt::NetworkTable> liveWindowTable;
  std::shared_ptr<nt::NetworkTable> statusTable;
  nt::NetworkTableEntry enabledEntry;

  bool startLiveWindow = false;
  bool liveWindowEnabled = false;
  bool telemetryEnabled = true;
};

LiveWindow::Impl::Impl()
    : liveWindowTable(
          nt::NetworkTableInstance::GetDefault().GetTable("LiveWindow")) {
  statusTable = liveWindowTable->GetSubTable(".status");
  enabledEntry = statusTable->GetEntry("LW Enabled");
}

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
LiveWindow::LiveWindow() : m_impl(new Impl) {}

bool LiveWindow::IsEnabled() const {
  std::lock_guard<wpi::mutex> lock(m_impl->mutex);
  return m_impl->liveWindowEnabled;
}

/**
 * Change the enabled status of LiveWindow.
 *
 * If it changes to enabled, start livewindow running otherwise stop it
 */
void LiveWindow::SetEnabled(bool enabled) {
  std::lock_guard<wpi::mutex> lock(m_impl->mutex);
  if (m_impl->liveWindowEnabled == enabled) return;
  Scheduler* scheduler = Scheduler::GetInstance();
  if (enabled) {
    scheduler->SetEnabled(false);
    scheduler->RemoveAll();
  } else {
    for (auto& i : m_impl->components) {
      i.getSecond().builder.StopLiveWindowMode();
    }
    scheduler->SetEnabled(true);
  }
  m_impl->startLiveWindow = enabled;
  m_impl->liveWindowEnabled = enabled;
  m_impl->enabledEntry.SetBoolean(enabled);
}

/**
 * @name AddSensor(subsystem, name, component)
 *
 * Add a Sensor associated with the subsystem and call it by the given name.
 *
 * @param subsystem The subsystem this component is part of.
 * @param name      The name of this component.
 * @param component A Sendable component that represents a sensor.
 */
//@{
/**
 * @brief Use a STL smart pointer to share ownership of component.
 * @deprecated Use Sendable::SetName() instead.
 */
void LiveWindow::AddSensor(const llvm::Twine& subsystem,
                           const llvm::Twine& name,
                           std::shared_ptr<Sendable> component) {
  Add(component);
  component->SetName(subsystem, name);
}

/**
 * @brief Pass a reference to LiveWindow and retain ownership of the component.
 * @deprecated Use Sendable::SetName() instead.
 */
void LiveWindow::AddSensor(const llvm::Twine& subsystem,
                           const llvm::Twine& name, Sendable& component) {
  Add(&component);
  component.SetName(subsystem, name);
}

/**
 * @brief Use a raw pointer to the LiveWindow.
 * @deprecated Use Sendable::SetName() instead.
 */
void LiveWindow::AddSensor(const llvm::Twine& subsystem,
                           const llvm::Twine& name, Sendable* component) {
  Add(component);
  component->SetName(subsystem, name);
}
//@}

/**
 * @name AddActuator(subsystem, name, component)
 *
 * Add an Actuator associated with the subsystem and call it by the given name.
 * @deprecated Use Sendable::SetName() instead.
 *
 * @param subsystem The subsystem this component is part of.
 * @param name      The name of this component.
 * @param component A Sendable component that represents a actuator.
 */
//@{
/**
 * @brief Use a STL smart pointer to share ownership of component.
 */
void LiveWindow::AddActuator(const llvm::Twine& subsystem,
                             const llvm::Twine& name,
                             std::shared_ptr<Sendable> component) {
  Add(component);
  component->SetName(subsystem, name);
}

/**
 * @brief Pass a reference to LiveWindow and retain ownership of the component.
 * @deprecated Use Sendable::SetName() instead.
 */
void LiveWindow::AddActuator(const llvm::Twine& subsystem,
                             const llvm::Twine& name, Sendable& component) {
  Add(&component);
  component.SetName(subsystem, name);
}

/**
 * @brief Use a raw pointer to the LiveWindow.
 * @deprecated Use Sendable::SetName() instead.
 */
void LiveWindow::AddActuator(const llvm::Twine& subsystem,
                             const llvm::Twine& name, Sendable* component) {
  Add(component);
  component->SetName(subsystem, name);
}
//@}

/**
 * Meant for internal use in other WPILib classes.
 * @deprecated Use SensorBase::SetName() instead.
 */
void LiveWindow::AddSensor(const llvm::Twine& type, int channel,
                           Sendable* component) {
  Add(component);
  component->SetName("Ungrouped",
                     type + Twine('[') + Twine(channel) + Twine(']'));
}

/**
 * Meant for internal use in other WPILib classes.
 * @deprecated Use SensorBase::SetName() instead.
 */
void LiveWindow::AddActuator(const llvm::Twine& type, int channel,
                             Sendable* component) {
  Add(component);
  component->SetName("Ungrouped",
                     type + Twine('[') + Twine(channel) + Twine(']'));
}

/**
 * Meant for internal use in other WPILib classes.
 * @deprecated Use SensorBase::SetName() instead.
 */
void LiveWindow::AddActuator(const llvm::Twine& type, int module, int channel,
                             Sendable* component) {
  Add(component);
  component->SetName("Ungrouped", type + Twine('[') + Twine(module) +
                                      Twine(',') + Twine(channel) + Twine(']'));
}

/**
 * Add a component to the LiveWindow.
 *
 * @param sendable component to add
 */
void LiveWindow::Add(std::shared_ptr<Sendable> sendable) {
  std::lock_guard<wpi::mutex> lock(m_impl->mutex);
  auto& comp = m_impl->components[sendable.get()];
  comp.sendable = sendable;
}

/**
 * Add a component to the LiveWindow.
 *
 * @param sendable component to add
 */
void LiveWindow::Add(Sendable* sendable) {
  Add(std::shared_ptr<Sendable>(sendable, NullDeleter<Sendable>()));
}

/**
 * Add a child component to a component.
 *
 * @param parent parent component
 * @param child child component
 */
void LiveWindow::AddChild(Sendable* parent, std::shared_ptr<Sendable> child) {
  AddChild(parent, child.get());
}

/**
 * Add a child component to a component.
 *
 * @param parent parent component
 * @param child child component
 */
void LiveWindow::AddChild(Sendable* parent, void* child) {
  std::lock_guard<wpi::mutex> lock(m_impl->mutex);
  auto& comp = m_impl->components[child];
  comp.parent = parent;
  comp.telemetryEnabled = false;
}

/**
 * Remove the component from the LiveWindow.
 *
 * @param sendable component to remove
 */
void LiveWindow::Remove(Sendable* sendable) {
  std::lock_guard<wpi::mutex> lock(m_impl->mutex);
  m_impl->components.erase(sendable);
}

/**
 * Enable telemetry for a single component.
 *
 * @param sendable component
 */
void LiveWindow::EnableTelemetry(Sendable* sendable) {
  std::lock_guard<wpi::mutex> lock(m_impl->mutex);
  // Re-enable global setting in case DisableAllTelemetry() was called.
  m_impl->telemetryEnabled = true;
  auto i = m_impl->components.find(sendable);
  if (i != m_impl->components.end()) i->getSecond().telemetryEnabled = true;
}

/**
 * Disable telemetry for a single component.
 *
 * @param sendable component
 */
void LiveWindow::DisableTelemetry(Sendable* sendable) {
  std::lock_guard<wpi::mutex> lock(m_impl->mutex);
  auto i = m_impl->components.find(sendable);
  if (i != m_impl->components.end()) i->getSecond().telemetryEnabled = false;
}

/**
 * Disable ALL telemetry.
 */
void LiveWindow::DisableAllTelemetry() {
  std::lock_guard<wpi::mutex> lock(m_impl->mutex);
  m_impl->telemetryEnabled = false;
  for (auto& i : m_impl->components) i.getSecond().telemetryEnabled = false;
}

/**
 * Tell all the sensors to update (send) their values.
 *
 * Actuators are handled through callbacks on their value changing from the
 * SmartDashboard widgets.
 */
void LiveWindow::UpdateValues() {
  std::lock_guard<wpi::mutex> lock(m_impl->mutex);
  // Only do this if either LiveWindow mode or telemetry is enabled.
  if (!m_impl->liveWindowEnabled && !m_impl->telemetryEnabled) return;

  for (auto& i : m_impl->components) {
    auto& comp = i.getSecond();
    if (comp.sendable && !comp.parent &&
        (m_impl->liveWindowEnabled || comp.telemetryEnabled)) {
      if (comp.firstTime) {
        // By holding off creating the NetworkTable entries, it allows the
        // components to be redefined. This allows default sensor and actuator
        // values to be created that are replaced with the custom names from
        // users calling setName.
        auto name = comp.sendable->GetName();
        if (name.empty()) continue;
        auto subsystem = comp.sendable->GetSubsystem();
        auto ssTable = m_impl->liveWindowTable->GetSubTable(subsystem);
        std::shared_ptr<NetworkTable> table;
        // Treat name==subsystem as top level of subsystem
        if (name == subsystem)
          table = ssTable;
        else
          table = ssTable->GetSubTable(name);
        table->GetEntry(".name").SetString(name);
        comp.builder.SetTable(table);
        comp.sendable->InitSendable(comp.builder);
        ssTable->GetEntry(".type").SetString("LW Subsystem");

        comp.firstTime = false;
      }

      if (m_impl->startLiveWindow) comp.builder.StartLiveWindowMode();
      comp.builder.UpdateTable();
    }
  }

  m_impl->startLiveWindow = false;
}
