// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <algorithm>
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <wpi/sendable/SendableBuilder.h>

#include "frc/smartdashboard/SendableChooser.h"

namespace frc {

template <class T>
  requires std::copy_constructible<T> && std::default_initializable<T>
void SendableChooser<T>::AddOption(std::string_view name, T object) {
  m_choices[name] = std::move(object);
}

template <class T>
  requires std::copy_constructible<T> && std::default_initializable<T>
void SendableChooser<T>::SetDefaultOption(std::string_view name, T object) {
  m_defaultChoice = name;
  AddOption(name, std::move(object));
}

template <class T>
  requires std::copy_constructible<T> && std::default_initializable<T>
typename SendableChooser<T>::CopyType SendableChooser<T>::GetSelected() const {
  std::string selected = m_defaultChoice;
  {
    std::scoped_lock lock(m_mutex);
    if (m_haveSelected) {
      selected = m_selected;
    }
  }
  if (selected.empty()) {
    return CopyType{};
  } else {
    return _unwrap_smart_ptr(m_choices.lookup(selected));
  }
}

template <class T>
  requires std::copy_constructible<T> && std::default_initializable<T>
void SendableChooser<T>::OnChange(std::function<void(T)> listener) {
  std::scoped_lock lock(m_mutex);
  m_listener = listener;
}

template <class T>
  requires std::copy_constructible<T> && std::default_initializable<T>
void SendableChooser<T>::InitSendable(wpi::SendableBuilder& builder) {
  builder.SetSmartDashboardType("String Chooser");
  builder.PublishConstInteger(kInstance, m_instance);
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
                              T choice{};
                              std::function<void(T)> listener;
                              {
                                std::scoped_lock lock(m_mutex);
                                m_haveSelected = true;
                                m_selected = val;
                                if (m_previousVal != val && m_listener) {
                                  choice = m_choices[val];
                                  listener = m_listener;
                                }
                                m_previousVal = val;
                              }
                              if (listener) {
                                listener(choice);
                              }
                            });
}

template <class T>
  requires std::copy_constructible<T> && std::default_initializable<T>
template <class U>
U SendableChooser<T>::_unwrap_smart_ptr(const U& value) {
  return value;
}

template <class T>
  requires std::copy_constructible<T> && std::default_initializable<T>
template <class U>
std::weak_ptr<U> SendableChooser<T>::_unwrap_smart_ptr(
    const std::shared_ptr<U>& value) {
  return value;
}

}  // namespace frc
