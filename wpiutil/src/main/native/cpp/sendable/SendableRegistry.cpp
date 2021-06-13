// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/sendable/SendableRegistry.h"

#include <memory>

#include "fmt/format.h"
#include "wpi/DenseMap.h"
#include "wpi/SmallVector.h"
#include "wpi/UidVector.h"
#include "wpi/mutex.h"
#include "wpi/sendable/Sendable.h"
#include "wpi/sendable/SendableBuilder.h"

using namespace wpi;

struct SendableRegistry::Impl {
  struct Component {
    Sendable* sendable = nullptr;
    std::unique_ptr<SendableBuilder> builder;
    std::string name;
    std::string subsystem = "Ungrouped";
    Sendable* parent = nullptr;
    bool liveWindow = false;
    wpi::SmallVector<std::shared_ptr<void>, 2> data;

    void SetName(std::string_view moduleType, int channel) {
      name = fmt::format("{}[{}]", moduleType, channel);
    }

    void SetName(std::string_view moduleType, int moduleNumber, int channel) {
      name = fmt::format("{}[{},{}]", moduleType, moduleNumber, channel);
    }
  };

  wpi::recursive_mutex mutex;

  std::function<std::unique_ptr<SendableBuilder>()> liveWindowFactory;
  wpi::UidVector<std::unique_ptr<Component>, 32> components;
  wpi::DenseMap<void*, UID> componentMap;
  int nextDataHandle = 0;

  Component& GetOrAdd(void* sendable, UID* uid = nullptr);
};

SendableRegistry::Impl::Component& SendableRegistry::Impl::GetOrAdd(
    void* sendable, UID* uid) {
  UID& compUid = componentMap[sendable];
  if (compUid == 0) {
    compUid = components.emplace_back(std::make_unique<Component>()) + 1;
  }
  if (uid) {
    *uid = compUid;
  }

  return *components[compUid - 1];
}

SendableRegistry& SendableRegistry::GetInstance() {
  static SendableRegistry instance;
  return instance;
}

void SendableRegistry::SetLiveWindowBuilderFactory(
    std::function<std::unique_ptr<SendableBuilder>()> factory) {
  m_impl->liveWindowFactory = std::move(factory);
}

void SendableRegistry::Add(Sendable* sendable, std::string_view name) {
  std::scoped_lock lock(m_impl->mutex);
  auto& comp = m_impl->GetOrAdd(sendable);
  comp.sendable = sendable;
  comp.name = name;
}

void SendableRegistry::Add(Sendable* sendable, std::string_view moduleType,
                           int channel) {
  std::scoped_lock lock(m_impl->mutex);
  auto& comp = m_impl->GetOrAdd(sendable);
  comp.sendable = sendable;
  comp.SetName(moduleType, channel);
}

void SendableRegistry::Add(Sendable* sendable, std::string_view moduleType,
                           int moduleNumber, int channel) {
  std::scoped_lock lock(m_impl->mutex);
  auto& comp = m_impl->GetOrAdd(sendable);
  comp.sendable = sendable;
  comp.SetName(moduleType, moduleNumber, channel);
}

void SendableRegistry::Add(Sendable* sendable, std::string_view subsystem,
                           std::string_view name) {
  std::scoped_lock lock(m_impl->mutex);
  auto& comp = m_impl->GetOrAdd(sendable);
  comp.sendable = sendable;
  comp.name = name;
  comp.subsystem = subsystem;
}

void SendableRegistry::AddLW(Sendable* sendable, std::string_view name) {
  std::scoped_lock lock(m_impl->mutex);
  auto& comp = m_impl->GetOrAdd(sendable);
  comp.sendable = sendable;
  if (m_impl->liveWindowFactory) {
    comp.builder = m_impl->liveWindowFactory();
  }
  comp.liveWindow = true;
  comp.name = name;
}

void SendableRegistry::AddLW(Sendable* sendable, std::string_view moduleType,
                             int channel) {
  std::scoped_lock lock(m_impl->mutex);
  auto& comp = m_impl->GetOrAdd(sendable);
  comp.sendable = sendable;
  if (m_impl->liveWindowFactory) {
    comp.builder = m_impl->liveWindowFactory();
  }
  comp.liveWindow = true;
  comp.SetName(moduleType, channel);
}

void SendableRegistry::AddLW(Sendable* sendable, std::string_view moduleType,
                             int moduleNumber, int channel) {
  std::scoped_lock lock(m_impl->mutex);
  auto& comp = m_impl->GetOrAdd(sendable);
  comp.sendable = sendable;
  if (m_impl->liveWindowFactory) {
    comp.builder = m_impl->liveWindowFactory();
  }
  comp.liveWindow = true;
  comp.SetName(moduleType, moduleNumber, channel);
}

void SendableRegistry::AddLW(Sendable* sendable, std::string_view subsystem,
                             std::string_view name) {
  std::scoped_lock lock(m_impl->mutex);
  auto& comp = m_impl->GetOrAdd(sendable);
  comp.sendable = sendable;
  if (m_impl->liveWindowFactory) {
    comp.builder = m_impl->liveWindowFactory();
  }
  comp.liveWindow = true;
  comp.name = name;
  comp.subsystem = subsystem;
}

void SendableRegistry::AddChild(Sendable* parent, Sendable* child) {
  std::scoped_lock lock(m_impl->mutex);
  auto& comp = m_impl->GetOrAdd(child);
  comp.parent = parent;
}

void SendableRegistry::AddChild(Sendable* parent, void* child) {
  std::scoped_lock lock(m_impl->mutex);
  auto& comp = m_impl->GetOrAdd(child);
  comp.parent = parent;
}

bool SendableRegistry::Remove(Sendable* sendable) {
  std::scoped_lock lock(m_impl->mutex);
  auto it = m_impl->componentMap.find(sendable);
  if (it == m_impl->componentMap.end()) {
    return false;
  }
  UID compUid = it->getSecond();
  m_impl->components.erase(compUid - 1);
  m_impl->componentMap.erase(it);
  // update any parent pointers
  for (auto&& comp : m_impl->components) {
    if (comp->parent == sendable) {
      comp->parent = nullptr;
    }
  }
  return true;
}

void SendableRegistry::Move(Sendable* to, Sendable* from) {
  std::scoped_lock lock(m_impl->mutex);
  auto it = m_impl->componentMap.find(from);
  if (it == m_impl->componentMap.end() ||
      !m_impl->components[it->getSecond() - 1]) {
    return;
  }
  UID compUid = it->getSecond();
  m_impl->componentMap.erase(it);
  m_impl->componentMap[to] = compUid;
  auto& comp = *m_impl->components[compUid - 1];
  comp.sendable = to;
  if (comp.builder && comp.builder->IsPublished()) {
    // rebuild builder, as lambda captures can point to "from"
    comp.builder->ClearProperties();
    to->InitSendable(*comp.builder);
  }
  // update any parent pointers
  for (auto&& comp : m_impl->components) {
    if (comp->parent == from) {
      comp->parent = to;
    }
  }
}

bool SendableRegistry::Contains(const Sendable* sendable) const {
  std::scoped_lock lock(m_impl->mutex);
  return m_impl->componentMap.count(sendable) != 0;
}

std::string SendableRegistry::GetName(const Sendable* sendable) const {
  std::scoped_lock lock(m_impl->mutex);
  auto it = m_impl->componentMap.find(sendable);
  if (it == m_impl->componentMap.end() ||
      !m_impl->components[it->getSecond() - 1]) {
    return {};
  }
  return m_impl->components[it->getSecond() - 1]->name;
}

void SendableRegistry::SetName(Sendable* sendable, std::string_view name) {
  std::scoped_lock lock(m_impl->mutex);
  auto it = m_impl->componentMap.find(sendable);
  if (it == m_impl->componentMap.end() ||
      !m_impl->components[it->getSecond() - 1]) {
    return;
  }
  m_impl->components[it->getSecond() - 1]->name = name;
}

void SendableRegistry::SetName(Sendable* sendable, std::string_view moduleType,
                               int channel) {
  std::scoped_lock lock(m_impl->mutex);
  auto it = m_impl->componentMap.find(sendable);
  if (it == m_impl->componentMap.end() ||
      !m_impl->components[it->getSecond() - 1]) {
    return;
  }
  m_impl->components[it->getSecond() - 1]->SetName(moduleType, channel);
}

void SendableRegistry::SetName(Sendable* sendable, std::string_view moduleType,
                               int moduleNumber, int channel) {
  std::scoped_lock lock(m_impl->mutex);
  auto it = m_impl->componentMap.find(sendable);
  if (it == m_impl->componentMap.end() ||
      !m_impl->components[it->getSecond() - 1]) {
    return;
  }
  m_impl->components[it->getSecond() - 1]->SetName(moduleType, moduleNumber,
                                                   channel);
}

void SendableRegistry::SetName(Sendable* sendable, std::string_view subsystem,
                               std::string_view name) {
  std::scoped_lock lock(m_impl->mutex);
  auto it = m_impl->componentMap.find(sendable);
  if (it == m_impl->componentMap.end() ||
      !m_impl->components[it->getSecond() - 1]) {
    return;
  }
  auto& comp = *m_impl->components[it->getSecond() - 1];
  comp.name = name;
  comp.subsystem = subsystem;
}

std::string SendableRegistry::GetSubsystem(const Sendable* sendable) const {
  std::scoped_lock lock(m_impl->mutex);
  auto it = m_impl->componentMap.find(sendable);
  if (it == m_impl->componentMap.end() ||
      !m_impl->components[it->getSecond() - 1]) {
    return {};
  }
  return m_impl->components[it->getSecond() - 1]->subsystem;
}

void SendableRegistry::SetSubsystem(Sendable* sendable,
                                    std::string_view subsystem) {
  std::scoped_lock lock(m_impl->mutex);
  auto it = m_impl->componentMap.find(sendable);
  if (it == m_impl->componentMap.end() ||
      !m_impl->components[it->getSecond() - 1]) {
    return;
  }
  m_impl->components[it->getSecond() - 1]->subsystem = subsystem;
}

int SendableRegistry::GetDataHandle() {
  std::scoped_lock lock(m_impl->mutex);
  return m_impl->nextDataHandle++;
}

std::shared_ptr<void> SendableRegistry::SetData(Sendable* sendable, int handle,
                                                std::shared_ptr<void> data) {
  assert(handle >= 0);
  std::scoped_lock lock(m_impl->mutex);
  auto it = m_impl->componentMap.find(sendable);
  if (it == m_impl->componentMap.end() ||
      !m_impl->components[it->getSecond() - 1]) {
    return nullptr;
  }
  auto& comp = *m_impl->components[it->getSecond() - 1];
  std::shared_ptr<void> rv;
  if (static_cast<size_t>(handle) < comp.data.size()) {
    rv = std::move(comp.data[handle]);
  } else {
    comp.data.resize(handle + 1);
  }
  comp.data[handle] = std::move(data);
  return rv;
}

std::shared_ptr<void> SendableRegistry::GetData(Sendable* sendable,
                                                int handle) {
  assert(handle >= 0);
  std::scoped_lock lock(m_impl->mutex);
  auto it = m_impl->componentMap.find(sendable);
  if (it == m_impl->componentMap.end() ||
      !m_impl->components[it->getSecond() - 1]) {
    return nullptr;
  }
  auto& comp = *m_impl->components[it->getSecond() - 1];
  if (static_cast<size_t>(handle) >= comp.data.size()) {
    return nullptr;
  }
  return comp.data[handle];
}

void SendableRegistry::EnableLiveWindow(Sendable* sendable) {
  std::scoped_lock lock(m_impl->mutex);
  auto it = m_impl->componentMap.find(sendable);
  if (it == m_impl->componentMap.end() ||
      !m_impl->components[it->getSecond() - 1]) {
    return;
  }
  m_impl->components[it->getSecond() - 1]->liveWindow = true;
}

void SendableRegistry::DisableLiveWindow(Sendable* sendable) {
  std::scoped_lock lock(m_impl->mutex);
  auto it = m_impl->componentMap.find(sendable);
  if (it == m_impl->componentMap.end() ||
      !m_impl->components[it->getSecond() - 1]) {
    return;
  }
  m_impl->components[it->getSecond() - 1]->liveWindow = false;
}

SendableRegistry::UID SendableRegistry::GetUniqueId(Sendable* sendable) {
  std::scoped_lock lock(m_impl->mutex);
  UID uid;
  auto& comp = m_impl->GetOrAdd(sendable, &uid);
  comp.sendable = sendable;
  return uid;
}

Sendable* SendableRegistry::GetSendable(UID uid) {
  if (uid == 0) {
    return nullptr;
  }
  std::scoped_lock lock(m_impl->mutex);
  if ((uid - 1) >= m_impl->components.size() || !m_impl->components[uid - 1]) {
    return nullptr;
  }
  return m_impl->components[uid - 1]->sendable;
}

void SendableRegistry::Publish(UID sendableUid,
                               std::unique_ptr<SendableBuilder> builder) {
  std::scoped_lock lock(m_impl->mutex);
  if (sendableUid == 0 || (sendableUid - 1) >= m_impl->components.size() ||
      !m_impl->components[sendableUid - 1]) {
    return;
  }
  auto& comp = *m_impl->components[sendableUid - 1];
  comp.builder = std::move(builder);  // clear any current builder
  comp.sendable->InitSendable(*comp.builder);
  comp.builder->Update();
}

void SendableRegistry::Update(UID sendableUid) {
  if (sendableUid == 0) {
    return;
  }
  std::scoped_lock lock(m_impl->mutex);
  if ((sendableUid - 1) >= m_impl->components.size() ||
      !m_impl->components[sendableUid - 1]) {
    return;
  }
  if (m_impl->components[sendableUid - 1]->builder) {
    m_impl->components[sendableUid - 1]->builder->Update();
  }
}

void SendableRegistry::ForeachLiveWindow(
    int dataHandle,
    wpi::function_ref<void(CallbackData& data)> callback) const {
  assert(dataHandle >= 0);
  std::scoped_lock lock(m_impl->mutex);
  wpi::SmallVector<Impl::Component*, 128> components;
  for (auto&& comp : m_impl->components) {
    components.emplace_back(comp.get());
  }
  for (auto comp : components) {
    if (comp && comp->builder && comp->sendable && comp->liveWindow) {
      if (static_cast<size_t>(dataHandle) >= comp->data.size()) {
        comp->data.resize(dataHandle + 1);
      }
      CallbackData cbdata{comp->sendable,         comp->name,
                          comp->subsystem,        comp->parent,
                          comp->data[dataHandle], *comp->builder};
      callback(cbdata);
    }
  }
}

SendableRegistry::SendableRegistry() : m_impl(new Impl) {}
