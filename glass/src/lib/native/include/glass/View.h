// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include <wpi/FunctionExtras.h>

namespace glass {

/**
 * A view is the contents of a window (1:1 mapping).
 * It may reference multiple models.
 *
 * Typically a view is constructed by a Provider and the View's constructor
 * is given the corresponding Model(s).
 *
 * A view may retain a reference to its parent window for dynamic
 * window configuration.
 */
class View {
 public:
  virtual ~View() = default;

  /**
   * Displays the window contents.  Called by Window::Display() from within an
   * ImGui::Begin() / ImGui::End() block.
   */
  virtual void Display() = 0;

  /**
   * Called instead of Display() when the window is hidden (e.g. when
   * ImGui::Begin() returns false).
   */
  virtual void Hidden();
};

/**
 * Make a View for a display functor.
 *
 * @param display Display function
 * @return unique_ptr to View
 */
std::unique_ptr<View> MakeFunctionView(wpi::unique_function<void()> display);

}  // namespace glass
