/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <functional>
#include <memory>
#include <type_traits>
#include <vector>

#include <imgui.h>
#include <wpi/FunctionExtras.h>
#include <wpi/StringRef.h>
#include <wpi/Twine.h>

#include "glass/Window.h"
#include "glass/support/IniSaverBase.h"

namespace glass {

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
   * @param iniName Group name to use in ini file
   */
  explicit WindowManager(const wpi::Twine& iniName);
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
   */
  Window* AddWindow(wpi::StringRef id, wpi::unique_function<void()> display);

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
   * @return Window, or nullptr on duplicate window
   */
  Window* AddWindow(wpi::StringRef id, std::unique_ptr<View> view);

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
   * @return Window, or nullptr on duplicate window
   */
  Window* GetOrAddWindow(wpi::StringRef id, bool duplicateOk = false);

  /**
   * Gets existing window.  If none exists, returns nullptr.
   *
   * @param id unique identifier of the window (default title bar)
   * @return Window, or nullptr if window does not exist
   */
  Window* GetWindow(wpi::StringRef id);

 protected:
  virtual void DisplayWindows();

  // kept sorted by id
  std::vector<std::unique_ptr<Window>> m_windows;

 private:
  class IniSaver : public IniSaverBase {
   public:
    explicit IniSaver(const wpi::Twine& typeName, WindowManager* manager)
        : IniSaverBase{typeName}, m_manager{manager} {}

    void* IniReadOpen(const char* name) override;
    void IniReadLine(void* entry, const char* lineStr) override;
    void IniWriteAll(ImGuiTextBuffer* out_buf) override;

   private:
    WindowManager* m_manager;
  };

  IniSaver m_iniSaver;
};

}  // namespace glass
