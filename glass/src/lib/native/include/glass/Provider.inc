// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <utility>

#include "glass/Provider.h"

namespace glass {

template <typename Functions>
void Provider<Functions>::GlobalInit() {
  WindowManager::GlobalInit();
  wpi::gui::AddEarlyExecute([this] { Update(); });
}

template <typename Functions>
void Provider<Functions>::ShowDefault(std::string_view name) {
  auto win = GetWindow(name);
  if (win) {
    return;
  }
  auto it = FindViewEntry(name);
  if (it == m_viewEntries.end() || (*it)->name != name) {
    return;
  }
  (*it)->showDefault = true;
}

template <typename Functions>
void Provider<Functions>::Register(std::string_view name, ExistsFunc exists,
                                   CreateModelFunc createModel,
                                   CreateViewFunc createView) {
  RegisterModel(name, std::move(exists), std::move(createModel));
  RegisterView(name, name, nullptr, std::move(createView));
}

template <typename Functions>
void Provider<Functions>::RegisterModel(std::string_view name,
                                        ExistsFunc exists,
                                        CreateModelFunc createModel) {
  auto it = FindModelEntry(name);
  // ignore if exists
  if (it != m_modelEntries.end() && (*it)->name == name) {
    return;
  }
  // insert in sorted location
  m_modelEntries.emplace(
      it, MakeModelEntry(name, std::move(exists), std::move(createModel)));
}

template <typename Functions>
void Provider<Functions>::RegisterView(std::string_view name,
                                       std::string_view modelName,
                                       ViewExistsFunc exists,
                                       CreateViewFunc createView) {
  // find model; if model doesn't exist, ignore
  auto modelIt = FindModelEntry(modelName);
  if (modelIt == m_modelEntries.end() || (*modelIt)->name != modelName) {
    return;
  }

  auto viewIt = FindViewEntry(name);
  // ignore if exists
  if (viewIt != m_viewEntries.end() && (*viewIt)->name == name) {
    return;
  }
  // insert in sorted location
  m_viewEntries.emplace(viewIt,
                        MakeViewEntry(name, modelIt->get(), std::move(exists),
                                      std::move(createView)));
}

template <typename Functions>
void Provider<Functions>::Update() {
  // update entries
  for (auto&& entry : m_modelEntries) {
    if (entry->model) {
      entry->model->Update();
    }
  }
}

template <typename Functions>
typename Provider<Functions>::ModelEntries::iterator
Provider<Functions>::FindModelEntry(std::string_view name) {
  return std::lower_bound(
      m_modelEntries.begin(), m_modelEntries.end(), name,
      [](const auto& elem, std::string_view s) { return elem->name < s; });
}

template <typename Functions>
typename Provider<Functions>::ViewEntries::iterator
Provider<Functions>::FindViewEntry(std::string_view name) {
  return std::lower_bound(
      m_viewEntries.begin(), m_viewEntries.end(), name,
      [](const auto& elem, std::string_view s) { return elem->name < s; });
}

}  // namespace glass
