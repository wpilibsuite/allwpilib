// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/tunables/TunableRegistry.hpp"

#include <algorithm>
#include <cstdint>
#include <format>
#include <iterator>
#include <memory>
#include <optional>
#include <print>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "wpi/tunables/ComplexTunable.hpp"
#include "wpi/tunables/TunableBackend.hpp"
#include "wpi/tunables/TunableConfig.hpp"
#include "wpi/tunables/TunableTable.hpp"
#include "wpi/tunables/detail/PathUtil.hpp"
#include "wpi/tunables/detail/TunableBase.hpp"
#include "wpi/tunables/detail/TunableMember.hpp"
#include "wpi/tunables/detail/TunableTypeValue.hpp"
#include "wpi/util/DenseMap.hpp"
#include "wpi/util/StringExtras.hpp"
#include "wpi/util/htrie_map.hpp"
#include "wpi/util/mutex.hpp"

using namespace wpi;
using namespace wpi::tunables;

static void DefaultReportWarning(std::string_view msg);

namespace {
struct Instance {
  util::mutex warningMutex;
  std::function<void(std::string_view)> reportWarning{DefaultReportWarning};

  util::mutex tunablesMutex;
  struct TunableInfoImpl {
    uint32_t uid;
    detail::TunableBase* tunable;
    std::optional<TunableConfig> config;
    detail::TunableTypeValue type;
    std::string name;
    TunableInfoImpl* parent = nullptr;
    std::vector<TunableInfoImpl*> children;
    std::unique_ptr<detail::TunableMemberBase> member;
  };
  wpi::util::DenseMap<uint32_t, std::unique_ptr<TunableInfoImpl>> tunables;
  struct UidInfo {
    uint32_t lastUid = 0;
    std::vector<uint32_t> freeUids;
  } uidInfo[static_cast<uint32_t>(detail::TunableTypeValue::MEMBER_COMPLEX) +
            1];
  wpi::util::DenseMap<uint32_t, std::vector<std::string>> complexPaths;
  std::unordered_map<std::string, uint32_t> complexUidByPath;
  std::unordered_map<std::string, uint32_t> complexChildUidByPath;

  util::recursive_mutex updateMutex;
  util::recursive_mutex backendsMutex;
  util::htrie_map<char, std::shared_ptr<TunableBackend>> backends;
  std::vector<std::shared_ptr<TunableBackend>> backendSnapshot;
  std::function<void()> preUpdateCallback;
  std::vector<uint32_t> pendingChangedResets;
  std::vector<std::function<void()>> pendingUpdateCallbacks;
  int updateDepth = 0;
};

class NoopTunableBackend : public TunableBackend {
 public:
  bool Publish(std::string_view, uint32_t, detail::TunableBase&,
               const TunableConfig*, detail::TunableTypeValue) override {
    return false;
  }

  void MarkDirty(uint32_t) override {}

  void Remove(std::string_view) override {}

  std::vector<TunableBackend::PublishedTunable> RemovePrefix(
      std::string_view) override {
    return {};
  }

  void UnregisterTunable(uint32_t) override {}

  void Update() override {}
};
}  // namespace

static Instance& GetInstance() {
  static Instance inst;
  return inst;
}

static std::shared_ptr<TunableBackend> GetMissingBackend() {
  static auto backend = std::make_shared<NoopTunableBackend>();
  return backend;
}

static void UpdateBackendSnapshot(Instance& inst) {
  inst.backendSnapshot.clear();
  for (auto backend : inst.backends) {
    auto ptr = backend.get();
    if (std::find_if(inst.backendSnapshot.begin(), inst.backendSnapshot.end(),
                     [ptr](auto&& existing) {
                       return existing.get() == ptr;
                     }) == inst.backendSnapshot.end()) {
      inst.backendSnapshot.emplace_back(backend);
    }
  }
}

static void DefaultReportWarning(std::string_view msg) {
  // TODO: do something smarter here
  std::print(stderr, "Tunable warning: {}\n", msg);
}

static std::shared_ptr<TunableBackend> GetBackendForNormalizedPath(
    Instance& inst, std::string_view path) {
  for (;;) {
    auto backendIt = inst.backends.find(path);
    if (backendIt != inst.backends.end()) {
      return backendIt.value();
    }

    size_t slash = path.find_last_of('/');
    if (slash == std::string_view::npos || slash == 0) {
      break;
    }
    path = path.substr(0, slash);
  }

  auto rootBackendIt = inst.backends.find("/");
  if (rootBackendIt != inst.backends.end()) {
    return rootBackendIt.value();
  }
  auto defaultBackendIt = inst.backends.find("");
  if (defaultBackendIt != inst.backends.end()) {
    return defaultBackendIt.value();
  }
  return nullptr;
}

static void AddComplexPath(uint32_t uid, std::string_view path) {
  Instance& inst = GetInstance();
  std::scoped_lock lock{inst.tunablesMutex};
  if (inst.complexUidByPath.contains(std::string{path})) {
    return;
  }
  inst.complexUidByPath[std::string{path}] = uid;
  inst.complexPaths[uid].emplace_back(path);
}

static void AddComplexChildPath(uint32_t uid, std::string_view path) {
  Instance& inst = GetInstance();
  std::scoped_lock lock{inst.tunablesMutex};
  size_t bestPrefixSize = 0;
  for (auto&& entry : inst.complexUidByPath) {
    auto&& complexPath = entry.first;
    std::string childPrefix = detail::GetChildTablePath(complexPath);
    if (wpi::util::starts_with(path, childPrefix) &&
        childPrefix.size() > bestPrefixSize) {
      bestPrefixSize = childPrefix.size();
    }
  }
  if (bestPrefixSize != 0) {
    inst.complexChildUidByPath[std::string{path}] = uid;
  }
}

static void RemoveComplexPaths(std::string_view path) {
  Instance& inst = GetInstance();
  std::scoped_lock lock{inst.tunablesMutex};

  std::vector<std::pair<std::string, uint32_t>> paths;
  for (auto&& [complexPath, uid] : inst.complexUidByPath) {
    if (detail::IsPathOrDescendant(complexPath, path)) {
      paths.emplace_back(complexPath, uid);
    }
  }
  for (auto&& [complexPath, uid] : paths) {
    inst.complexUidByPath.erase(complexPath);
    auto pathsIt = inst.complexPaths.find(uid);
    if (pathsIt == inst.complexPaths.end()) {
      continue;
    }
    std::erase(pathsIt->second, complexPath);
    if (pathsIt->second.empty()) {
      inst.complexPaths.erase(pathsIt);
    }
  }
}

static std::vector<uint32_t> RemoveComplexChildPaths(std::string_view path) {
  Instance& inst = GetInstance();
  std::scoped_lock lock{inst.tunablesMutex};
  std::vector<uint32_t> uids;
  for (auto it = inst.complexChildUidByPath.begin();
       it != inst.complexChildUidByPath.end();) {
    if (detail::IsPathOrDescendant(it->first, path)) {
      uids.emplace_back(it->second);
      it = inst.complexChildUidByPath.erase(it);
    } else {
      ++it;
    }
  }
  return uids;
}

static bool HasComplexChildPath(Instance& inst, uint32_t uid) {
  return std::find_if(inst.complexChildUidByPath.begin(),
                      inst.complexChildUidByPath.end(), [uid](auto&& entry) {
                        return entry.second == uid;
                      }) != inst.complexChildUidByPath.end();
}

static void UnregisterUnpublishedMemberTunables(std::vector<uint32_t> uids) {
  if (uids.empty()) {
    return;
  }

  std::sort(uids.begin(), uids.end());
  uids.erase(std::unique(uids.begin(), uids.end()), uids.end());

  std::vector<uint32_t> uidsToUnregister;
  Instance& inst = GetInstance();
  {
    std::scoped_lock lock{inst.tunablesMutex};
    for (auto uid : uids) {
      auto it = inst.tunables.find(uid);
      if (it != inst.tunables.end() && it->second->member &&
          !HasComplexChildPath(inst, uid)) {
        uidsToUnregister.emplace_back(uid);
      }
    }
  }

  for (auto uid : uidsToUnregister) {
    TunableRegistry::UnregisterTunable(uid);
  }
}

static std::vector<std::string> GetComplexPaths(uint32_t uid) {
  Instance& inst = GetInstance();
  std::scoped_lock lock{inst.tunablesMutex};
  auto pathsIt = inst.complexPaths.find(uid);
  if (pathsIt == inst.complexPaths.end()) {
    return {};
  }
  return pathsIt->second;
}

static std::vector<uint32_t> GetComplexTunableUids() {
  Instance& inst = GetInstance();
  std::scoped_lock lock{inst.tunablesMutex};
  std::vector<uint32_t> uids;
  for (auto&& [uid, paths] : inst.complexPaths) {
    if (paths.empty()) {
      continue;
    }
    auto it = inst.tunables.find(uid);
    if (it != inst.tunables.end() &&
        it->second->type == detail::TunableTypeValue::COMPLEX) {
      uids.emplace_back(uid);
    }
  }
  return uids;
}

static void UpdateComplexTunables() {
  for (auto uid : GetComplexTunableUids()) {
    auto info = TunableRegistry::GetTunable(uid);
    if (info && info.type == detail::TunableTypeValue::COMPLEX) {
      static_cast<ComplexTunable*>(info.tunable)->UpdateTunable();
    }
  }
}

static std::string GetChildName(uint32_t parentUid, std::string_view path) {
  size_t bestPrefixSize = 0;
  for (auto&& parentPath : GetComplexPaths(parentUid)) {
    std::string childPrefix = detail::GetChildTablePath(parentPath);
    if (wpi::util::starts_with(path, childPrefix) &&
        childPrefix.size() > bestPrefixSize) {
      bestPrefixSize = childPrefix.size();
    }
  }
  return detail::NormalizeChildName(path.substr(bestPrefixSize));
}

static void ResetChangedNow(uint32_t uid) {
  auto info = TunableRegistry::GetTunable(uid);
  if (info) {
    info.ResetChanged();
  }
}

static void ResetQueuedChanged(Instance& inst) {
  if (inst.pendingChangedResets.empty()) {
    return;
  }
  for (auto uid : inst.pendingChangedResets) {
    ResetChangedNow(uid);
  }
  inst.pendingChangedResets.clear();
}

static std::vector<std::function<void()>> FinishUpdate(Instance& inst) {
  --inst.updateDepth;
  if (inst.updateDepth != 0) {
    return {};
  }
  ResetQueuedChanged(inst);
  if (inst.pendingUpdateCallbacks.empty()) {
    return {};
  }
  auto callbacks = std::move(inst.pendingUpdateCallbacks);
  inst.pendingUpdateCallbacks.clear();
  return callbacks;
}

bool wpi::tunables::TunableRegistry::TunableInfo::IsChanged() const {
  return tunable && tunable->GetTunableChanged();
}

void wpi::tunables::TunableRegistry::TunableInfo::ResetChanged() {
  if (tunable) {
    tunable->ResetTunableChanged();
  }
}

void TunableRegistry::SetReportWarning(
    std::function<void(std::string_view)> func) {
  Instance& inst = GetInstance();
  std::scoped_lock lock{inst.warningMutex};
  if (func) {
    inst.reportWarning = std::move(func);
  } else {
    inst.reportWarning = DefaultReportWarning;
  }
}

std::function<void(std::string_view)> TunableRegistry::GetReportWarning() {
  Instance& inst = GetInstance();
  std::scoped_lock lock{inst.warningMutex};
  return inst.reportWarning;
}

void TunableRegistry::ReportWarning(std::string_view msg) {
  Instance& inst = GetInstance();
  std::function<void(std::string_view)> reportWarning;
  {
    std::scoped_lock lock{inst.warningMutex};
    reportWarning = inst.reportWarning;
  }
  reportWarning(msg);
}

void TunableRegistry::RegisterBackend(std::string_view prefix,
                                      std::shared_ptr<TunableBackend> backend) {
  assert(backend);
  std::string prefixBuf;
  prefix = detail::NormalizeBackendPrefix(prefix, prefixBuf);
  Instance& inst = GetInstance();
  std::scoped_lock updateLock{inst.updateMutex};
  std::vector<std::shared_ptr<TunableBackend>> retireBackends;
  {
    std::scoped_lock lock{inst.backendsMutex};
    std::vector<std::shared_ptr<TunableBackend>> oldBackends;
    for (auto oldBackend : inst.backends) {
      oldBackends.emplace_back(std::move(oldBackend));
    }

    auto newBackend = std::move(backend);
    inst.backends[prefix] = newBackend;
    UpdateBackendSnapshot(inst);

    // C++ complex tunables publish each member as an independent backend entry,
    // so migrations can republish every removed path directly.
    std::vector<TunableBackend::PublishedTunable> migrations;
    std::vector<std::shared_ptr<TunableBackend>> displacedBackends;
    for (auto oldBackend : oldBackends) {
      if (oldBackend != newBackend) {
        auto removed = oldBackend->RemovePrefix(prefix);
        migrations.insert(migrations.end(),
                          std::make_move_iterator(removed.begin()),
                          std::make_move_iterator(removed.end()));
        if (std::find(displacedBackends.begin(), displacedBackends.end(),
                      oldBackend) == displacedBackends.end()) {
          displacedBackends.emplace_back(std::move(oldBackend));
        }
      }
    }

    for (auto&& migration : migrations) {
      auto targetBackend = GetBackendForNormalizedPath(inst, migration.path);
      if (!targetBackend) {
        continue;
      }
      uint32_t uid = migration.uid & detail::TunableBase::UID_MASK;
      auto info = GetTunable(uid);
      if (info) {
        targetBackend->Publish(migration.path, uid, *info.tunable, info.config,
                               info.type);
      }
    }

    for (auto&& displacedBackend : displacedBackends) {
      if (std::find(inst.backendSnapshot.begin(), inst.backendSnapshot.end(),
                    displacedBackend) == inst.backendSnapshot.end()) {
        retireBackends.emplace_back(std::move(displacedBackend));
      }
    }
  }

  for (auto&& retiredBackend : retireBackends) {
    retiredBackend->Retire();
  }
}

std::shared_ptr<TunableBackend> TunableRegistry::GetBackend(
    std::string_view path) {
  std::string buf;
  path = detail::NormalizeName(path, buf);
  Instance& inst = GetInstance();
  {
    std::scoped_lock lock{inst.backendsMutex};
    auto backend = GetBackendForNormalizedPath(inst, path);
    if (backend) {
      return backend;
    }
  }
  ReportWarning(std::format("no backend for path '{}'", path));
  return GetMissingBackend();
}

bool TunableRegistry::PublishImpl(std::string_view path,
                                  detail::TunableBase& tunable) {
  std::string normalizedBuf;
  std::string_view normalizedPath = detail::NormalizeName(path, normalizedBuf);
  Instance& inst = GetInstance();
  bool missingBackend = false;
  {
    std::scoped_lock lock{inst.backendsMutex};
    auto backend = GetBackendForNormalizedPath(inst, normalizedPath);
    if (!backend) {
      missingBackend = true;
    } else {
      const TunableConfig* config;
      detail::TunableTypeValue type;
      if ((tunable.m_uid & detail::TunableBase::TYPE_FLAG) != 0) {
        config = nullptr;
        type = static_cast<detail::TunableTypeValue>(
            tunable.m_uid & detail::TunableBase::UID_MASK);
        // Ensure move tracking is set up
        tunable.m_uid = RegisterTunable(&tunable, nullptr, type);
      } else {
        auto info = GetTunable(tunable.m_uid);
        config = info.config;
        type = info.type;
      }
      uint32_t uid = tunable.m_uid & detail::TunableBase::UID_MASK;
      if (!backend->Publish(path, uid, tunable, config, type)) {
        return false;
      }
      if (type == detail::TunableTypeValue::COMPLEX) {
        AddComplexPath(uid, path);
      } else {
        AddComplexChildPath(uid, path);
      }
      return true;
    }
  }
  if (missingBackend) {
    ReportWarning(std::format("no backend for path '{}'", normalizedPath));
  }
  return false;
}

bool TunableRegistry::Publish(std::string_view path,
                              detail::TunableBase& tunable) {
  Instance& inst = GetInstance();
  std::scoped_lock updateLock{inst.updateMutex};
  return PublishImpl(path, tunable);
}

bool TunableRegistry::Publish(std::string_view path, ComplexTunable& tunable) {
  Instance& inst = GetInstance();
  std::scoped_lock updateLock{inst.updateMutex};
  if (!PublishImpl(path, static_cast<detail::TunableBase&>(tunable))) {
    return false;
  }
  TunableTable table{detail::GetChildTablePath(path)};
  tunable.PublishTunable(table);
  return true;
}

bool TunableRegistry::Publish(
    std::string_view path, ComplexTunable* tunable,
    std::unique_ptr<detail::TunableMemberBase> member) {
  assert(tunable);
  assert(member);

  Instance& inst = GetInstance();
  std::scoped_lock updateLock{inst.updateMutex};
  std::string normalizedBuf;
  std::string_view normalizedPath = detail::NormalizeName(path, normalizedBuf);
  bool missingBackend = false;
  {
    std::scoped_lock lock{inst.backendsMutex};
    auto backend = GetBackendForNormalizedPath(inst, normalizedPath);
    if (!backend) {
      missingBackend = true;
    } else {
      if ((tunable->m_uid & detail::TunableBase::TYPE_FLAG) != 0) {
        tunable->m_uid = RegisterTunable(tunable, nullptr,
                                         detail::TunableTypeValue::COMPLEX);
      }
      uint32_t parentUid = tunable->m_uid & detail::TunableBase::UID_MASK;

      TunableConfig memberConfig;
      const TunableConfig* config;
      detail::TunableTypeValue type;
      if ((member->m_uid & detail::TunableBase::TYPE_FLAG) != 0) {
        memberConfig.parent = tunable;
        config = &memberConfig;
        type = static_cast<detail::TunableTypeValue>(
            member->m_uid & detail::TunableBase::UID_MASK);
        member->m_uid = RegisterTunable(member.get(), config, type);
      } else {
        auto info = GetTunable(member->m_uid);
        config = info.config;
        type = info.type;
      }
      uint32_t memberUid = member->m_uid & detail::TunableBase::UID_MASK;

      std::string childName = GetChildName(parentUid, path);
      {
        std::scoped_lock lock{inst.tunablesMutex};
        auto parentIt = inst.tunables.find(parentUid);
        auto childIt = inst.tunables.find(memberUid);
        if (parentIt != inst.tunables.end() && childIt != inst.tunables.end()) {
          auto& child = *childIt->second;
          if (!child.config) {
            child.config = TunableConfig{};
          }
          child.config->parent = tunable;
          child.parent = parentIt->second.get();
          child.name = childName;
          parentIt->second->children.emplace_back(&child);
          config = &*child.config;
        }
      }

      auto memberPtr = member.get();
      if (!backend->Publish(path, memberUid, *memberPtr, config, type)) {
        UnregisterTunable(memberUid);
        return false;
      }

      {
        std::scoped_lock lock{inst.tunablesMutex};
        auto childIt = inst.tunables.find(memberUid);
        if (childIt != inst.tunables.end()) {
          childIt->second->member = std::move(member);
        }
      }

      AddComplexChildPath(memberUid, path);
      return true;
    }
  }
  if (missingBackend) {
    ReportWarning(std::format("no backend for path '{}'", normalizedPath));
  }
  return false;
}

void TunableRegistry::Remove(std::string_view path) {
  // Backends may have changed since publishing, so remove from all backends
  Instance& inst = GetInstance();
  std::scoped_lock updateLock{inst.updateMutex};
  {
    std::scoped_lock lock{inst.backendsMutex};
    std::string childPrefix = detail::GetChildTablePath(path);
    for (auto backend : inst.backends) {
      backend->Remove(path);
      backend->RemovePrefix(childPrefix);
    }
  }
  auto childUids = RemoveComplexChildPaths(path);
  RemoveComplexPaths(path);
  UnregisterUnpublishedMemberTunables(std::move(childUids));
}

void TunableRegistry::PublishChild(ComplexTunable& parent,
                                   std::string_view name,
                                   detail::TunableBase& tunable) {
  if ((parent.m_uid & detail::TunableBase::TYPE_FLAG) != 0) {
    return;
  }
  for (auto&& path :
       GetComplexPaths(parent.m_uid & detail::TunableBase::UID_MASK)) {
    TunableTable table{detail::GetChildTablePath(path)};
    table.Publish(name, tunable);
  }
}

void TunableRegistry::PublishChild(ComplexTunable& parent,
                                   std::string_view name,
                                   ComplexTunable& tunable) {
  if ((parent.m_uid & detail::TunableBase::TYPE_FLAG) != 0) {
    return;
  }
  for (auto&& path :
       GetComplexPaths(parent.m_uid & detail::TunableBase::UID_MASK)) {
    TunableTable table{detail::GetChildTablePath(path)};
    table.Publish(name, tunable);
  }
}

void TunableRegistry::RemoveChild(ComplexTunable& parent,
                                  std::string_view name) {
  if ((parent.m_uid & detail::TunableBase::TYPE_FLAG) != 0) {
    return;
  }
  for (auto&& path :
       GetComplexPaths(parent.m_uid & detail::TunableBase::UID_MASK)) {
    TunableTable table{detail::GetChildTablePath(path)};
    table.Remove(name);
  }
}

void TunableRegistry::SetChildChanged(ComplexTunable& parent,
                                      std::string_view name) {
  if ((parent.m_uid & detail::TunableBase::TYPE_FLAG) != 0) {
    return;
  }

  std::string childName = detail::NormalizeChildName(name);
  Instance& inst = GetInstance();
  std::vector<detail::TunableBase*> changedTunables;
  {
    std::scoped_lock lock{inst.tunablesMutex};
    auto parentIt =
        inst.tunables.find(parent.m_uid & detail::TunableBase::UID_MASK);
    if (parentIt == inst.tunables.end()) {
      return;
    }
    for (auto child : parentIt->second->children) {
      if (child->name == childName) {
        changedTunables.emplace_back(child->tunable);
      }
    }
  }
  for (auto tunable : changedTunables) {
    tunable->SetTunableChanged();
  }
}

TunableRegistry::TunableInfo TunableRegistry::GetTunable(uint32_t uid) {
  uid &= detail::TunableBase::UID_MASK;
  Instance& inst = GetInstance();
  std::scoped_lock lock{inst.tunablesMutex};
  auto it = inst.tunables.find(uid);
  if (it == inst.tunables.end()) {
    return {nullptr, nullptr, detail::TunableTypeValue::UNKNOWN};
  }
  return {it->second->tunable,
          it->second->config ? &*it->second->config : nullptr,
          it->second->type};
}

void TunableRegistry::Reset() {
  Instance& inst = GetInstance();
  std::scoped_lock updateLock{inst.updateMutex};
  std::vector<std::shared_ptr<TunableBackend>> backends;
  {
    std::scoped_lock lock{inst.backendsMutex};
    backends = inst.backendSnapshot;
    inst.backends.clear();
    inst.backendSnapshot.clear();
    inst.preUpdateCallback = nullptr;
    inst.pendingChangedResets.clear();
    inst.pendingUpdateCallbacks.clear();
    inst.updateDepth = 0;
  }
  {
    std::scoped_lock lock1{inst.tunablesMutex};
    for (auto& [uid, info] : inst.tunables) {
      info->tunable->m_uid = detail::TunableBase::TYPE_FLAG | (uid >> 24);
    }
    inst.tunables.clear();
    inst.complexPaths.clear();
    inst.complexUidByPath.clear();
    inst.complexChildUidByPath.clear();
    for (auto& uidInfo : inst.uidInfo) {
      uidInfo.lastUid = 0;
      uidInfo.freeUids.clear();
    }
  }

  for (auto&& backend : backends) {
    backend->RemovePrefix("");
    backend->Retire();
  }
}

uint32_t TunableRegistry::RegisterTunable(detail::TunableBase* tunable,
                                          const TunableConfig* config,
                                          detail::TunableTypeValue type) {
  Instance& inst = GetInstance();
  std::scoped_lock lock{inst.tunablesMutex};
  uint32_t uid;
  auto& uidInfo = inst.uidInfo[static_cast<uint32_t>(type)];
  if (!uidInfo.freeUids.empty()) {
    uid = uidInfo.freeUids.back();
    uidInfo.freeUids.pop_back();
  } else {
    uid = ++uidInfo.lastUid;
  }
  assert((uid & 0x3f000000) == 0);  // ensure type bits are clear
  uid |= static_cast<uint32_t>(type) << 24;
  inst.tunables[uid] = std::make_unique<Instance::TunableInfoImpl>(
      uid, tunable, config ? std::make_optional(*config) : std::nullopt, type);
  return uid;
}

void TunableRegistry::MoveTunableCallbackParent(ComplexTunable* oldParent,
                                                ComplexTunable* newParent) {
  if (oldParent == newParent) {
    return;
  }

  Instance& inst = GetInstance();
  std::scoped_lock lock{inst.tunablesMutex};
  for (auto&& entry : inst.tunables) {
    auto& info = entry.second;
    if (info->config && info->config->parent == oldParent) {
      info->config->parent = newParent;
    }
  }
}

void TunableRegistry::PrepareComplexMoveAssignment(
    detail::TunableBase& destination, detail::TunableBase& source) {
  if (&destination == &source ||
      (source.m_uid & detail::TunableBase::TYPE_FLAG) != 0 ||
      (destination.m_uid & detail::TunableBase::TYPE_FLAG) != 0) {
    return;
  }

  uint32_t destinationUid = destination.m_uid & detail::TunableBase::UID_MASK;
  std::vector<uint32_t> childUids;
  auto paths = GetComplexPaths(destinationUid);
  {
    Instance& inst = GetInstance();
    std::scoped_lock lock{inst.backendsMutex};
    for (auto&& path : paths) {
      std::string childPrefix = detail::GetChildTablePath(path);
      for (auto backend : inst.backends) {
        for (auto&& removed : backend->RemovePrefix(childPrefix)) {
          childUids.emplace_back(removed.uid);
        }
      }
    }
  }

  for (auto childUid : childUids) {
    UnregisterTunable(childUid);
  }
  UnregisterTunable(destinationUid);
}

void TunableRegistry::UnregisterTunable(uint32_t uid) {
  uid &= detail::TunableBase::UID_MASK;
  Instance& inst = GetInstance();
  std::vector<uint32_t> uidsToErase;
  {
    std::scoped_lock lock{inst.tunablesMutex};
    auto collect = [&](auto&& self, uint32_t curUid) -> void {
      auto it = inst.tunables.find(curUid);
      if (it == inst.tunables.end()) {
        return;
      }
      for (auto child : it->second->children) {
        self(self, child->uid);
      }
      uidsToErase.emplace_back(curUid);
    };
    collect(collect, uid);
  }

  {
    std::scoped_lock lock{inst.backendsMutex};
    for (auto backend : inst.backends) {
      for (auto eraseUid : uidsToErase) {
        backend->UnregisterTunable(eraseUid);
      }
    }
  }
  {
    std::scoped_lock lock{inst.tunablesMutex};
    for (auto eraseUid : uidsToErase) {
      if (auto pathsIt = inst.complexPaths.find(eraseUid);
          pathsIt != inst.complexPaths.end()) {
        for (auto&& path : pathsIt->second) {
          inst.complexUidByPath.erase(path);
        }
        inst.complexPaths.erase(pathsIt);
      }
      for (auto childIt = inst.complexChildUidByPath.begin();
           childIt != inst.complexChildUidByPath.end();) {
        if (childIt->second == eraseUid) {
          childIt = inst.complexChildUidByPath.erase(childIt);
        } else {
          ++childIt;
        }
      }
      auto it = inst.tunables.find(eraseUid);
      if (it == inst.tunables.end()) {
        continue;
      }
      auto& info = *it->second;
      if (info.parent) {
        std::erase(info.parent->children, &info);
      }
      info.children.clear();
      info.tunable->m_uid = detail::TunableBase::TYPE_FLAG | (eraseUid >> 24);
      auto& uidInfo = inst.uidInfo[eraseUid >> 24];
      uidInfo.freeUids.push_back(eraseUid & 0x00ffffff);
      inst.tunables.erase(it);
    }
  }
}

void TunableRegistry::MoveTunable(uint32_t uid, detail::TunableBase* tunable) {
  Instance& inst = GetInstance();
  std::scoped_lock lock{inst.tunablesMutex};
  auto it = inst.tunables.find(uid);
  if (it != inst.tunables.end()) {
    it->second->tunable = tunable;
    for (auto child : it->second->children) {
      if (child->config) {
        child->config->parent = static_cast<ComplexTunable*>(tunable);
      }
    }
  } else {
    ReportWarning(std::format(
        "attempted to move tunable with uid {} that is not registered", uid));
  }
}

void TunableRegistry::Update() {
  Instance& inst = GetInstance();
  std::scoped_lock updateLock{inst.updateMutex};

  std::function<void()> preUpdateCallback;
  {
    std::scoped_lock lock{inst.backendsMutex};
    if (inst.updateDepth == 0) {
      preUpdateCallback = inst.preUpdateCallback;
    }
  }
  if (preUpdateCallback) {
    preUpdateCallback();
  }

  std::vector<std::shared_ptr<TunableBackend>> backends;
  {
    std::scoped_lock lock{inst.backendsMutex};
    ++inst.updateDepth;
    backends = inst.backendSnapshot;
  }

  UpdateComplexTunables();
  for (auto&& backend : backends) {
    backend->Update();
  }

  std::vector<std::function<void()>> callbacks;
  {
    std::scoped_lock lock{inst.backendsMutex};
    callbacks = FinishUpdate(inst);
  }
  for (auto&& callback : callbacks) {
    callback();
  }
}

void TunableRegistry::NotifyChanged(uint32_t uid) {
  uid &= detail::TunableBase::UID_MASK;
  Instance& inst = GetInstance();
  std::scoped_lock lock{inst.backendsMutex};
  for (auto&& backend : inst.backendSnapshot) {
    backend->MarkDirty(uid);
  }
}

void TunableRegistry::ResetChangedAfterUpdate(uint32_t uid) {
  Instance& inst = GetInstance();
  std::scoped_lock lock{inst.backendsMutex};
  if (inst.updateDepth > 0) {
    inst.pendingChangedResets.emplace_back(uid);
  } else {
    ResetChangedNow(uid);
  }
}

void TunableRegistry::RunAfterUpdate(std::function<void()> callback) {
  Instance& inst = GetInstance();
  std::scoped_lock lock{inst.backendsMutex};
  if (inst.updateDepth > 0) {
    inst.pendingUpdateCallbacks.emplace_back(std::move(callback));
  } else {
    callback();
  }
}

wpi::util::recursive_mutex& TunableRegistry::GetUpdateMutex() {
  return GetInstance().updateMutex;
}

void wpi::tunables::detail::SetTunableRegistryPreUpdateCallback(
    std::function<void()> callback) {
  Instance& inst = GetInstance();
  std::scoped_lock lock{inst.backendsMutex};
  inst.preUpdateCallback = std::move(callback);
}
