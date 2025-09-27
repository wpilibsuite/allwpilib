// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string_view>
#include <vector>

#include <wpi/FunctionExtras.h>

#include "glass/Window.h"

namespace glass {

class Storage;

/**
 * Window manager.
 *
 * To properly integrate into an application:
 * - Call GlobalInit() from the application main, after calling
 *   wpi::gui::CreateContext(), but before calling wpi::gui::Initialize().
 * - Add DisplayMenu() to the application's MainMenuBar.
 */
class WindowManager {
 public:
  /**
   * Constructor.
   *
   * @param storage Storage for window information
   */
  explicit WindowManager(Storage& storage);
  virtual ~WindowManager() = default;

  WindowManager(const WindowManager&) = delete;
  WindowManager& operator=(const WindowManager&) = delete;

  /**
   * Perform global initialization.  This should be called prior to
   * wpi::gui::Initialize().
   */
  virtual void GlobalInit();

  /**
   * Displays menu contents, one item for each window.
   * See Window::DisplayMenuItem().
   */
  virtual void DisplayMenu();

  /**
   * Adds window to GUI.  The display function is called from within a
   * ImGui::Begin()/End() block.  While windows can be created within the
   * execute function passed to gui::AddExecute(), using this function ensures
   * the windows are consistently integrated with the rest of the GUI.
   *
   * On each Dear ImGui frame, gui::AddExecute() functions are always called
   * prior to AddWindow display functions.  Note that windows may be shaded or
   * completely hidden, in which case this function will not be called.
   * It's important to perform any processing steps that must be performed
   * every frame in the gui::AddExecute() function.
   *
   * @param id unique identifier of the window (title bar)
   * @param display window contents display function
   * @param defaultVisibility default window visibility
   */
  Window* AddWindow(std::string_view id, wpi::unique_function<void()> display,
                    Window::Visibility defaultVisibility = Window::kShow);

  /**
   * Adds window to GUI.  The view's display function is called from within a
   * ImGui::Begin()/End() block.  While windows can be created within the
   * execute function passed to gui::AddExecute(), using this function ensures
   * the windows are consistently integrated with the rest of the GUI.
   *
   * On each Dear ImGui frame, gui::AddExecute() functions are always called
   * prior to AddWindow display functions.  Note that windows may be shaded or
   * completely hidden, in which case this function will not be called.
   * It's important to perform any processing steps that must be performed
   * every frame in the gui::AddExecute() function.
   *
   * @param id unique identifier of the window (title bar)
   * @param view view object
   * @param defaultVisibility default window visibility
   * @return Window, or nullptr on duplicate window
   */
  Window* AddWindow(std::string_view id, std::unique_ptr<View> view,
                    Window::Visibility defaultVisibility = Window::kShow);

  /**
   * Adds window to GUI.  A View must be assigned to the returned Window
   * to display the window contents.  While windows can be created within the
   * execute function passed to gui::AddExecute(), using this function ensures
   * the windows are consistently integrated with the rest of the GUI.
   *
   * On each Dear ImGui frame, gui::AddExecute() functions are always called
   * prior to AddWindow display functions.  Note that windows may be shaded or
   * completely hidden, in which case this function will not be called.
   * It's important to perform any processing steps that must be performed
   * every frame in the gui::AddExecute() function.
   *
   * @param id unique identifier of the window (default title bar)
   * @param duplicateOk if false, warn on duplicates
   * @param defaultVisibility default window visibility
   * @return Window, or nullptr on duplicate window
   */
  Window* GetOrAddWindow(std::string_view id, bool duplicateOk = false,
                         Window::Visibility defaultVisibility = Window::kShow);

  /**
   * Gets existing window.  If none exists, returns nullptr.
   *
   * @param id unique identifier of the window (default title bar)
   * @return Window, or nullptr if window does not exist
   */
  Window* GetWindow(std::string_view id);

  /**
   * Erases all windows.
   */
  void EraseWindows() { m_windows.clear(); }

  /**
   * Get window count.
   */
  size_t GetNumWindows() const { return m_windows.size(); }

 protected:
  /**
   * Removes existing window (by index)
   *
   * @param index index of window in m_windows
   */
  void RemoveWindow(size_t index);

  // kept sorted by id
  std::vector<std::unique_ptr<Window>> m_windows;

  Storage& m_storage;

 private:
  void DisplayWindows();
};

}  // namespace glass
