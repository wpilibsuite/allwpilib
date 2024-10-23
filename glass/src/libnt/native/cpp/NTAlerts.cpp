// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/networktables/NTAlerts.h"

#include <utility>

#include <fmt/format.h>

using namespace glass;

NTAlertsModel::NTAlertsModel(std::string_view path)
    : NTAlertsModel{nt::NetworkTableInstance::GetDefault(), path} {}

NTAlertsModel::NTAlertsModel(nt::NetworkTableInstance inst,
                             std::string_view path)
    : m_inst{inst},
      m_infos{m_inst.GetStringArrayTopic(fmt::format("{}/infos", path))
                  .Subscribe({})},
      m_warnings{m_inst.GetStringArrayTopic(fmt::format("{}/warnings", path))
                     .Subscribe({})},
      m_errors{m_inst.GetStringArrayTopic(fmt::format("{}/errors", path))
                   .Subscribe({})} {}

void NTAlertsModel::Update() {
  if (!m_infos.Exists()) {
    m_infosValue.clear();
  }
  for (auto&& v : m_infos.ReadQueue()) {
    m_infosValue = std::move(v.value);
  }

  if (!m_warnings.Exists()) {
    m_warningsValue.clear();
  }
  for (auto&& v : m_warnings.ReadQueue()) {
    m_warningsValue = std::move(v.value);
  }

  if (!m_errors.Exists()) {
    m_errorsValue.clear();
  }
  for (auto&& v : m_errors.ReadQueue()) {
    m_errorsValue = std::move(v.value);
  }
}

bool NTAlertsModel::Exists() {
  return m_infos.Exists();
}
