// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/tunable/TunableRegistry.hpp"

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include <fmt/format.h>

#include "wpi/tunable/ComplexTunable.hpp"
#include "wpi/tunable/TunableBackend.hpp"
#include "wpi/tunable/TunableConfig.hpp"
#include "wpi/tunable/detail/TunableBase.hpp"
#include "wpi/tunable/detail/TunableTypeValue.hpp"
#include "wpi/util/DenseMap.hpp"
#include "wpi/util/StringExtras.hpp"
#include "wpi/util/htrie_map.hpp"
#include "wpi/util/mutex.hpp"

using namespace wpi;

static void DefaultReportWarning(std::string_view msg);

namespace {
struct Instance {
  util::mutex warningMutex;
  std::function<void(std::string_view)> reportWarning{DefaultReportWarning};

  util::mutex tunablesMutex;
  struct TunableInfoImpl {
    detail::TunableBase* tunable;
    std::optional<TunableConfig> config;
    detail::TunableTypeValue type;
    TunableInfoImpl* parent = nullptr;
    std::vector<TunableInfoImpl*> children;
  };
  wpi::util::DenseMap<uint32_t, std::unique_ptr<TunableInfoImpl>> tunables;
  struct UidInfo {
    uint32_t lastUid = 0;
    std::vector<uint32_t> freeUids;
  } uidInfo[static_cast<uint32_t>(detail::TunableTypeValue::MEMBER_COMPLEX) + 1];

  util::mutex backendsMutex;
  util::htrie_map<char, std::shared_ptr<TunableBackend>> backends;
};
}  // namespace

static Instance& GetInstance() {
  static Instance inst;
  return inst;
}

static void DefaultReportWarning(std::string_view msg) {
  // TODO: do something smarter here
  fmt::print(stderr, "Tunable warning: {}\n", msg);
}

bool wpi::TunableRegistry::TunableInfo::IsChanged() const {
  return tunable && tunable->GetTunableChanged();
}

void wpi::TunableRegistry::TunableInfo::ResetChanged() {
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
  std::scoped_lock lock{inst.warningMutex};
  inst.reportWarning(msg);
}

void TunableRegistry::RegisterBackend(std::string_view prefix,
                                      std::shared_ptr<TunableBackend> backend) {
  assert(backend);
  Instance& inst = GetInstance();
  std::scoped_lock lock{inst.backendsMutex};
  inst.backends[prefix] = std::move(backend);
}

std::shared_ptr<TunableBackend> TunableRegistry::GetBackend(
    std::string_view path) {
  Instance& inst = GetInstance();
  std::scoped_lock lock{inst.backendsMutex};
  auto backendIt = inst.backends.longest_prefix(path);
  if (backendIt == inst.backends.end()) {
    throw std::out_of_range(fmt::format("no backend for path '{}'", path));
  }
  return backendIt.value();
}

std::string_view TunableRegistry::NormalizeName(std::string_view path,
                                                std::string& buf) {
  // common case is a well formatted name, so check first
  if (util::starts_with(path, '/') && !util::contains(path, "//")) {
    return path;
  }
  buf.clear();
  buf.reserve(path.size() + 2);
  if (!util::starts_with(path, '/')) {
    buf.push_back('/');
  }
  char prevCh = '\0';
  for (auto ch : path) {
    if (ch != '/' || prevCh != '/') {
      buf.push_back(ch);
    }
    prevCh = ch;
  }
  return buf;
}

void TunableRegistry::Publish(std::string_view path,
                              detail::TunableBase& tunable) {
  const TunableConfig* config;
  detail::TunableTypeValue type;
  if ((tunable.m_uid & detail::TunableBase::TYPE_FLAG) != 0) {
    config = nullptr;
    type = static_cast<detail::TunableTypeValue>(tunable.m_uid &
                                                 detail::TunableBase::UID_MASK);
    // Ensure move tracking is set up
    tunable.m_uid = RegisterTunable(&tunable, nullptr, type);
  } else {
    auto info = GetTunable(tunable.m_uid);
    config = info.config;
    type = info.type;
  }
  GetBackend(path)->Publish(path, tunable.m_uid, tunable, config, type);
}

void TunableRegistry::Remove(std::string_view path) {
  // Backends may have changed since publishing, so remove from all backends
  Instance& inst = GetInstance();
  std::scoped_lock lock{inst.backendsMutex};
  for (auto backend : inst.backends) {
    backend->Remove(path);
  }
}

TunableRegistry::TunableInfo TunableRegistry::GetTunable(uint32_t uid) {
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
  {
    std::scoped_lock lock{inst.backendsMutex};
    inst.backends.clear();
  }
  {
    std::scoped_lock lock1{inst.tunablesMutex};
    for (auto& [uid, info] : inst.tunables) {
      info->tunable->m_uid = detail::TunableBase::TYPE_FLAG | (uid >> 24);
    }
    inst.tunables.clear();
    for (auto& uidInfo : inst.uidInfo) {
      uidInfo.lastUid = 0;
      uidInfo.freeUids.clear();
    }
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
      tunable, config ? std::make_optional(*config) : std::nullopt, type);
  return uid;
}

void TunableRegistry::UnregisterTunable(uint32_t uid) {
  Instance& inst = GetInstance();
  {
    std::scoped_lock lock{inst.backendsMutex};
    for (auto backend : inst.backends) {
      backend->UnregisterTunable(uid);
    }
  }
  {
    std::scoped_lock lock{inst.tunablesMutex};
    auto& uidInfo = inst.uidInfo[uid >> 24];
    uidInfo.freeUids.push_back(uid & 0x00ffffff);
    inst.tunables.erase(uid);
  }
}

void TunableRegistry::MoveTunable(uint32_t uid, detail::TunableBase* tunable) {
  Instance& inst = GetInstance();
  std::scoped_lock lock{inst.tunablesMutex};
  auto it = inst.tunables.find(uid);
  if (it != inst.tunables.end()) {
    it->second->tunable = tunable;
    for (auto child : it->second->children) {
      if (auto config = child->config) {
        config->parent = static_cast<ComplexTunable*>(tunable);
      }
    }
  } else {
    ReportWarning(fmt::format(
        "attempted to move tunable with uid {} that is not registered", uid));
  }
}

void TunableRegistry::Update() {
  Instance& inst = GetInstance();
  std::scoped_lock lock{inst.backendsMutex};
  for (auto backend : inst.backends) {
    backend->Update();
  }
}

wpi::util::mutex& TunableRegistry::GetUpdateMutex() {
  return GetInstance().backendsMutex;
}
