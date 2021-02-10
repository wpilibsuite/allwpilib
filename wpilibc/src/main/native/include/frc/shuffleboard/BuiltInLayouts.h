// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/shuffleboard/LayoutType.h"

namespace frc {

/**
 * The types of layouts bundled with Shuffleboard.
 *
 * <pre>{@code
 * ShuffleboardLayout myList = Shuffleboard::GetTab("My Tab")
 *   .GetLayout(BuiltinLayouts::kList, "My List");
 * }</pre>
 */
enum class BuiltInLayouts {
  /**
   * Groups components in a vertical list. New widgets added to the layout will
   * be placed at the bottom of the list. <br>Custom properties: <table>
   * <tr><th>Name</th><th>Type</th><th>Default Value</th><th>Notes</th></tr>
   * <tr><td>Label position</td><td>String</td><td>"BOTTOM"</td>
   * <td>The position of component labels inside the grid. One of
   * {@code ["TOP", "LEFT", "BOTTOM", "RIGHT", "HIDDEN"}</td></tr>
   * </table>
   */
  kList,

  /**
   * Groups components in an <i>n</i> x <i>m</i> grid. Grid layouts default to
   * 3x3. <br>Custom properties: <table>
   * <tr><th>Name</th><th>Type</th><th>Default Value</th><th>Notes</th></tr>
   * <tr><td>Number of columns</td><td>Number</td><td>3</td><td>Must be in the
   * range [1,15]</td>
   * </tr>
   * <tr><td>Number of rows</td><td>Number</td><td>3</td><td>Must be in the
   * range [1,15]</td></tr> <tr> <td>Label position</td> <td>String</td>
   * <td>"BOTTOM"</td>
   * <td>The position of component labels inside the grid.
   * One of {@code ["TOP", "LEFT", "BOTTOM", "RIGHT", "HIDDEN"}</td>
   * </tr>
   * </table>
   */
  kGrid
};

}  // namespace frc
