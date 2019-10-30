/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/smartdashboard/SendableChooserBase.h"

#include "frc/smartdashboard/SendableRegistry.h"

using namespace frc;

std::atomic_int SendableChooserBase::s_instances{0};

SendableChooserBase::SendableChooserBase() : m_instance{s_instances++} {
  SendableRegistry::GetInstance().Add(this, "SendableChooser", m_instance);
}

SendableChooserBase::SendableChooserBase(SendableChooserBase&& oth)
    : SendableHelper(std::move(oth)),
      m_defaultChoice(std::move(oth.m_defaultChoice)),
      m_selected(std::move(oth.m_selected)),
      m_haveSelected(std::move(oth.m_haveSelected)),
      m_activeEntries(std::move(oth.m_activeEntries)),
      m_instance(std::move(oth.m_instance)) {}

SendableChooserBase& SendableChooserBase::operator=(SendableChooserBase&& oth) {
  SendableHelper::operator=(oth);
  std::scoped_lock lock(m_mutex, oth.m_mutex);
  m_defaultChoice = std::move(oth.m_defaultChoice);
  m_selected = std::move(oth.m_selected);
  m_haveSelected = std::move(oth.m_haveSelected);
  m_activeEntries = std::move(oth.m_activeEntries);
  m_instance = std::move(oth.m_instance);
  return *this;
}
