// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <algorithm>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <networktables/NTSendableBuilder.h>

#include "frc/smartdashboard/SendableChooser.h"

namespace frc {

template <class T>
void SendableChooser<T>::AddOption(std::string_view name, T object) {
  m_choices[name] = std::move(object);
}

template <class T>
void SendableChooser<T>::SetDefaultOption(std::string_view name, T object) {
  m_defaultChoice = name;
  AddOption(name, std::move(object));
}

template <class T>
auto SendableChooser<T>::GetSelected()
    -> decltype(_unwrap_smart_ptr(m_choices[""])) {
  std::string selected = m_defaultChoice;
  {
    std::scoped_lock lock(m_mutex);
    if (m_haveSelected) {
      selected = m_selected;
    }
  }
  if (selected.empty()) {
    return decltype(_unwrap_smart_ptr(m_choices[""])){};
  } else {
    return _unwrap_smart_ptr(m_choices[selected]);
  }
}

template <class T>
void SendableChooser<T>::InitSendable(nt::NTSendableBuilder& builder) {
  builder.SetSmartDashboardType("String Chooser");
  {
    std::scoped_lock lock(m_mutex);
    m_instancePubs.emplace_back(
        nt::IntegerTopic{builder.GetTopic(kInstance)}.Publish());
    m_instancePubs.back().Set(m_instance);
    m_activePubs.emplace_back(
        nt::StringTopic{builder.GetTopic(kActive)}.Publish());
  }
  builder.AddStringArrayProperty(
      kOptions,
      [=, this] {
        std::vector<std::string> keys;
        for (const auto& choice : m_choices) {
          keys.emplace_back(choice.first());
        }

        // Unlike std::map, wpi::StringMap elements
        // are not sorted
        std::sort(keys.begin(), keys.end());

        return keys;
      },
      nullptr);
  builder.AddSmallStringProperty(
      kDefault,
      [=, this](wpi::SmallVectorImpl<char>&) -> std::string_view {
        return m_defaultChoice;
      },
      nullptr);
  builder.AddSmallStringProperty(
      kActive,
      [=, this](wpi::SmallVectorImpl<char>& buf) -> std::string_view {
        std::scoped_lock lock(m_mutex);
        if (m_haveSelected) {
          buf.assign(m_selected.begin(), m_selected.end());
          return {buf.data(), buf.size()};
        } else {
          return m_defaultChoice;
        }
      },
      nullptr);
  builder.AddStringProperty(kSelected, nullptr,
                            [=, this](std::string_view val) {
                              std::scoped_lock lock(m_mutex);
                              m_haveSelected = true;
                              m_selected = val;
                              for (auto& pub : m_activePubs) {
                                pub.Set(val);
                              }
                            });
}

template <class T>
template <class U>
U SendableChooser<T>::_unwrap_smart_ptr(const U& value) {
  return value;
}

template <class T>
template <class U>
U* SendableChooser<T>::_unwrap_smart_ptr(const std::unique_ptr<U>& value) {
  return value.get();
}

template <class T>
template <class U>
std::weak_ptr<U> SendableChooser<T>::_unwrap_smart_ptr(
    const std::shared_ptr<U>& value) {
  return value;
}

}  // namespace frc
