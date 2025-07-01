// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/tunable/MockTunableBackend.hpp"

#include <mutex>
#include <string>
#include <utility>
#include <vector>

#include "wpi/tunable/TunableConfig.hpp"
#include "wpi/tunable/TunableRegistry.hpp"
#include "wpi/tunable/detail/TunableTypeTraits.hpp"
#include "wpi/util/type_name.hpp"

using namespace wpi;

static void NotifyOnTune(TunableRegistry::TunableInfo& info) {
  info.ResetChanged();
  if (auto config = info.config) {
    if (auto&& onTune = config->onTune) {
      onTune(*info.tunable, config->parent);
    }
  }
}

MockTunableBackend::~MockTunableBackend() = default;

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
  std::scoped_lock lock{m_mutex};
  uint32_t uid = m_tunables[path];
  auto info = TunableRegistry::GetTunable(uid);
  if (info.type != detail::GetTunableTypeValue<T, false>() &&
      info.type != detail::GetTunableTypeValue<T, true>()) {
    throw std::runtime_error(fmt::format("Tunable is not a {}: {}",
                                         wpi::util::GetTypeName<T>(), path));
  }
  m_actions.emplace_back(
      uid, [value = MakeCopy<T>(std::move(value))](uint32_t uid) mutable {
        if (auto info = TunableRegistry::GetTunable(uid)) {
          if (auto v = detail::CastTunable<T, false>(info.tunable, info.type)) {
            v->Set(std::move(value));
          } else if (auto v = detail::CastTunable<T, true>(info.tunable,
                                                           info.type)) {
            v->Set(info.config->parent, std::move(value));
          }
          NotifyOnTune(info);
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
  std::scoped_lock lock{m_mutex};
  uint32_t uid = m_tunables[path];
  auto info = TunableRegistry::GetTunable(uid);
  if (info.type != detail::TunableTypeValue::STRUCT &&
      info.type != detail::TunableTypeValue::MEMBER_STRUCT) {
    throw std::runtime_error(fmt::format("Tunable is not a struct: {}", path));
  }
  m_actions.emplace_back(uid, [data = std::vector<uint8_t>{data.begin(),
                                                           data.end()}](
                                  uint32_t uid) mutable {
    if (auto info = TunableRegistry::GetTunable(uid)) {
      if (auto v = detail::CastTunable<detail::TunableStructTag, false>(
              info.tunable, info.type)) {
        v->UnpackStruct(data);
      } else if (auto v = detail::CastTunable<detail::TunableStructTag, true>(
                     info.tunable, info.type)) {
        v->UnpackStruct(info.config->parent, data);
      }
      NotifyOnTune(info);
    }
  });
}

void MockTunableBackend::SetProtobufData(std::string_view path,
                                         std::string_view typeString,
                                         std::span<const uint8_t> data) {
  std::scoped_lock lock{m_mutex};
  uint32_t uid = m_tunables[path];
  auto info = TunableRegistry::GetTunable(uid);
  if (info.type != detail::TunableTypeValue::PROTOBUF &&
      info.type != detail::TunableTypeValue::MEMBER_PROTOBUF) {
    throw std::runtime_error(
        fmt::format("Tunable is not a protobuf: {}", path));
  }
  m_actions.emplace_back(uid, [data = std::vector<uint8_t>{data.begin(),
                                                           data.end()}](
                                  uint32_t uid) mutable {
    if (auto info = TunableRegistry::GetTunable(uid)) {
      if (auto v = detail::CastTunable<detail::TunableProtobufTag, false>(
              info.tunable, info.type)) {
        v->UnpackProtobuf(data);
      } else if (auto v = detail::CastTunable<detail::TunableProtobufTag, true>(
                     info.tunable, info.type)) {
        v->UnpackProtobuf(info.config->parent, data);
      }
      NotifyOnTune(info);
    }
  });
}

void MockTunableBackend::Publish(std::string_view path, uint32_t uid,
                                 detail::TunableBase& tunable,
                                 const TunableConfig* config,
                                 detail::TunableTypeValue type) {
  std::scoped_lock lock{m_mutex};
  auto& curUid = m_tunables[path];
  if (curUid != 0) {
    throw std::runtime_error(fmt::format("Tunable already exists: {}", path));
  }
  curUid = uid;
  m_uids[uid].emplace_back(path);
}

void MockTunableBackend::Remove(std::string_view path) {
  std::scoped_lock lock{m_mutex};
  auto it = m_tunables.find(path);
  if (it != m_tunables.end()) {
    auto uidIt = m_uids.find(it->second);
    if (uidIt != m_uids.end()) {
      std::erase(uidIt->second, path);
    }
    m_tunables.erase(it);
    std::erase_if(m_actions,
                  [&](auto&& action) { return action.uid == it->second; });
  }
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

void MockTunableBackend::Update() {
  std::scoped_lock lock{m_mutex};
  for (auto&& action : m_actions) {
    action.update(action.uid);
  }
  m_actions.clear();
}
