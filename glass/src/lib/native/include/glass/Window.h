/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>
#include <string>
#include <utility>

#include <imgui.h>
#include <wpi/StringRef.h>
#include <wpi/Twine.h>

#include "glass/View.h"

namespace glass {

/**
 * Managed window information.
 * A Window owns the View that displays the window's contents.
 */
class Window {
 public:
  Window() = default;
  explicit Window(wpi::StringRef id) : m_id{id} {}

  wpi::StringRef GetId() const { return m_id; }

  enum Visibility { kHide = 0, kShow, kDisabled };

  bool HasView() { return static_cast<bool>(m_view); }

  void SetView(std::unique_ptr<View> view) { m_view = std::move(view); }

  View* GetView() { return m_view.get(); }
  const View* GetView() const { return m_view.get(); }

  bool IsVisible() const { return m_visible; }
  void SetVisible(bool visible) { m_visible = visible; }
  bool IsEnabled() const { return m_enabled; }
  void SetEnabled(bool enabled) { m_enabled = enabled; }

  void SetFlags(ImGuiWindowFlags flags) { m_flags = flags; }

  void SetName(const wpi::Twine& name) { m_name = name.str(); }

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
  void SetVisibility(Visibility visibility);

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
   * @return True if window went from invisible to visible.
   */
  bool DisplayMenuItem(const char* label = nullptr);

  /**
   * Scale default window position and size.
   */
  void ScaleDefault(float scale);

  void IniReadLine(const char* lineStr);
  void IniWriteAll(const char* typeName, ImGuiTextBuffer* out_buf);

 private:
  std::string m_id;
  std::string m_name;
  std::unique_ptr<View> m_view;
  ImGuiWindowFlags m_flags = 0;
  bool m_visible = true;
  bool m_enabled = true;
  bool m_renamePopupEnabled = true;
  ImGuiCond m_posCond = 0;
  ImGuiCond m_sizeCond = 0;
  ImVec2 m_pos;
  ImVec2 m_size;
  bool m_setPadding = false;
  ImVec2 m_padding;
};

}  // namespace glass
