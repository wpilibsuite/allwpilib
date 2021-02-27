// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <vector>

namespace glass {

class WindowManager;

/**
 * GUI main menu bar.
 */
class MainMenuBar {
 public:
  /**
   * Displays the main menu bar.  Should be added to GUI LateExecute.
   */
  void Display();

  /**
   * Adds to GUI's main menu bar.  The menu function is called from within a
   * ImGui::BeginMainMenuBar()/EndMainMenuBar() block.  Usually it's only
   * appropriate to create a menu with ImGui::BeginMenu()/EndMenu() inside of
   * this function.
   *
   * @param menu menu display function
   */
  void AddMainMenu(std::function<void()> menu);

  /**
   * Adds to GUI's option menu.  The menu function is called from within a
   * ImGui::BeginMenu()/EndMenu() block.  Usually it's only appropriate to
   * create menu items inside of this function.
   *
   * @param menu menu display function
   */
  void AddOptionMenu(std::function<void()> menu);

 private:
  std::vector<std::function<void()>> m_optionMenus;
  std::vector<std::function<void()>> m_menus;
};

}  // namespace glass
