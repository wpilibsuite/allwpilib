// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/tunables/MockTunableBackend.hpp"

#include <format>
#include <mutex>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "wpi/tunables/TunableConfig.hpp"
#include "wpi/tunables/TunableRegistry.hpp"
#include "wpi/tunables/detail/PathUtil.hpp"
#include "wpi/tunables/detail/TunableTypeTraits.hpp"
#include "wpi/util/type_name.hpp"

using namespace wpi;
using namespace wpi::tunables;

static void NotifyOnTune(uint32_t uid, TunableRegistry::TunableInfo& info) {
  TunableRegistry::ResetChangedAfterUpdate(uid);
  if (auto config = info.config) {
    if (config->onTune) {
      TunableRegistry::RunAfterUpdate([uid] {
        auto info = TunableRegistry::GetTunable(uid);
        if (auto config = info.config) {
          if (auto&& onTune = config->onTune) {
            onTune(*info.tunable, config->parent);
          }
        }
      });
    }
  }
}

static void NotifyRemoteSet(TunableRegistry::TunableInfo& info) {
  if (auto config = info.config) {
    if (config->onRemoteSet) {
      config->onRemoteSet(*info.tunable, config->parent);
    }
  }
}

static bool IsMutable(TunableRegistry::TunableInfo& info) {
  return !info.config || info.config->isMutable;
}

MockTunableBackend::~MockTunableBackend() = default;

uint32_t MockTunableBackend::GetUidOrThrow(std::string_view path) const {
  std::scoped_lock lock{m_mutex};
  auto uidIt = m_tunables.find(path);
  if (uidIt == m_tunables.end()) {
    throw std::invalid_argument{std::format("No such tunable: {}", path)};
  }
  return uidIt->second;
}

template <detail::TunableValueType T>
T MockTunableBackend::GetValue(std::string_view path) const {
  std::string pathBuf;
  path = detail::NormalizeName(path, pathBuf);
  auto uid = GetUidOrThrow(path);
  auto info = TunableRegistry::GetTunable(uid);
  if (!info) {
    throw std::runtime_error{
        std::format("No registered tunable for path: {}", path)};
  }

  if (info.type != detail::GetTunableTypeValue<T, false>() &&
      info.type != detail::GetTunableTypeValue<T, true>()) {
    throw std::invalid_argument{
        std::format("Tunable at {} is not a Tunable<{}>", path,
                    wpi::util::GetTypeName<T>())};
  }

  if (auto v = detail::CastTunable<T, false>(info.tunable, info.type)) {
    return v->Get();
  }
  if (auto v = detail::CastTunable<T, true>(info.tunable, info.type)) {
    return v->Get(info.config->parent);
  }
  throw std::runtime_error{
      std::format("Tunable has unexpected type: {}", path)};
}

template bool MockTunableBackend::GetValue<bool>(std::string_view path) const;
template int32_t MockTunableBackend::GetValue<int32_t>(
    std::string_view path) const;
template int64_t MockTunableBackend::GetValue<int64_t>(
    std::string_view path) const;
template float MockTunableBackend::GetValue<float>(std::string_view path) const;
template double MockTunableBackend::GetValue<double>(
    std::string_view path) const;
template std::string MockTunableBackend::GetValue<std::string>(
    std::string_view path) const;
template std::vector<uint8_t>
MockTunableBackend::GetValue<std::vector<uint8_t>>(std::string_view path) const;
template std::vector<bool> MockTunableBackend::GetValue<std::vector<bool>>(
    std::string_view path) const;
template std::vector<int32_t>
MockTunableBackend::GetValue<std::vector<int32_t>>(std::string_view path) const;
template std::vector<int64_t>
MockTunableBackend::GetValue<std::vector<int64_t>>(std::string_view path) const;
template std::vector<float> MockTunableBackend::GetValue<std::vector<float>>(
    std::string_view path) const;
template std::vector<double> MockTunableBackend::GetValue<std::vector<double>>(
    std::string_view path) const;
template std::vector<std::string> MockTunableBackend::GetValue<
    std::vector<std::string>>(std::string_view path) const;

std::string MockTunableBackend::GetStructTypeName(std::string_view path) const {
  std::string pathBuf;
  path = detail::NormalizeName(path, pathBuf);
  auto uid = GetUidOrThrow(path);
  auto info = TunableRegistry::GetTunable(uid);
  if (!info) {
    throw std::runtime_error{
        std::format("No registered tunable for path: {}", path)};
  }

  if (auto v = detail::CastTunable<detail::TunableStructTag, false>(
          info.tunable, info.type)) {
    return v->GetStructTypeName();
  }
  if (auto v = detail::CastTunable<detail::TunableStructTag, true>(info.tunable,
                                                                   info.type)) {
    return v->GetStructTypeName();
  }
  throw std::invalid_argument{
      std::format("Tunable at {} is not a struct", path)};
}

std::vector<uint8_t> MockTunableBackend::GetStructData(
    std::string_view path) const {
  std::string pathBuf;
  path = detail::NormalizeName(path, pathBuf);
  auto uid = GetUidOrThrow(path);
  auto info = TunableRegistry::GetTunable(uid);
  if (!info) {
    throw std::runtime_error{
        std::format("No registered tunable for path: {}", path)};
  }

  if (auto v = detail::CastTunable<detail::TunableStructTag, false>(
          info.tunable, info.type)) {
    std::vector<uint8_t> data(v->GetStructSize());
    v->PackStruct(data);
    return data;
  }
  if (auto v = detail::CastTunable<detail::TunableStructTag, true>(info.tunable,
                                                                   info.type)) {
    std::vector<uint8_t> data(v->GetStructSize(info.config->parent));
    v->PackStruct(info.config->parent, data);
    return data;
  }
  throw std::invalid_argument{
      std::format("Tunable at {} is not a struct", path)};
}

std::string MockTunableBackend::GetProtobufTypeString(
    std::string_view path) const {
  std::string pathBuf;
  path = detail::NormalizeName(path, pathBuf);
  auto uid = GetUidOrThrow(path);
  auto info = TunableRegistry::GetTunable(uid);
  if (!info) {
    throw std::runtime_error{
        std::format("No registered tunable for path: {}", path)};
  }

  if (auto v = detail::CastTunable<detail::TunableProtobufTag, false>(
          info.tunable, info.type)) {
    return v->GetProtobufTypeString();
  }
  if (auto v = detail::CastTunable<detail::TunableProtobufTag, true>(
          info.tunable, info.type)) {
    return v->GetProtobufTypeString();
  }
  throw std::invalid_argument{
      std::format("Tunable at {} is not a protobuf", path)};
}

std::vector<uint8_t> MockTunableBackend::GetProtobufData(
    std::string_view path) const {
  std::string pathBuf;
  path = detail::NormalizeName(path, pathBuf);
  auto uid = GetUidOrThrow(path);
  auto info = TunableRegistry::GetTunable(uid);
  if (!info) {
    throw std::runtime_error{
        std::format("No registered tunable for path: {}", path)};
  }

  std::vector<uint8_t> data;
  if (auto v = detail::CastTunable<detail::TunableProtobufTag, false>(
          info.tunable, info.type)) {
    if (!v->PackProtobuf(data)) {
      throw std::runtime_error{
          std::format("Failed to pack protobuf tunable: {}", path)};
    }
    return data;
  }
  if (auto v = detail::CastTunable<detail::TunableProtobufTag, true>(
          info.tunable, info.type)) {
    if (!v->PackProtobuf(info.config->parent, data)) {
      throw std::runtime_error{
          std::format("Failed to pack protobuf tunable: {}", path)};
    }
    return data;
  }
  throw std::invalid_argument{
      std::format("Tunable at {} is not a protobuf", path)};
}

template <typename T, typename U>
static T MakeCopy(U&& value) {
  return T{value};
}

template <typename T, typename U>
static T MakeCopy(std::vector<U>&& value) {
  return T{value.begin(), value.end()};
}

template <typename T, typename U>
static T MakeCopy(std::span<U>&& value) {
  return T{value.begin(), value.end()};
}

template <typename T, typename U>
void MockTunableBackend::SetValue(std::string_view path, U value) {
  std::string pathBuf;
  path = detail::NormalizeName(path, pathBuf);
  uint32_t uid = GetUidOrThrow(path);
  auto info = TunableRegistry::GetTunable(uid);
  if (info.type != detail::GetTunableTypeValue<T, false>() &&
      info.type != detail::GetTunableTypeValue<T, true>()) {
    throw std::invalid_argument(std::format("Tunable is not a {}: {}",
                                            wpi::util::GetTypeName<T>(), path));
  }

  std::scoped_lock lock{m_mutex};
  m_actions.emplace_back(
      std::string{path}, uid,
      [value = MakeCopy<T>(std::move(value))](uint32_t uid) mutable {
        if (auto info = TunableRegistry::GetTunable(uid)) {
          if (!IsMutable(info)) {
            return;
          }
          if (auto v = detail::CastTunable<T, false>(info.tunable, info.type)) {
            auto copiedValue = value;
            v->Set(std::move(copiedValue));
          } else if (auto v = detail::CastTunable<T, true>(info.tunable,
                                                           info.type)) {
            auto copiedValue = value;
            v->Set(info.config->parent, std::move(copiedValue));
          }
          NotifyRemoteSet(info);
          NotifyOnTune(uid, info);
        }
      });
}

template void MockTunableBackend::SetValue<bool>(std::string_view path,
                                                 bool value);
template void MockTunableBackend::SetValue<int32_t>(std::string_view path,
                                                    int32_t value);
template void MockTunableBackend::SetValue<int64_t>(std::string_view path,
                                                    int64_t value);
template void MockTunableBackend::SetValue<float>(std::string_view path,
                                                  float value);
template void MockTunableBackend::SetValue<double>(std::string_view path,
                                                   double value);
template void MockTunableBackend::SetValue<std::string>(std::string_view path,
                                                        std::string_view value);
template void MockTunableBackend::SetValue<std::vector<uint8_t>>(
    std::string_view path, std::span<const uint8_t> value);
template void MockTunableBackend::SetValue<std::vector<bool>>(
    std::string_view path, std::span<const bool> value);
template void MockTunableBackend::SetValue<std::vector<bool>>(
    std::string_view path, std::vector<bool> value);
template void MockTunableBackend::SetValue<std::vector<int32_t>>(
    std::string_view path, std::span<const int32_t> value);
template void MockTunableBackend::SetValue<std::vector<int64_t>>(
    std::string_view path, std::span<const int64_t> value);
template void MockTunableBackend::SetValue<std::vector<float>>(
    std::string_view path, std::span<const float> value);
template void MockTunableBackend::SetValue<std::vector<double>>(
    std::string_view path, std::span<const double> value);
template void MockTunableBackend::SetValue<std::vector<std::string>>(
    std::string_view path, std::span<const std::string> value);

void MockTunableBackend::SetStructData(std::string_view path,
                                       std::string_view typeString,
                                       std::span<const uint8_t> data) {
  std::string pathBuf;
  path = detail::NormalizeName(path, pathBuf);
  uint32_t uid = GetUidOrThrow(path);
  auto info = TunableRegistry::GetTunable(uid);
  if (info.type != detail::TunableTypeValue::STRUCT &&
      info.type != detail::TunableTypeValue::MEMBER_STRUCT) {
    throw std::invalid_argument(
        std::format("Tunable is not a struct: {}", path));
  }

  std::scoped_lock lock{m_mutex};
  m_actions.emplace_back(
      std::string{path}, uid,
      [data = std::vector<uint8_t>{data.begin(), data.end()}](
          uint32_t uid) mutable {
        if (auto info = TunableRegistry::GetTunable(uid)) {
          if (!IsMutable(info)) {
            return;
          }
          bool updated = false;
          if (auto v = detail::CastTunable<detail::TunableStructTag, false>(
                  info.tunable, info.type)) {
            updated = v->UnpackStruct(data);
          } else if (auto v =
                         detail::CastTunable<detail::TunableStructTag, true>(
                             info.tunable, info.type)) {
            updated = v->UnpackStruct(info.config->parent, data);
          }
          if (!updated) {
            return;
          }
          NotifyRemoteSet(info);
          NotifyOnTune(uid, info);
        }
      });
}

void MockTunableBackend::SetProtobufData(std::string_view path,
                                         std::string_view typeString,
                                         std::span<const uint8_t> data) {
  std::string pathBuf;
  path = detail::NormalizeName(path, pathBuf);
  uint32_t uid = GetUidOrThrow(path);
  auto info = TunableRegistry::GetTunable(uid);
  if (info.type != detail::TunableTypeValue::PROTOBUF &&
      info.type != detail::TunableTypeValue::MEMBER_PROTOBUF) {
    throw std::invalid_argument(
        std::format("Tunable is not a protobuf: {}", path));
  }

  std::scoped_lock lock{m_mutex};
  m_actions.emplace_back(
      std::string{path}, uid,
      [data = std::vector<uint8_t>{data.begin(), data.end()}](
          uint32_t uid) mutable {
        if (auto info = TunableRegistry::GetTunable(uid)) {
          if (!IsMutable(info)) {
            return;
          }
          bool updated = false;
          if (auto v = detail::CastTunable<detail::TunableProtobufTag, false>(
                  info.tunable, info.type)) {
            updated = v->UnpackProtobuf(data);
          } else if (auto v =
                         detail::CastTunable<detail::TunableProtobufTag, true>(
                             info.tunable, info.type)) {
            updated = v->UnpackProtobuf(info.config->parent, data);
          }
          if (!updated) {
            return;
          }
          NotifyRemoteSet(info);
          NotifyOnTune(uid, info);
        }
      });
}

bool MockTunableBackend::Publish(std::string_view path, uint32_t uid,
                                 detail::TunableBase& tunable,
                                 const TunableConfig* config,
                                 detail::TunableTypeValue type) {
  std::string pathBuf;
  path = detail::NormalizeName(path, pathBuf);
  std::scoped_lock lock{m_mutex};
  auto& curUid = m_tunables[path];
  if (curUid != 0) {
    TunableRegistry::ReportWarning(
        std::format("Tunable already exists: {}", path));
    return false;
  }
  curUid = uid;
  m_uids[uid].emplace_back(path);
  return true;
}

void MockTunableBackend::MarkDirty(uint32_t) {}

void MockTunableBackend::Remove(std::string_view path) {
  std::string pathBuf;
  path = detail::NormalizeName(path, pathBuf);
  std::scoped_lock lock{m_mutex};
  auto it = m_tunables.find(path);
  if (it != m_tunables.end()) {
    auto uid = it->second;
    auto uidIt = m_uids.find(uid);
    if (uidIt != m_uids.end()) {
      std::erase(uidIt->second, path);
      if (uidIt->second.empty()) {
        m_uids.erase(uidIt);
      }
    }
    m_tunables.erase(it);
    std::erase_if(m_actions,
                  [&](auto&& action) { return action.path == path; });
  }
}

std::vector<TunableBackend::PublishedTunable> MockTunableBackend::RemovePrefix(
    std::string_view prefix) {
  std::string prefixBuf;
  prefix = detail::NormalizePrefix(prefix, prefixBuf);
  std::scoped_lock lock{m_mutex};
  std::vector<PublishedTunable> removed;
  for (auto it = m_tunables.begin(); it != m_tunables.end();) {
    if (!detail::IsPathOrDescendant(it->first, prefix)) {
      ++it;
      continue;
    }
    std::string path{it->first};
    auto uid = it->second;
    removed.push_back({path, uid});
    if (auto uidIt = m_uids.find(uid); uidIt != m_uids.end()) {
      std::erase(uidIt->second, path);
      if (uidIt->second.empty()) {
        m_uids.erase(uidIt);
      }
    }
    it = m_tunables.erase(it);
    std::erase_if(m_actions, [&](auto&& action) {
      return detail::IsPathOrDescendant(action.path, prefix);
    });
  }
  return removed;
}

void MockTunableBackend::UnregisterTunable(uint32_t uid) {
  std::scoped_lock lock{m_mutex};
  auto it = m_uids.find(uid);
  if (it != m_uids.end()) {
    for (auto&& path : it->second) {
      m_tunables.erase(path);
    }
    m_uids.erase(it);
  }
  std::erase_if(m_actions, [&](auto&& action) { return action.uid == uid; });
}

std::optional<uint32_t> MockTunableBackend::GetUid(
    std::string_view path) const {
  std::string pathBuf;
  path = detail::NormalizeName(path, pathBuf);
  std::scoped_lock lock{m_mutex};
  auto it = m_tunables.find(path);
  if (it == m_tunables.end()) {
    return std::nullopt;
  }
  return it->second;
}

void MockTunableBackend::Update() {
  std::vector<Action> actions;
  {
    std::scoped_lock lock{m_mutex};
    actions = std::move(m_actions);
    m_actions.clear();
  }
  auto isCurrentAction = [this](const Action& action) {
    std::scoped_lock lock{m_mutex};
    auto it = m_tunables.find(action.path);
    return it != m_tunables.end() && it->second == action.uid;
  };
  for (auto&& action : actions) {
    if (!isCurrentAction(action)) {
      continue;
    }
    action.update(action.uid);
  }
}
