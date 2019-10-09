/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/smartdashboard/SendableRegistry.h"

#include <wpi/DenseMap.h>
#include <wpi/SmallVector.h>
#include <wpi/mutex.h>

using namespace frc;

struct SendableRegistry::Impl {
  struct Component {
    Sendable* sendable = nullptr;
    std::string name;
    std::string subsystem = "Ungrouped";
    Sendable* parent = nullptr;
    bool liveWindow = false;
    wpi::SmallVector<std::shared_ptr<void>, 2> data;

    void SetName(const wpi::Twine& moduleType, int channel) {
      name =
          (moduleType + wpi::Twine('[') + wpi::Twine(channel) + wpi::Twine(']'))
              .str();
    }

    void SetName(const wpi::Twine& moduleType, int moduleNumber, int channel) {
      name = (moduleType + wpi::Twine('[') + wpi::Twine(moduleNumber) +
              wpi::Twine(',') + wpi::Twine(channel) + wpi::Twine(']'))
                 .str();
    }
  };

  wpi::mutex mutex;

  wpi::DenseMap<void*, Component> components;
  int nextDataHandle = 0;

  Component& GetOrAdd(Sendable* sendable);
};

SendableRegistry::Impl::Component& SendableRegistry::Impl::GetOrAdd(
    Sendable* sendable) {
  auto& comp = components[sendable];
  comp.sendable = sendable;
  return comp;
}

SendableRegistry& SendableRegistry::GetInstance() {
  static SendableRegistry instance;
  return instance;
}

void SendableRegistry::Add(Sendable* sendable, const wpi::Twine& name) {
  std::scoped_lock lock(m_impl->mutex);
  auto& comp = m_impl->GetOrAdd(sendable);
  comp.name = name.str();
}

void SendableRegistry::Add(Sendable* sendable, const wpi::Twine& moduleType,
                           int channel) {
  std::scoped_lock lock(m_impl->mutex);
  auto& comp = m_impl->GetOrAdd(sendable);
  comp.SetName(moduleType, channel);
}

void SendableRegistry::Add(Sendable* sendable, const wpi::Twine& moduleType,
                           int moduleNumber, int channel) {
  std::scoped_lock lock(m_impl->mutex);
  auto& comp = m_impl->GetOrAdd(sendable);
  comp.SetName(moduleType, moduleNumber, channel);
}

void SendableRegistry::Add(Sendable* sendable, const wpi::Twine& subsystem,
                           const wpi::Twine& name) {
  std::scoped_lock lock(m_impl->mutex);
  auto& comp = m_impl->GetOrAdd(sendable);
  comp.name = name.str();
  comp.subsystem = subsystem.str();
}

void SendableRegistry::AddLW(Sendable* sendable, const wpi::Twine& name) {
  std::scoped_lock lock(m_impl->mutex);
  auto& comp = m_impl->GetOrAdd(sendable);
  comp.liveWindow = true;
  comp.name = name.str();
}

void SendableRegistry::AddLW(Sendable* sendable, const wpi::Twine& moduleType,
                             int channel) {
  std::scoped_lock lock(m_impl->mutex);
  auto& comp = m_impl->GetOrAdd(sendable);
  comp.liveWindow = true;
  comp.SetName(moduleType, channel);
}

void SendableRegistry::AddLW(Sendable* sendable, const wpi::Twine& moduleType,
                             int moduleNumber, int channel) {
  std::scoped_lock lock(m_impl->mutex);
  auto& comp = m_impl->GetOrAdd(sendable);
  comp.liveWindow = true;
  comp.SetName(moduleType, moduleNumber, channel);
}

void SendableRegistry::AddLW(Sendable* sendable, const wpi::Twine& subsystem,
                             const wpi::Twine& name) {
  std::scoped_lock lock(m_impl->mutex);
  auto& comp = m_impl->GetOrAdd(sendable);
  comp.liveWindow = true;
  comp.name = name.str();
  comp.subsystem = subsystem.str();
}

void SendableRegistry::AddChild(Sendable* parent, void* child) {
  std::scoped_lock lock(m_impl->mutex);
  auto& comp = m_impl->components[child];
  comp.parent = parent;
}

bool SendableRegistry::Remove(Sendable* sendable) {
  std::scoped_lock lock(m_impl->mutex);
  return m_impl->components.erase(sendable);
}

void SendableRegistry::Move(Sendable* to, Sendable* from) {
  std::scoped_lock lock(m_impl->mutex);
  auto it = m_impl->components.find(from);
  if (it == m_impl->components.end()) return;
  Impl::Component old = std::move(it->getSecond());
  m_impl->components.erase(it);
  m_impl->components[to] = std::move(old);
  m_impl->components[to].sendable = to;
}

bool SendableRegistry::Contains(const Sendable* sendable) const {
  std::scoped_lock lock(m_impl->mutex);
  return m_impl->components.count(sendable) != 0;
}

std::string SendableRegistry::GetName(const Sendable* sendable) const {
  std::scoped_lock lock(m_impl->mutex);
  auto it = m_impl->components.find(sendable);
  if (it == m_impl->components.end()) return std::string{};
  return it->getSecond().name;
}

void SendableRegistry::SetName(Sendable* sendable, const wpi::Twine& name) {
  std::scoped_lock lock(m_impl->mutex);
  auto it = m_impl->components.find(sendable);
  if (it == m_impl->components.end()) return;
  it->getSecond().name = name.str();
}

void SendableRegistry::SetName(Sendable* sendable, const wpi::Twine& moduleType,
                               int channel) {
  std::scoped_lock lock(m_impl->mutex);
  auto it = m_impl->components.find(sendable);
  if (it == m_impl->components.end()) return;
  it->getSecond().SetName(moduleType, channel);
}

void SendableRegistry::SetName(Sendable* sendable, const wpi::Twine& moduleType,
                               int moduleNumber, int channel) {
  std::scoped_lock lock(m_impl->mutex);
  auto it = m_impl->components.find(sendable);
  if (it == m_impl->components.end()) return;
  it->getSecond().SetName(moduleType, moduleNumber, channel);
}

void SendableRegistry::SetName(Sendable* sendable, const wpi::Twine& subsystem,
                               const wpi::Twine& name) {
  std::scoped_lock lock(m_impl->mutex);
  auto it = m_impl->components.find(sendable);
  if (it == m_impl->components.end()) return;
  it->getSecond().name = name.str();
  it->getSecond().subsystem = subsystem.str();
}

std::string SendableRegistry::GetSubsystem(const Sendable* sendable) const {
  std::scoped_lock lock(m_impl->mutex);
  auto it = m_impl->components.find(sendable);
  if (it == m_impl->components.end()) return std::string{};
  return it->getSecond().subsystem;
}

void SendableRegistry::SetSubsystem(Sendable* sendable,
                                    const wpi::Twine& subsystem) {
  std::scoped_lock lock(m_impl->mutex);
  auto it = m_impl->components.find(sendable);
  if (it == m_impl->components.end()) return;
  it->getSecond().subsystem = subsystem.str();
}

int SendableRegistry::GetDataHandle() {
  std::scoped_lock lock(m_impl->mutex);
  return m_impl->nextDataHandle++;
}

std::shared_ptr<void> SendableRegistry::SetData(Sendable* sendable, int handle,
                                                std::shared_ptr<void> data) {
  assert(handle >= 0);
  std::scoped_lock lock(m_impl->mutex);
  auto it = m_impl->components.find(sendable);
  if (it == m_impl->components.end()) return nullptr;
  auto& comp = it->getSecond();
  std::shared_ptr<void> rv;
  if (static_cast<size_t>(handle) < comp.data.size())
    rv = std::move(comp.data[handle]);
  else
    comp.data.resize(handle + 1);
  comp.data[handle] = std::move(data);
  return rv;
}

std::shared_ptr<void> SendableRegistry::GetData(Sendable* sendable,
                                                int handle) {
  assert(handle >= 0);
  std::scoped_lock lock(m_impl->mutex);
  auto it = m_impl->components.find(sendable);
  if (it == m_impl->components.end()) return nullptr;
  auto& comp = it->getSecond();
  if (static_cast<size_t>(handle) >= comp.data.size()) return nullptr;
  return comp.data[handle];
}

void SendableRegistry::EnableLiveWindow(Sendable* sendable) {
  std::scoped_lock lock(m_impl->mutex);
  auto it = m_impl->components.find(sendable);
  if (it == m_impl->components.end()) return;
  it->getSecond().liveWindow = true;
}

void SendableRegistry::DisableLiveWindow(Sendable* sendable) {
  std::scoped_lock lock(m_impl->mutex);
  auto it = m_impl->components.find(sendable);
  if (it == m_impl->components.end()) return;
  it->getSecond().liveWindow = false;
}

void SendableRegistry::ForeachLiveWindow(
    int dataHandle,
    wpi::function_ref<void(Sendable* sendable, wpi::StringRef name,
                           wpi::StringRef subsystem, Sendable* parent,
                           std::shared_ptr<void>& data)>
        callback) const {
  assert(dataHandle >= 0);
  std::scoped_lock lock(m_impl->mutex);
  for (auto& i : m_impl->components) {
    auto& comp = i.getSecond();
    if (comp.sendable && comp.liveWindow) {
      if (static_cast<size_t>(dataHandle) >= comp.data.size())
        comp.data.resize(dataHandle + 1);
      callback(comp.sendable, comp.name, comp.subsystem, comp.parent,
               comp.data[dataHandle]);
    }
  }
}

SendableRegistry::SendableRegistry() : m_impl(new Impl) {}
