/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#ifdef __cplusplus
#include <functional>
#endif

extern "C" {

void HALSIMGUI_Add(void* param, void (*initialize)(void*));
void HALSIMGUI_AddExecute(void* param, void (*execute)(void*));
void HALSIMGUI_AddWindow(const char* name, void* param, void (*display)(void*),
                         int32_t flags);
void HALSIMGUI_AddMainMenu(void* param, void (*menu)(void*));
void HALSIMGUI_AddOptionMenu(void* param, void (*menu)(void*));
void HALSIMGUI_SetWindowVisibility(const char* name, int32_t visibility);
void HALSIMGUI_SetDefaultWindowPos(const char* name, float x, float y);
void HALSIMGUI_SetDefaultWindowSize(const char* name, float width,
                                    float height);
void HALSIMGUI_SetWindowPadding(const char* name, float x, float y);
int HALSIMGUI_AreOutputsDisabled(void);

}  // extern "C"

#ifdef __cplusplus

namespace halsimgui {

class HALSimGui {
 public:
  static bool Initialize();
  static void Main(void*);
  static void Exit(void*);

  /**
   * Adds feature to GUI.  The initialize function is called once, immediately
   * after the GUI (both GLFW and Dear ImGui) are initialized.
   *
   * @param initialize initialization function
   * @param execute frame execution function
   */
  static void Add(std::function<void()> initialize);

  /**
   * Adds per-frame executor to GUI.  The passed function is called on each
   * Dear ImGui frame prior to window and menu functions.
   *
   * @param execute frame execution function
   */
  static void AddExecute(std::function<void()> execute);

  /**
   * Adds window to GUI.  The display function is called from within a
   * ImGui::Begin()/End() block.  While windows can be created within the
   * execute function passed to AddExecute(), using this function ensures the
   * windows are consistently integrated with the rest of the GUI.
   *
   * On each Dear ImGui frame, AddExecute() functions are always called prior
   * to AddWindow display functions.  Note that windows may be shaded or
   * completely hidden, in which case this function will not be called.
   * It's important to perform any processing steps that must be performed
   * every frame in the AddExecute() function.
   *
   * @param name name of the window (title bar)
   * @param display window contents display function
   * @param flags Dear ImGui window flags
   */
  static void AddWindow(const char* name, std::function<void()> display,
                        int flags = 0);

  /**
   * Adds to GUI's main menu bar.  The menu function is called from within a
   * ImGui::BeginMainMenuBar()/EndMainMenuBar() block.  Usually it's only
   * appropriate to create a menu with ImGui::BeginMenu()/EndMenu() inside of
   * this function.
   *
   * On each Dear ImGui frame, AddExecute() functions are always called prior
   * to AddMainMenu menu functions.
   *
   * @param menu menu display function
   */
  static void AddMainMenu(std::function<void()> menu);

  /**
   * Adds to GUI's option menu.  The menu function is called from within a
   * ImGui::BeginMenu()/EndMenu() block.  Usually it's only appropriate to
   * create menu items inside of this function.
   *
   * On each Dear ImGui frame, AddExecute() functions are always called prior
   * to AddMainMenu menu functions.
   *
   * @param menu menu display function
   */
  static void AddOptionMenu(std::function<void()> menu);

  enum WindowVisibility { kHide = 0, kShow, kDisabled };

  /**
   * Sets visibility of window added with AddWindow().
   *
   * @param name window name (same as name passed to AddWindow())
   * @param visibility 0=hide, 1=show, 2=disabled (force-hide)
   */
  static void SetWindowVisibility(const char* name,
                                  WindowVisibility visibility);

  /**
   * Sets default position of window added with AddWindow().
   *
   * @param name window name (same as name passed to AddWindow())
   * @param x x location of upper left corner
   * @param y y location of upper left corner
   */
  static void SetDefaultWindowPos(const char* name, float x, float y);

  /**
   * Sets default size of window added with AddWindow().
   *
   * @param name window name (same as name passed to AddWindow())
   * @param width width
   * @param height height
   */
  static void SetDefaultWindowSize(const char* name, float width, float height);

  /**
   * Sets internal padding of window added with AddWindow().
   * @param name window name (same as name passed to AddWindow())
   * @param x horizontal padding
   * @param y vertical padding
   */
  static void SetWindowPadding(const char* name, float x, float y);

  /**
   * Returns true if outputs are disabled.
   *
   * @return true if outputs are disabled, false otherwise.
   */
  static bool AreOutputsDisabled();
};

}  // namespace halsimgui

#endif  // __cplusplus
