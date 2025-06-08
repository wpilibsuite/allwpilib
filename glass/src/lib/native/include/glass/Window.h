// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <utility>

#include <imgui.h>

#include "glass/Context.h"
#include "glass/Storage.h"
#include "glass/View.h"

namespace glass {

/**
 * Managed window information.
 * A Window owns the View that displays the window's contents.
 */
class Window {
 public:
  enum Visibility { kHide = 0, kShow, kDisabled };

  Window(Storage& storage, std::string_view id,
         Visibility defaultVisibility = kShow)
      : Window{storage, storage.GetChild("window"), id, defaultVisibility} {}

  std::string_view GetId() const { return m_id; }

  bool HasView() { return static_cast<bool>(m_view); }

  void SetView(std::unique_ptr<View> view) { m_view = std::move(view); }

  View* GetView() { return m_view.get(); }
  const View* GetView() const { return m_view.get(); }

  bool IsVisible() const { return m_visible; }
  void SetVisible(bool visible) { m_visible = visible; }
  bool IsEnabled() const { return m_enabled; }
  void SetEnabled(bool enabled) { m_enabled = enabled; }

  void SetFlags(ImGuiWindowFlags flags) { m_flags = flags; }

  void SetName(std::string_view name) { m_name = name; }
  void SetDefaultName(std::string_view name) { m_defaultName = name; }

  /**
   * Normally windows provide a right-click popup menu on the title bar to
   * rename the window.  Calling this disables that functionality so the
   * view can provide its own popup.
   */
  void DisableRenamePopup() { m_renamePopupEnabled = false; }

  /**
   * Sets visibility of window.
   *
   * @param visibility 0=hide, 1=show, 2=disabled (force-hide)
   */
  void SetVisibility(Visibility visibility) {
    m_visible = visibility != kHide;
    m_enabled = visibility != kDisabled;
  }

  /**
   * Sets default visibility of window.
   *
   * @param visibility 0=hide, 1=show, 2=disabled (force-hide)
   */
  void SetDefaultVisibility(Visibility visibility) {
    m_defaultVisible = visibility != kHide;
    m_defaultEnabled = visibility != kDisabled;
  }

  /**
   * Sets default position of window.
   *
   * @param x x location of upper left corner
   * @param y y location of upper left corner
   */
  void SetDefaultPos(float x, float y) {
    m_posCond = ImGuiCond_FirstUseEver;
    m_pos = ImVec2{x, y};
  }

  /**
   * Sets default size of window.
   *
   * @param width width
   * @param height height
   */
  void SetDefaultSize(float width, float height) {
    m_sizeCond = ImGuiCond_FirstUseEver;
    m_size = ImVec2{width, height};
  }

  /**
   * Sets internal padding of window.
   * @param x horizontal padding
   * @param y vertical padding
   */
  void SetPadding(float x, float y) {
    m_setPadding = true;
    m_padding = ImVec2{x, y};
  }

  /**
   * Displays window.
   */
  void Display();

  /**
   * Displays menu item for the window.
   * @param label what to display as the menu item label; defaults to
   *              window ID if nullptr
   * @param enabled false to force-disable menu item (if window not visible)
   * @return True if window went from invisible to visible.
   */
  bool DisplayMenuItem(const char* label = nullptr, bool enabled = true);

  /**
   * Scale default window position and size.
   */
  void ScaleDefault(float scale);

  [[nodiscard]]
  bool BeginWindow();
  void EndWindow();

  bool EditName() { return ItemEditName(&m_name); }

 private:
  Window(Storage& storage, Storage& windowStorage, std::string_view id,
         Visibility defaultVisibility);

  Storage& m_storage;
  std::string m_id;
  std::string& m_name;
  std::string m_defaultName;
  std::unique_ptr<View> m_view;
  ImGuiWindowFlags m_flags = 0;
  bool& m_visible;
  bool& m_enabled;
  bool& m_defaultVisible;
  bool& m_defaultEnabled;
  bool m_renamePopupEnabled = true;
  ImGuiCond m_posCond = 0;
  ImGuiCond m_sizeCond = 0;
  ImVec2 m_pos;
  ImVec2 m_size;
  bool m_setPadding = false;
  ImVec2 m_padding;
  bool m_inWindow = false;
};

namespace imm {
Window* CreateWindow(
    Storage& root, std::string_view id, bool duplicateOk = false,
    Window::Visibility defaultVisibility = Window::Visibility::kShow);

inline Window* CreateWindow(
    std::string_view id, bool duplicateOk = false,
    Window::Visibility defaultVisibility = Window::Visibility::kShow) {
  return CreateWindow(GetStorage(), id, duplicateOk, defaultVisibility);
}

inline Window* GetWindow() {
  return GetStorage().GetChild("window").GetData<Window>();
}

inline Window* GetWindow(std::string_view id) {
  return GetStorage().GetChild(id).GetChild("window").GetData<Window>();
}

[[nodiscard]]
inline bool BeginWindow(Window* window) {
  assert(window);
  return window->BeginWindow();
}

[[nodiscard]]
inline bool BeginWindow() {
  return BeginWindow(GetWindow());
}

[[nodiscard]]
inline bool BeginWindow(std::string_view id) {
  return BeginWindow(GetWindow(id));
}

inline void EndWindow() {
  Window* window = GetWindow();
  assert(window);
  window->EndWindow();
}

[[nodiscard]]
bool BeginWindowSettingsPopup();

}  // namespace imm

}  // namespace glass
