// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/WindowManager.h"

#include <algorithm>
#include <cstdio>
#include <memory>
#include <utility>

#include <fmt/format.h>
#include <wpi/print.h>
#include <wpigui.h>

#include "glass/Context.h"
#include "glass/Storage.h"

using namespace glass;

WindowManager::WindowManager(Storage& storage) : m_storage{storage} {
  storage.SetCustomApply([this] {
    for (auto&& childIt : m_storage.GetChildren()) {
      GetOrAddWindow(childIt.key(), true);
    }
  });
}

Window* WindowManager::AddWindow(std::string_view id,
                                 wpi::unique_function<void()> display,
                                 Window::Visibility defaultVisibility) {
  auto win = GetOrAddWindow(id, false, defaultVisibility);
  if (!win) {
    return nullptr;
  }
  if (win->HasView()) {
    wpi::print(stderr, "GUI: ignoring duplicate window '{}'\n", id);
    return nullptr;
  }
  win->SetView(MakeFunctionView(std::move(display)));
  return win;
}

Window* WindowManager::AddWindow(std::string_view id,
                                 std::unique_ptr<View> view,
                                 Window::Visibility defaultVisibility) {
  auto win = GetOrAddWindow(id, false, defaultVisibility);
  if (!win) {
    return nullptr;
  }
  if (win->HasView()) {
    wpi::print(stderr, "GUI: ignoring duplicate window '{}'\n", id);
    return nullptr;
  }
  win->SetView(std::move(view));
  return win;
}

Window* WindowManager::GetOrAddWindow(std::string_view id, bool duplicateOk,
                                      Window::Visibility defaultVisibility) {
  // binary search
  auto it = std::lower_bound(
      m_windows.begin(), m_windows.end(), id,
      [](const auto& elem, std::string_view s) { return elem->GetId() < s; });
  if (it != m_windows.end() && (*it)->GetId() == id) {
    if (!duplicateOk) {
      wpi::print(stderr, "GUI: ignoring duplicate window '{}'\n", id);
      return nullptr;
    }
    return it->get();
  }
  // insert before (keeps sort)
  return m_windows
      .emplace(it, std::make_unique<Window>(
                       m_storage.GetChild(id).GetChild("window"), id,
                       defaultVisibility))
      ->get();
}

Window* WindowManager::GetWindow(std::string_view id) {
  // binary search
  auto it = std::lower_bound(
      m_windows.begin(), m_windows.end(), id,
      [](const auto& elem, std::string_view s) { return elem->GetId() < s; });
  if (it == m_windows.end() || (*it)->GetId() != id) {
    return nullptr;
  }
  return it->get();
}

void WindowManager::RemoveWindow(size_t index) {
  m_storage.Erase(m_windows[index]->GetId());
  m_windows.erase(m_windows.begin() + index);
}

void WindowManager::GlobalInit() {
  wpi::gui::AddWindowScaler([this](float scale) {
    // scale default window positions
    for (auto&& window : m_windows) {
      window->ScaleDefault(scale);
    }
  });
  wpi::gui::AddLateExecute([this] { DisplayWindows(); });
}

void WindowManager::DisplayMenu() {
  for (auto&& window : m_windows) {
    window->DisplayMenuItem();
  }
}

void WindowManager::DisplayWindows() {
  PushStorageStack(m_storage);
  for (auto&& window : m_windows) {
    window->Display();
  }
  PopStorageStack();
}
