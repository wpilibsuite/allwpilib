/*----------------------------------------------------------------------------*/
/* Copyright (c) 2012-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/livewindow/LiveWindow.h"

#include <algorithm>

#include <networktables/NetworkTable.h>
#include <networktables/NetworkTableEntry.h>
#include <networktables/NetworkTableInstance.h>
#include <wpi/DenseMap.h>
#include <wpi/SmallString.h>
#include <wpi/mutex.h>
#include <wpi/raw_ostream.h>

#include "frc/commands/Scheduler.h"
#include "frc/smartdashboard/SendableBuilderImpl.h"

using namespace frc;

using wpi::Twine;

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

  wpi::DenseMap<void*, Component> components;

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

LiveWindow* LiveWindow::GetInstance() {
  static LiveWindow instance;
  return &instance;
}

void LiveWindow::Run() { UpdateValues(); }

void LiveWindow::AddSensor(const wpi::Twine& subsystem, const wpi::Twine& name,
                           Sendable* component) {
  Add(component);
  component->SetName(subsystem, name);
}

void LiveWindow::AddSensor(const wpi::Twine& subsystem, const wpi::Twine& name,
                           Sendable& component) {
  Add(&component);
  component.SetName(subsystem, name);
}

void LiveWindow::AddSensor(const wpi::Twine& subsystem, const wpi::Twine& name,
                           std::shared_ptr<Sendable> component) {
  Add(component);
  component->SetName(subsystem, name);
}

void LiveWindow::AddActuator(const wpi::Twine& subsystem,
                             const wpi::Twine& name, Sendable* component) {
  Add(component);
  component->SetName(subsystem, name);
}

void LiveWindow::AddActuator(const wpi::Twine& subsystem,
                             const wpi::Twine& name, Sendable& component) {
  Add(&component);
  component.SetName(subsystem, name);
}

void LiveWindow::AddActuator(const wpi::Twine& subsystem,
                             const wpi::Twine& name,
                             std::shared_ptr<Sendable> component) {
  Add(component);
  component->SetName(subsystem, name);
}

void LiveWindow::AddSensor(const wpi::Twine& type, int channel,
                           Sendable* component) {
  Add(component);
  component->SetName("Ungrouped",
                     type + Twine('[') + Twine(channel) + Twine(']'));
}

void LiveWindow::AddActuator(const wpi::Twine& type, int channel,
                             Sendable* component) {
  Add(component);
  component->SetName("Ungrouped",
                     type + Twine('[') + Twine(channel) + Twine(']'));
}

void LiveWindow::AddActuator(const wpi::Twine& type, int module, int channel,
                             Sendable* component) {
  Add(component);
  component->SetName("Ungrouped", type + Twine('[') + Twine(module) +
                                      Twine(',') + Twine(channel) + Twine(']'));
}

void LiveWindow::Add(std::shared_ptr<Sendable> sendable) {
  std::lock_guard<wpi::mutex> lock(m_impl->mutex);
  auto& comp = m_impl->components[sendable.get()];
  comp.sendable = sendable;
}

void LiveWindow::Add(Sendable* sendable) {
  Add(std::shared_ptr<Sendable>(sendable, NullDeleter<Sendable>()));
}

void LiveWindow::AddChild(Sendable* parent, std::shared_ptr<Sendable> child) {
  AddChild(parent, child.get());
}

void LiveWindow::AddChild(Sendable* parent, void* child) {
  std::lock_guard<wpi::mutex> lock(m_impl->mutex);
  auto& comp = m_impl->components[child];
  comp.parent = parent;
  comp.telemetryEnabled = false;
}

void LiveWindow::Remove(Sendable* sendable) {
  std::lock_guard<wpi::mutex> lock(m_impl->mutex);
  m_impl->components.erase(sendable);
}

void LiveWindow::EnableTelemetry(Sendable* sendable) {
  std::lock_guard<wpi::mutex> lock(m_impl->mutex);
  // Re-enable global setting in case DisableAllTelemetry() was called.
  m_impl->telemetryEnabled = true;
  auto i = m_impl->components.find(sendable);
  if (i != m_impl->components.end()) i->getSecond().telemetryEnabled = true;
}

void LiveWindow::DisableTelemetry(Sendable* sendable) {
  std::lock_guard<wpi::mutex> lock(m_impl->mutex);
  auto i = m_impl->components.find(sendable);
  if (i != m_impl->components.end()) i->getSecond().telemetryEnabled = false;
}

void LiveWindow::DisableAllTelemetry() {
  std::lock_guard<wpi::mutex> lock(m_impl->mutex);
  m_impl->telemetryEnabled = false;
  for (auto& i : m_impl->components) i.getSecond().telemetryEnabled = false;
}

bool LiveWindow::IsEnabled() const {
  std::lock_guard<wpi::mutex> lock(m_impl->mutex);
  return m_impl->liveWindowEnabled;
}

void LiveWindow::SetEnabled(bool enabled) {
  std::lock_guard<wpi::mutex> lock(m_impl->mutex);
  if (m_impl->liveWindowEnabled == enabled) return;
  Scheduler* scheduler = Scheduler::GetInstance();
  m_impl->startLiveWindow = enabled;
  m_impl->liveWindowEnabled = enabled;
  // Force table generation now to make sure everything is defined
  UpdateValuesUnsafe();
  if (enabled) {
    scheduler->SetEnabled(false);
    scheduler->RemoveAll();
  } else {
    for (auto& i : m_impl->components) {
      i.getSecond().builder.StopLiveWindowMode();
    }
    scheduler->SetEnabled(true);
  }
  m_impl->enabledEntry.SetBoolean(enabled);
}

void LiveWindow::UpdateValues() {
  std::lock_guard<wpi::mutex> lock(m_impl->mutex);
  UpdateValuesUnsafe();
}

void LiveWindow::UpdateValuesUnsafe() {
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

LiveWindow::LiveWindow() : m_impl(new Impl) {}
