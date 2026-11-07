// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/glass/networktables/NTCommandScheduler.hpp"

#include <array>
#include <format>
#include <utility>

#include "wpi/glass/networktables/NTTunableTopic.hpp"
#include "wpi/util/StringExtras.hpp"

using namespace wpi::glass;

NTCommandSchedulerModel::NTCommandSchedulerModel(std::string_view path)
    : NTCommandSchedulerModel(wpi::nt::NetworkTableInstance::GetDefault(),
                              path) {}

NTCommandSchedulerModel::NTCommandSchedulerModel(
    wpi::nt::NetworkTableInstance inst, std::string_view path)
    : m_inst{inst},
      m_commands{inst.GetStringArrayTopic(std::format("{}/Names", path))
                     .Subscribe({})},
      m_ids{
          inst.GetIntegerArrayTopic(std::format("{}/Ids", path)).Subscribe({})},
      m_cancelTopic{inst.GetIntegerArrayTopic(std::format("{}/Cancel", path))},
      m_cancelTuneTopic{
          inst.GetIntegerArrayTopic(std::format("{}/Cancel/tune", path))},
      m_cancelValueTopic{
          inst.GetIntegerArrayTopic(std::format("{}/Cancel/value", path))},
      m_nameValue{wpi::util::rsplit(path, '/').second} {}

void NTCommandSchedulerModel::CancelCommand(size_t index) {
  if (index < m_idsValue.size()) {
    std::array<int64_t, 1> toCancel{m_idsValue[index]};
    if (IsTunableTopicMutable(m_cancelValueTopic)) {
      if (!m_cancelTune) {
        m_cancelTune = m_cancelTuneTopic.Publish();
      }
      m_cancelTune.Set(toCancel);
    } else if (IsTunableTopicMutable(m_cancelTopic)) {
      if (!m_cancel) {
        m_cancel = m_cancelTopic.Publish();
      }
      m_cancel.Set(toCancel);
    }
  }
}

void NTCommandSchedulerModel::Update() {
  for (auto&& v : m_commands.ReadQueue()) {
    m_commandsValue = std::move(v.value);
  }
  for (auto&& v : m_ids.ReadQueue()) {
    m_idsValue = std::move(v.value);
  }
}

bool NTCommandSchedulerModel::Exists() {
  return m_commands.Exists();
}

bool NTCommandSchedulerModel::IsReadOnly() {
  return !IsTunableTopicMutable(m_cancelValueTopic) &&
         !IsTunableTopicMutable(m_cancelTopic);
}
