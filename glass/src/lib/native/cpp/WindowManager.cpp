/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "glass/WindowManager.h"

#include <algorithm>

#include <wpi/StringRef.h>
#include <wpi/raw_ostream.h>
#include <wpigui.h>

using namespace glass;

WindowManager::WindowManager(const wpi::Twine& iniName)
    : m_iniSaver{iniName, this} {}

// read/write open state to ini file
void* WindowManager::IniSaver::IniReadOpen(const char* name) {
  return m_manager->GetOrAddWindow(name, true);
}

void WindowManager::IniSaver::IniReadLine(void* entry, const char* lineStr) {
  static_cast<Window*>(entry)->IniReadLine(lineStr);
}

void WindowManager::IniSaver::IniWriteAll(ImGuiTextBuffer* out_buf) {
  const char* typeName = GetTypeName();
  for (auto&& window : m_manager->m_windows) {
    window->IniWriteAll(typeName, out_buf);
  }
}

Window* WindowManager::AddWindow(wpi::StringRef id,
                                 wpi::unique_function<void()> display) {
  auto win = GetOrAddWindow(id, false);
  if (!win) return nullptr;
  if (win->HasView()) {
    wpi::errs() << "GUI: ignoring duplicate window '" << id << "'\n";
    return nullptr;
  }
  win->SetView(MakeFunctionView(std::move(display)));
  return win;
}

Window* WindowManager::AddWindow(wpi::StringRef id,
                                 std::unique_ptr<View> view) {
  auto win = GetOrAddWindow(id, false);
  if (!win) return nullptr;
  if (win->HasView()) {
    wpi::errs() << "GUI: ignoring duplicate window '" << id << "'\n";
    return nullptr;
  }
  win->SetView(std::move(view));
  return win;
}

Window* WindowManager::GetOrAddWindow(wpi::StringRef id, bool duplicateOk) {
  // binary search
  auto it = std::lower_bound(
      m_windows.begin(), m_windows.end(), id,
      [](const auto& elem, wpi::StringRef s) { return elem->GetId() < s; });
  if (it != m_windows.end() && (*it)->GetId() == id) {
    if (!duplicateOk) {
      wpi::errs() << "GUI: ignoring duplicate window '" << id << "'\n";
      return nullptr;
    }
    return it->get();
  }
  // insert before (keeps sort)
  return m_windows.emplace(it, std::make_unique<Window>(id))->get();
}

Window* WindowManager::GetWindow(wpi::StringRef id) {
  // binary search
  auto it = std::lower_bound(
      m_windows.begin(), m_windows.end(), id,
      [](const auto& elem, wpi::StringRef s) { return elem->GetId() < s; });
  if (it == m_windows.end() || (*it)->GetId() != id) return nullptr;
  return it->get();
}

void WindowManager::GlobalInit() {
  wpi::gui::AddInit([this] { m_iniSaver.Initialize(); });
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
  for (auto&& window : m_windows) {
    window->Display();
  }
}
