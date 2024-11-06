// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/sendable/SendableRegistry.h"

#include <memory>
#include <string>
#include <utility>

#include <fmt/format.h>

#include "wpi/DenseMap.h"
#include "wpi/SmallVector.h"
#include "wpi/UidVector.h"
#include "wpi/mutex.h"
#include "wpi/sendable/Sendable.h"
#include "wpi/sendable/SendableBuilder.h"

using namespace wpi;

namespace {
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

struct SendableRegistryInst {
  wpi::recursive_mutex mutex;

  std::function<std::unique_ptr<SendableBuilder>()> liveWindowFactory;
  wpi::UidVector<std::unique_ptr<Component>, 32> components;
  wpi::DenseMap<void*, SendableRegistry::UID> componentMap;
  int nextDataHandle = 0;

  Component& GetOrAdd(void* sendable, SendableRegistry::UID* uid = nullptr);
};
}  // namespace

Component& SendableRegistryInst::GetOrAdd(void* sendable,
                                          SendableRegistry::UID* uid) {
  SendableRegistry::UID& compUid = componentMap[sendable];
  if (compUid == 0) {
    compUid = components.emplace_back(std::make_unique<Component>()) + 1;
  }
  if (uid) {
    *uid = compUid;
  }

  return *components[compUid - 1];
}

static std::unique_ptr<SendableRegistryInst>& GetInstanceHolder() {
  static std::unique_ptr<SendableRegistryInst> instance =
      std::make_unique<SendableRegistryInst>();
  return instance;
}

static SendableRegistryInst& GetInstance() {
  return *GetInstanceHolder();
}

#ifndef __FRC_ROBORIO__
namespace wpi::impl {
void ResetSendableRegistry() {
  std::make_unique<SendableRegistryInst>().swap(GetInstanceHolder());
}
}  // namespace wpi::impl
#endif

void SendableRegistry::EnsureInitialized() {
  GetInstance();
}

void SendableRegistry::SetLiveWindowBuilderFactory(
    std::function<std::unique_ptr<SendableBuilder>()> factory) {
  GetInstance().liveWindowFactory = std::move(factory);
}

void SendableRegistry::Add(Sendable* sendable, std::string_view name) {
  auto& inst = GetInstance();
  std::scoped_lock lock(inst.mutex);
  auto& comp = inst.GetOrAdd(sendable);
  comp.sendable = sendable;
  comp.name = name;
}

void SendableRegistry::Add(Sendable* sendable, std::string_view moduleType,
                           int channel) {
  auto& inst = GetInstance();
  std::scoped_lock lock(inst.mutex);
  auto& comp = inst.GetOrAdd(sendable);
  comp.sendable = sendable;
  comp.SetName(moduleType, channel);
}

void SendableRegistry::Add(Sendable* sendable, std::string_view moduleType,
                           int moduleNumber, int channel) {
  auto& inst = GetInstance();
  std::scoped_lock lock(inst.mutex);
  auto& comp = inst.GetOrAdd(sendable);
  comp.sendable = sendable;
  comp.SetName(moduleType, moduleNumber, channel);
}

void SendableRegistry::Add(Sendable* sendable, std::string_view subsystem,
                           std::string_view name) {
  auto& inst = GetInstance();
  std::scoped_lock lock(inst.mutex);
  auto& comp = inst.GetOrAdd(sendable);
  comp.sendable = sendable;
  comp.name = name;
  comp.subsystem = subsystem;
}

void SendableRegistry::AddLW(Sendable* sendable, std::string_view name) {
  auto& inst = GetInstance();
  std::scoped_lock lock(inst.mutex);
  auto& comp = inst.GetOrAdd(sendable);
  comp.sendable = sendable;
  if (inst.liveWindowFactory) {
    comp.builder = inst.liveWindowFactory();
  }
  comp.liveWindow = true;
  comp.name = name;
}

void SendableRegistry::AddLW(Sendable* sendable, std::string_view moduleType,
                             int channel) {
  auto& inst = GetInstance();
  std::scoped_lock lock(inst.mutex);
  auto& comp = inst.GetOrAdd(sendable);
  comp.sendable = sendable;
  if (inst.liveWindowFactory) {
    comp.builder = inst.liveWindowFactory();
  }
  comp.liveWindow = true;
  comp.SetName(moduleType, channel);
}

void SendableRegistry::AddLW(Sendable* sendable, std::string_view moduleType,
                             int moduleNumber, int channel) {
  auto& inst = GetInstance();
  std::scoped_lock lock(inst.mutex);
  auto& comp = inst.GetOrAdd(sendable);
  comp.sendable = sendable;
  if (inst.liveWindowFactory) {
    comp.builder = inst.liveWindowFactory();
  }
  comp.liveWindow = true;
  comp.SetName(moduleType, moduleNumber, channel);
}

void SendableRegistry::AddLW(Sendable* sendable, std::string_view subsystem,
                             std::string_view name) {
  auto& inst = GetInstance();
  std::scoped_lock lock(inst.mutex);
  auto& comp = inst.GetOrAdd(sendable);
  comp.sendable = sendable;
  if (inst.liveWindowFactory) {
    comp.builder = inst.liveWindowFactory();
  }
  comp.liveWindow = true;
  comp.name = name;
  comp.subsystem = subsystem;
}

void SendableRegistry::AddChild(Sendable* parent, Sendable* child) {
  auto& inst = GetInstance();
  std::scoped_lock lock(inst.mutex);
  auto& comp = inst.GetOrAdd(child);
  comp.parent = parent;
}

void SendableRegistry::AddChild(Sendable* parent, void* child) {
  auto& inst = GetInstance();
  std::scoped_lock lock(inst.mutex);
  auto& comp = inst.GetOrAdd(child);
  comp.parent = parent;
}

bool SendableRegistry::Remove(Sendable* sendable) {
  auto& inst = GetInstance();
  std::scoped_lock lock(inst.mutex);
  auto it = inst.componentMap.find(sendable);
  if (it == inst.componentMap.end()) {
    return false;
  }
  UID compUid = it->getSecond();
  inst.components.erase(compUid - 1);
  inst.componentMap.erase(it);
  // update any parent pointers
  for (auto&& comp : inst.components) {
    if (comp->parent == sendable) {
      comp->parent = nullptr;
    }
  }
  return true;
}

void SendableRegistry::Move(Sendable* to, Sendable* from) {
  auto& inst = GetInstance();
  std::scoped_lock lock(inst.mutex);
  auto it = inst.componentMap.find(from);
  if (it == inst.componentMap.end() || !inst.components[it->getSecond() - 1]) {
    return;
  }
  UID compUid = it->getSecond();
  inst.componentMap.erase(it);
  inst.componentMap[to] = compUid;
  auto& comp = *inst.components[compUid - 1];
  comp.sendable = to;
  if (comp.builder && comp.builder->IsPublished()) {
    // rebuild builder, as lambda captures can point to "from"
    comp.builder->ClearProperties();
    to->InitSendable(*comp.builder);
  }
  // update any parent pointers
  for (auto&& comp : inst.components) {
    if (comp->parent == from) {
      comp->parent = to;
    }
  }
}

bool SendableRegistry::Contains(const Sendable* sendable) {
  auto& inst = GetInstance();
  std::scoped_lock lock(inst.mutex);
  return inst.componentMap.count(sendable) != 0;
}

std::string SendableRegistry::GetName(const Sendable* sendable) {
  auto& inst = GetInstance();
  std::scoped_lock lock(inst.mutex);
  auto it = inst.componentMap.find(sendable);
  if (it == inst.componentMap.end() || !inst.components[it->getSecond() - 1]) {
    return {};
  }
  return inst.components[it->getSecond() - 1]->name;
}

void SendableRegistry::SetName(Sendable* sendable, std::string_view name) {
  auto& inst = GetInstance();
  std::scoped_lock lock(inst.mutex);
  auto it = inst.componentMap.find(sendable);
  if (it == inst.componentMap.end() || !inst.components[it->getSecond() - 1]) {
    return;
  }
  inst.components[it->getSecond() - 1]->name = name;
}

void SendableRegistry::SetName(Sendable* sendable, std::string_view moduleType,
                               int channel) {
  auto& inst = GetInstance();
  std::scoped_lock lock(inst.mutex);
  auto it = inst.componentMap.find(sendable);
  if (it == inst.componentMap.end() || !inst.components[it->getSecond() - 1]) {
    return;
  }
  inst.components[it->getSecond() - 1]->SetName(moduleType, channel);
}

void SendableRegistry::SetName(Sendable* sendable, std::string_view moduleType,
                               int moduleNumber, int channel) {
  auto& inst = GetInstance();
  std::scoped_lock lock(inst.mutex);
  auto it = inst.componentMap.find(sendable);
  if (it == inst.componentMap.end() || !inst.components[it->getSecond() - 1]) {
    return;
  }
  inst.components[it->getSecond() - 1]->SetName(moduleType, moduleNumber,
                                                channel);
}

void SendableRegistry::SetName(Sendable* sendable, std::string_view subsystem,
                               std::string_view name) {
  auto& inst = GetInstance();
  std::scoped_lock lock(inst.mutex);
  auto it = inst.componentMap.find(sendable);
  if (it == inst.componentMap.end() || !inst.components[it->getSecond() - 1]) {
    return;
  }
  auto& comp = *inst.components[it->getSecond() - 1];
  comp.name = name;
  comp.subsystem = subsystem;
}

std::string SendableRegistry::GetSubsystem(const Sendable* sendable) {
  auto& inst = GetInstance();
  std::scoped_lock lock(inst.mutex);
  auto it = inst.componentMap.find(sendable);
  if (it == inst.componentMap.end() || !inst.components[it->getSecond() - 1]) {
    return {};
  }
  return inst.components[it->getSecond() - 1]->subsystem;
}

void SendableRegistry::SetSubsystem(Sendable* sendable,
                                    std::string_view subsystem) {
  auto& inst = GetInstance();
  std::scoped_lock lock(inst.mutex);
  auto it = inst.componentMap.find(sendable);
  if (it == inst.componentMap.end() || !inst.components[it->getSecond() - 1]) {
    return;
  }
  inst.components[it->getSecond() - 1]->subsystem = subsystem;
}

int SendableRegistry::GetDataHandle() {
  auto& inst = GetInstance();
  std::scoped_lock lock(inst.mutex);
  return inst.nextDataHandle++;
}

std::shared_ptr<void> SendableRegistry::SetData(Sendable* sendable, int handle,
                                                std::shared_ptr<void> data) {
  auto& inst = GetInstance();
  assert(handle >= 0);
  std::scoped_lock lock(inst.mutex);
  auto it = inst.componentMap.find(sendable);
  if (it == inst.componentMap.end() || !inst.components[it->getSecond() - 1]) {
    return nullptr;
  }
  auto& comp = *inst.components[it->getSecond() - 1];
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
  auto& inst = GetInstance();
  assert(handle >= 0);
  std::scoped_lock lock(inst.mutex);
  auto it = inst.componentMap.find(sendable);
  if (it == inst.componentMap.end() || !inst.components[it->getSecond() - 1]) {
    return nullptr;
  }
  auto& comp = *inst.components[it->getSecond() - 1];
  if (static_cast<size_t>(handle) >= comp.data.size()) {
    return nullptr;
  }
  return comp.data[handle];
}

void SendableRegistry::EnableLiveWindow(Sendable* sendable) {
  auto& inst = GetInstance();
  std::scoped_lock lock(inst.mutex);
  auto it = inst.componentMap.find(sendable);
  if (it == inst.componentMap.end() || !inst.components[it->getSecond() - 1]) {
    return;
  }
  inst.components[it->getSecond() - 1]->liveWindow = true;
}

void SendableRegistry::DisableLiveWindow(Sendable* sendable) {
  auto& inst = GetInstance();
  std::scoped_lock lock(inst.mutex);
  auto it = inst.componentMap.find(sendable);
  if (it == inst.componentMap.end() || !inst.components[it->getSecond() - 1]) {
    return;
  }
  inst.components[it->getSecond() - 1]->liveWindow = false;
}

SendableRegistry::UID SendableRegistry::GetUniqueId(Sendable* sendable) {
  auto& inst = GetInstance();
  std::scoped_lock lock(inst.mutex);
  UID uid;
  auto& comp = inst.GetOrAdd(sendable, &uid);
  comp.sendable = sendable;
  return uid;
}

Sendable* SendableRegistry::GetSendable(UID uid) {
  auto& inst = GetInstance();
  if (uid == 0) {
    return nullptr;
  }
  std::scoped_lock lock(inst.mutex);
  if ((uid - 1) >= inst.components.size() || !inst.components[uid - 1]) {
    return nullptr;
  }
  return inst.components[uid - 1]->sendable;
}

void SendableRegistry::Publish(UID sendableUid,
                               std::unique_ptr<SendableBuilder> builder) {
  auto& inst = GetInstance();
  std::scoped_lock lock(inst.mutex);
  if (sendableUid == 0 || (sendableUid - 1) >= inst.components.size() ||
      !inst.components[sendableUid - 1]) {
    return;
  }
  auto& comp = *inst.components[sendableUid - 1];
  comp.builder = std::move(builder);  // clear any current builder
  comp.sendable->InitSendable(*comp.builder);
  comp.builder->Update();
}

void SendableRegistry::Update(UID sendableUid) {
  auto& inst = GetInstance();
  if (sendableUid == 0) {
    return;
  }
  std::scoped_lock lock(inst.mutex);
  if ((sendableUid - 1) >= inst.components.size() ||
      !inst.components[sendableUid - 1]) {
    return;
  }
  if (inst.components[sendableUid - 1]->builder) {
    inst.components[sendableUid - 1]->builder->Update();
  }
}

void SendableRegistry::ForeachLiveWindow(
    int dataHandle, wpi::function_ref<void(CallbackData& data)> callback) {
  auto& inst = GetInstance();
  assert(dataHandle >= 0);
  std::scoped_lock lock(inst.mutex);
  wpi::SmallVector<Component*, 128> components;
  for (auto&& comp : inst.components) {
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
