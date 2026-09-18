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
    struct ParentLink {
      TunableInfoImpl* parent;
      std::string path;
    };

    struct ChildLink {
      TunableInfoImpl* child;
      std::string path;
      std::string name;
      bool ownedMember;
    };

    uint32_t uid;
    detail::TunableBase* tunable;
    std::optional<TunableConfig> config;
    detail::TunableTypeValue type;
    uint64_t tuneRevision = 0;
    std::vector<ParentLink> parents;
    std::vector<ChildLink> children;
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

struct ComplexParentMatch {
  Instance::TunableInfoImpl* parent;
  std::string name;
};

static std::optional<ComplexParentMatch> FindNearestComplexParentLocked(
    Instance& inst, uint32_t childUid, std::string_view path) {
  size_t bestPrefixSize = 0;
  uint32_t bestParentUid = 0;
  for (auto&& entry : inst.complexUidByPath) {
    auto&& complexPath = entry.first;
    uint32_t parentUid = entry.second;
    if (parentUid == childUid) {
      continue;
    }
    std::string childPrefix = detail::GetChildTablePath(complexPath);
    if (wpi::util::starts_with(path, childPrefix) &&
        childPrefix.size() > bestPrefixSize) {
      bestPrefixSize = childPrefix.size();
      bestParentUid = parentUid;
    }
  }

  if (bestParentUid == 0) {
    return std::nullopt;
  }

  auto parentIt = inst.tunables.find(bestParentUid);
  if (parentIt == inst.tunables.end()) {
    return std::nullopt;
  }

  return ComplexParentMatch{
      parentIt->second.get(),
      detail::NormalizeChildName(path.substr(bestPrefixSize))};
}

static std::string GetLeafChildName(std::string_view path) {
  size_t slash = path.find_last_of('/');
  if (slash != std::string_view::npos) {
    path.remove_prefix(slash + 1);
  }
  return detail::NormalizeChildName(path);
}

static void LinkComplexParentLocked(Instance::TunableInfoImpl& parent,
                                    Instance::TunableInfoImpl& child,
                                    std::string_view path, std::string name,
                                    bool ownedMember) {
  std::string pathString{path};
  if (std::find_if(child.parents.begin(), child.parents.end(),
                   [&](const auto& link) {
                     return link.parent == &parent && link.path == pathString;
                   }) == child.parents.end()) {
    child.parents.emplace_back(
        Instance::TunableInfoImpl::ParentLink{&parent, pathString});
  }

  auto childLink = std::find_if(
      parent.children.begin(), parent.children.end(), [&](const auto& link) {
        return link.child == &child && link.path == pathString;
      });
  if (childLink != parent.children.end()) {
    childLink->ownedMember = childLink->ownedMember || ownedMember;
    childLink->name = std::move(name);
  } else {
    parent.children.emplace_back(Instance::TunableInfoImpl::ChildLink{
        &child, std::move(pathString), std::move(name), ownedMember});
  }
}

static void LinkComplexParentLocked(Instance& inst, uint32_t childUid,
                                    std::string_view path, bool ownedMember) {
  auto childIt = inst.tunables.find(childUid);
  if (childIt == inst.tunables.end()) {
    return;
  }

  auto parent = FindNearestComplexParentLocked(inst, childUid, path);
  if (!parent) {
    return;
  }

  LinkComplexParentLocked(*parent->parent, *childIt->second, path,
                          std::move(parent->name), ownedMember);
}

static void UnlinkComplexParentPathLocked(Instance& inst, uint32_t childUid,
                                          std::string_view path) {
  auto childIt = inst.tunables.find(childUid);
  if (childIt == inst.tunables.end()) {
    return;
  }

  auto& child = *childIt->second;
  std::string pathString{path};
  for (auto parentIt = child.parents.begin();
       parentIt != child.parents.end();) {
    if (parentIt->path != pathString) {
      ++parentIt;
      continue;
    }
    auto* parent = parentIt->parent;
    std::erase_if(parent->children, [&](const auto& link) {
      return link.child == &child && link.path == pathString;
    });
    parentIt = child.parents.erase(parentIt);
  }
}

static void UnlinkAllComplexRelationsLocked(
    Instance::TunableInfoImpl& tunable) {
  for (auto&& parentLink : tunable.parents) {
    auto* parent = parentLink.parent;
    std::erase_if(parent->children,
                  [&](const auto& link) { return link.child == &tunable; });
  }
  tunable.parents.clear();

  for (auto&& childLink : tunable.children) {
    auto* child = childLink.child;
    std::erase_if(child->parents,
                  [&](const auto& link) { return link.parent == &tunable; });
  }
  tunable.children.clear();
}

struct ComplexParentPathState {
  uint32_t childUid;
  std::string path;
  std::vector<Instance::TunableInfoImpl::ParentLink> parents;
  std::vector<std::pair<Instance::TunableInfoImpl*,
                        Instance::TunableInfoImpl::ChildLink>>
      parentChildren;
};

static ComplexParentPathState SnapshotComplexParentPathLocked(
    Instance& inst, uint32_t childUid, std::string_view path) {
  ComplexParentPathState state{childUid, std::string{path}, {}, {}};
  auto childIt = inst.tunables.find(childUid);
  if (childIt == inst.tunables.end()) {
    return state;
  }

  auto& child = *childIt->second;
  for (auto&& parentLink : child.parents) {
    if (parentLink.path != state.path) {
      continue;
    }

    state.parents.emplace_back(parentLink);
    auto childLink =
        std::find_if(parentLink.parent->children.begin(),
                     parentLink.parent->children.end(), [&](const auto& link) {
                       return link.child == &child && link.path == state.path;
                     });
    if (childLink != parentLink.parent->children.end()) {
      state.parentChildren.emplace_back(parentLink.parent, *childLink);
    }
  }
  return state;
}

static void RestoreComplexParentPathLocked(
    Instance& inst, const ComplexParentPathState& state) {
  UnlinkComplexParentPathLocked(inst, state.childUid, state.path);

  auto childIt = inst.tunables.find(state.childUid);
  if (childIt == inst.tunables.end()) {
    return;
  }

  auto& child = *childIt->second;
  for (auto&& parentLink : state.parents) {
    if (std::find_if(child.parents.begin(), child.parents.end(),
                     [&](const auto& link) {
                       return link.parent == parentLink.parent &&
                              link.path == parentLink.path;
                     }) == child.parents.end()) {
      child.parents.emplace_back(parentLink);
    }
  }

  for (auto&& [parent, childLink] : state.parentChildren) {
    auto restoredChildLink = childLink;
    restoredChildLink.child = &child;
    if (std::find_if(parent->children.begin(), parent->children.end(),
                     [&](const auto& link) {
                       return link.child == &child &&
                              link.path == restoredChildLink.path;
                     }) == parent->children.end()) {
      parent->children.emplace_back(std::move(restoredChildLink));
    }
  }
}

static void RestoreComplexParentPathsLocked(
    Instance& inst, const std::vector<ComplexParentPathState>& states) {
  for (auto it = states.rbegin(); it != states.rend(); ++it) {
    RestoreComplexParentPathLocked(inst, *it);
  }
}

static void LinkExistingComplexDescendantsLocked(
    Instance& inst, std::string_view path,
    std::vector<ComplexParentPathState>* previousLinks = nullptr) {
  std::string childPrefix = detail::GetChildTablePath(path);
  for (auto&& [childPath, childUid] : inst.complexChildUidByPath) {
    if (wpi::util::starts_with(childPath, childPrefix)) {
      if (previousLinks) {
        previousLinks->emplace_back(
            SnapshotComplexParentPathLocked(inst, childUid, childPath));
      }
      LinkComplexParentLocked(inst, childUid, childPath, false);
    }
  }
  for (auto&& [complexPath, childUid] : inst.complexUidByPath) {
    if (complexPath != path &&
        wpi::util::starts_with(complexPath, childPrefix)) {
      if (previousLinks) {
        previousLinks->emplace_back(
            SnapshotComplexParentPathLocked(inst, childUid, complexPath));
      }
      LinkComplexParentLocked(inst, childUid, complexPath, false);
    }
  }
}

struct ComplexPathState {
  std::string path;
  uint32_t uid;
  bool addedPath;
  std::vector<ComplexParentPathState> descendantLinks;
};

static ComplexPathState AddComplexPath(uint32_t uid, std::string_view path) {
  Instance& inst = GetInstance();
  std::scoped_lock lock{inst.tunablesMutex};
  std::string pathString{path};
  if (inst.complexUidByPath.contains(pathString)) {
    return {std::move(pathString), uid, false, {}};
  }

  LinkComplexParentLocked(inst, uid, pathString, false);
  inst.complexUidByPath[pathString] = uid;
  inst.complexPaths[uid].emplace_back(pathString);
  std::vector<ComplexParentPathState> descendantLinks;
  LinkExistingComplexDescendantsLocked(inst, pathString, &descendantLinks);
  return {std::move(pathString), uid, true, std::move(descendantLinks)};
}

static void RemoveComplexPath(uint32_t uid, std::string_view path) {
  Instance& inst = GetInstance();
  std::scoped_lock lock{inst.tunablesMutex};
  std::string pathString{path};
  UnlinkComplexParentPathLocked(inst, uid, pathString);
  if (auto pathIt = inst.complexUidByPath.find(pathString);
      pathIt != inst.complexUidByPath.end() && pathIt->second == uid) {
    inst.complexUidByPath.erase(pathIt);
  }
  auto pathsIt = inst.complexPaths.find(uid);
  if (pathsIt == inst.complexPaths.end()) {
    return;
  }
  std::erase(pathsIt->second, pathString);
  if (pathsIt->second.empty()) {
    inst.complexPaths.erase(pathsIt);
  }
}

static void RestoreComplexPath(const ComplexPathState& state) {
  Instance& inst = GetInstance();
  RemoveComplexPath(state.uid, state.path);
  std::scoped_lock lock{inst.tunablesMutex};
  RestoreComplexParentPathsLocked(inst, state.descendantLinks);
}

struct ComplexChildPathState {
  std::string path;
  uint32_t uid;
  std::optional<uint32_t> previousUid;
};

static ComplexChildPathState AddComplexChildPath(uint32_t uid,
                                                 std::string_view path) {
  Instance& inst = GetInstance();
  std::scoped_lock lock{inst.tunablesMutex};
  std::string pathString{path};
  std::optional<uint32_t> previousUid;
  if (auto pathIt = inst.complexChildUidByPath.find(pathString);
      pathIt != inst.complexChildUidByPath.end()) {
    previousUid = pathIt->second;
    if (*previousUid != uid) {
      UnlinkComplexParentPathLocked(inst, *previousUid, pathString);
    }
  }
  inst.complexChildUidByPath[pathString] = uid;
  LinkComplexParentLocked(inst, uid, pathString, false);
  return {std::move(pathString), uid, previousUid};
}

static void RestoreComplexChildPath(const ComplexChildPathState& state) {
  Instance& inst = GetInstance();
  std::scoped_lock lock{inst.tunablesMutex};
  auto pathIt = inst.complexChildUidByPath.find(state.path);
  if (pathIt == inst.complexChildUidByPath.end() ||
      pathIt->second != state.uid) {
    return;
  }
  UnlinkComplexParentPathLocked(inst, state.uid, state.path);
  if (state.previousUid) {
    pathIt->second = *state.previousUid;
    LinkComplexParentLocked(inst, *state.previousUid, state.path, false);
  } else {
    inst.complexChildUidByPath.erase(pathIt);
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
    UnlinkComplexParentPathLocked(inst, uid, complexPath);
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
      std::string childPath = it->first;
      uids.emplace_back(it->second);
      UnlinkComplexParentPathLocked(inst, it->second, childPath);
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

uint64_t TunableRegistry::GetTuneRevision(const detail::TunableBase& tunable) {
  if (!detail::TunableBase::IsRegisteredUid(tunable.m_uid)) {
    return 0;
  }

  Instance& inst = GetInstance();
  std::scoped_lock lock{inst.tunablesMutex};
  auto it = inst.tunables.find(tunable.m_uid & detail::TunableBase::UID_MASK);
  if (it == inst.tunables.end()) {
    return 0;
  }
  return it->second->tuneRevision;
}

uint64_t TunableRegistry::GetTuneRevision(const ComplexTunable& tunable) {
  return GetTuneRevision(static_cast<const detail::TunableBase&>(tunable));
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
      std::optional<ComplexPathState> complexPathState;
      std::optional<ComplexChildPathState> childPathState;
      if (type == detail::TunableTypeValue::COMPLEX) {
        complexPathState = AddComplexPath(uid, normalizedPath);
      } else {
        childPathState = AddComplexChildPath(uid, normalizedPath);
      }
      if (!backend->Publish(normalizedPath, uid, tunable, config, type)) {
        if (type == detail::TunableTypeValue::COMPLEX) {
          if (complexPathState->addedPath) {
            RestoreComplexPath(*complexPathState);
          }
        } else {
          RestoreComplexChildPath(*childPathState);
        }
        return false;
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
  std::string normalizedBuf;
  std::string_view normalizedPath = detail::NormalizeName(path, normalizedBuf);
  TunableTable table{detail::GetChildTablePath(normalizedPath)};
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
          LinkComplexParentLocked(*parentIt->second, child, normalizedPath,
                                  GetLeafChildName(normalizedPath), true);
          config = &*child.config;
        }
      }

      auto memberPtr = member.get();
      if (!backend->Publish(normalizedPath, memberUid, *memberPtr, config,
                            type)) {
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

      AddComplexChildPath(memberUid, normalizedPath);
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
      if (child.name == childName) {
        auto* tunable = child.child->tunable;
        if (std::find(changedTunables.begin(), changedTunables.end(),
                      tunable) == changedTunables.end()) {
          changedTunables.emplace_back(tunable);
        }
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
      if (std::find(uidsToErase.begin(), uidsToErase.end(), curUid) !=
          uidsToErase.end()) {
        return;
      }
      auto it = inst.tunables.find(curUid);
      if (it == inst.tunables.end()) {
        return;
      }
      for (auto&& child : it->second->children) {
        if (child.ownedMember) {
          self(self, child.child->uid);
        }
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
      std::vector<std::string> removedComplexPaths;
      if (auto pathsIt = inst.complexPaths.find(eraseUid);
          pathsIt != inst.complexPaths.end()) {
        for (auto&& path : pathsIt->second) {
          UnlinkComplexParentPathLocked(inst, eraseUid, path);
          inst.complexUidByPath.erase(path);
          removedComplexPaths.emplace_back(path);
        }
        inst.complexPaths.erase(pathsIt);
      }
      for (auto childIt = inst.complexChildUidByPath.begin();
           childIt != inst.complexChildUidByPath.end();) {
        if (childIt->second == eraseUid) {
          std::string childPath = childIt->first;
          UnlinkComplexParentPathLocked(inst, eraseUid, childPath);
          childIt = inst.complexChildUidByPath.erase(childIt);
        } else {
          ++childIt;
        }
      }
      auto it = inst.tunables.find(eraseUid);
      if (it == inst.tunables.end()) {
        for (auto&& path : removedComplexPaths) {
          LinkExistingComplexDescendantsLocked(inst, path);
        }
        continue;
      }
      auto& info = *it->second;
      UnlinkAllComplexRelationsLocked(info);
      for (auto&& path : removedComplexPaths) {
        LinkExistingComplexDescendantsLocked(inst, path);
      }
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
    if (it->second->type == detail::TunableTypeValue::COMPLEX) {
      for (auto&& child : it->second->children) {
        if (child.ownedMember && child.child->config &&
            child.child->config->parent) {
          child.child->config->parent = static_cast<ComplexTunable*>(tunable);
        }
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

void TunableRegistry::RecordTuneApplied(uint32_t uid) {
  uid &= detail::TunableBase::UID_MASK;
  Instance& inst = GetInstance();
  std::scoped_lock lock{inst.tunablesMutex};
  auto it = inst.tunables.find(uid);
  if (it == inst.tunables.end()) {
    return;
  }

  std::vector<Instance::TunableInfoImpl*> records;
  auto collect = [&](auto&& self, Instance::TunableInfoImpl* info) -> void {
    if (std::find(records.begin(), records.end(), info) != records.end()) {
      return;
    }
    records.emplace_back(info);
    for (auto&& parent : info->parents) {
      self(self, parent.parent);
    }
  };
  collect(collect, it->second.get());

  for (auto* info : records) {
    ++info->tuneRevision;
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
