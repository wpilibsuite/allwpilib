/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <utility>

namespace glass {

template <typename Functions>
void Provider<Functions>::GlobalInit() {
  WindowManager::GlobalInit();
  wpi::gui::AddEarlyExecute([this] { Update(); });
}

template <typename Functions>
void Provider<Functions>::ShowDefault(wpi::StringRef name) {
  auto win = GetWindow(name);
  if (win) return;
  auto it = FindViewEntry(name);
  if (it == m_viewEntries.end() || (*it)->name != name) return;
  this->Show(it->get(), (*it)->window);
}

template <typename Functions>
void Provider<Functions>::Register(wpi::StringRef name, ExistsFunc exists,
                                   CreateModelFunc createModel,
                                   CreateViewFunc createView) {
  RegisterModel(name, std::move(exists), std::move(createModel));
  RegisterView(name, name, nullptr, std::move(createView));
}

template <typename Functions>
void Provider<Functions>::RegisterModel(wpi::StringRef name, ExistsFunc exists,
                                        CreateModelFunc createModel) {
  auto it = FindModelEntry(name);
  // ignore if exists
  if (it != m_modelEntries.end() && (*it)->name == name) return;
  // insert in sorted location
  m_modelEntries.emplace(
      it, MakeModelEntry(name, std::move(exists), std::move(createModel)));
}

template <typename Functions>
void Provider<Functions>::RegisterView(wpi::StringRef name,
                                       wpi::StringRef modelName,
                                       ViewExistsFunc exists,
                                       CreateViewFunc createView) {
  // find model; if model doesn't exist, ignore
  auto modelIt = FindModelEntry(modelName);
  if (modelIt == m_modelEntries.end() || (*modelIt)->name != modelName) return;

  auto viewIt = FindViewEntry(name);
  // ignore if exists
  if (viewIt != m_viewEntries.end() && (*viewIt)->name == name) return;
  // insert in sorted location
  m_viewEntries.emplace(viewIt,
                        MakeViewEntry(name, modelIt->get(), std::move(exists),
                                      std::move(createView)));
}

template <typename Functions>
void Provider<Functions>::Update() {
  // update entries
  for (auto&& entry : m_modelEntries) {
    if (entry->model) entry->model->Update();
  }
}

template <typename Functions>
typename Provider<Functions>::ModelEntries::iterator
Provider<Functions>::FindModelEntry(wpi::StringRef name) {
  return std::lower_bound(
      m_modelEntries.begin(), m_modelEntries.end(), name,
      [](const auto& elem, wpi::StringRef s) { return elem->name < s; });
}

template <typename Functions>
typename Provider<Functions>::ViewEntries::iterator
Provider<Functions>::FindViewEntry(wpi::StringRef name) {
  return std::lower_bound(
      m_viewEntries.begin(), m_viewEntries.end(), name,
      [](const auto& elem, wpi::StringRef s) { return elem->name < s; });
}

}  // namespace glass
