// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/smartdashboard/SendableChooserBase.hpp"

#include <utility>

#include "wpi/util/sendable/SendableRegistry.hpp"

using namespace wpi;

std::atomic_int SendableChooserBase::s_instances{0};

SendableChooserBase::SendableChooserBase() : m_instance{s_instances++} {
  wpi::util::SendableRegistry::Add(this, "SendableChooser", m_instance);
}

SendableChooserBase::SendableChooserBase(SendableChooserBase&& oth)
    : SendableHelper(std::move(oth)),
      m_defaultChoice(std::move(oth.m_defaultChoice)),
      m_selected(std::move(oth.m_selected)),
      m_haveSelected(std::move(oth.m_haveSelected)),
      m_instance(std::move(oth.m_instance)) {}

SendableChooserBase& SendableChooserBase::operator=(SendableChooserBase&& oth) {
  SendableHelper::operator=(oth);
  std::scoped_lock lock(m_mutex, oth.m_mutex);
  m_defaultChoice = std::move(oth.m_defaultChoice);
  m_selected = std::move(oth.m_selected);
  m_haveSelected = std::move(oth.m_haveSelected);
  m_instance = std::move(oth.m_instance);
  return *this;
}
