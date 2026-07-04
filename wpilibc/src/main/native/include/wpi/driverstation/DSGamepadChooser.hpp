// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <initializer_list>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace wpi {

class Gamepad;

/**
 * A set of selectable options controlled by a Gamepad.
 *
 * The D-pad up and down buttons select which selectable is active. The D-pad
 * left and right buttons change the selected option in the active selectable.
 */
class DSGamepadChooser {
 public:
  /** A single named set of selectable string options. */
  class GamepadSelectable {
   public:
    /**
     * Gets the selectable name.
     *
     * @return selectable name
     */
    std::string_view GetName() const;

    /**
     * Gets the available options.
     *
     * @return available options
     */
    const std::vector<std::string>& GetOptions() const;

    /**
     * Gets the currently selected option.
     *
     * @return the currently selected option
     */
    std::string_view GetSelected() const;

    /**
     * Gets the currently selected option index.
     *
     * @return the currently selected option index
     */
    int GetSelectedIndex() const;

   private:
    friend class DSGamepadChooser;

    GamepadSelectable(std::string_view name, std::vector<std::string> options);

    void SelectPrevious();
    void SelectNext();

    std::string m_name;
    std::vector<std::string> m_options;
    int m_selectedIndex = 0;
  };

  /**
   * Constructs a DSGamepadChooser.
   *
   * @param port The port index on the Driver Station that the gamepad is
   *             plugged into (0-5).
   */
  explicit DSGamepadChooser(int port);

  /**
   * Constructs a DSGamepadChooser.
   *
   * @param gamepad The gamepad used to change selections.
   */
  explicit DSGamepadChooser(Gamepad& gamepad);

  /**
   * Get the gamepad used to change selections.
   *
   * @return the gamepad
   */
  Gamepad& GetGamepad();

  /**
   * Get the gamepad used to change selections.
   *
   * @return the gamepad
   */
  const Gamepad& GetGamepad() const;

  /**
   * Adds a selectable with the given options.
   *
   * The first option is selected by default.
   *
   * @param name selectable name
   * @param options available options
   * @return the created selectable
   */
  GamepadSelectable& AddOptions(std::string_view name,
                                std::vector<std::string> options);

  /**
   * Adds a selectable with the given options.
   *
   * The first option is selected by default.
   *
   * @param name selectable name
   * @param options available options
   * @return the created selectable
   */
  GamepadSelectable& AddOptions(
      std::string_view name, std::initializer_list<std::string_view> options);

  /**
   * Adds a selectable with integer options from min to max, inclusive, stepping
   * by delta.
   *
   * The first option is selected by default.
   *
   * @param name selectable name
   * @param min minimum option
   * @param max maximum option
   * @param delta amount between options
   * @return the created selectable
   */
  GamepadSelectable& AddIntegerOptions(std::string_view name, int min,
                                       int max, int delta);

  /**
   * Adds a selectable with floating point options from min to max, inclusive,
   * stepping by delta.
   *
   * The first option is selected by default.
   *
   * @param name selectable name
   * @param min minimum option
   * @param max maximum option
   * @param delta amount between options
   * @return the created selectable
   */
  GamepadSelectable& AddDoubleOptions(std::string_view name, double min,
                                      double max, double delta);

  /**
   * Updates selectable selection and adds the current selections to the driver
   * station display.
   *
   * The D-pad up and down buttons select which selectable is active. The D-pad
   * left and right buttons change the selected option in the active selectable.
   *
   * Call DriverStationDisplay::UpdateLines() externally to flush display
   * updates.
   */
  void Update();

  /**
   * Gets the currently selected option for a selectable.
   *
   * @param name selectable name
   * @return the currently selected option
   */
  std::string_view GetSelected(std::string_view name) const;

  /**
   * Gets the currently selected integer option for a selectable.
   *
   * @param name selectable name
   * @return the currently selected option
   */
  int GetSelectedInteger(std::string_view name) const;

  /**
   * Gets the currently selected double option for a selectable.
   *
   * @param name selectable name
   * @return the currently selected option
   */
  double GetSelectedDouble(std::string_view name) const;

  /**
   * Gets the currently selected option index for a selectable.
   *
   * @param name selectable name
   * @return the currently selected option index
   */
  int GetSelectedIndex(std::string_view name) const;

  /**
   * Gets the names of all selectables.
   *
   * @return selectable names
   */
  std::vector<std::string> GetSelectableNames() const;

  /**
   * Gets the selectable currently controlled by D-pad left and right.
   *
   * @return the selected selectable, or nullptr if no selectable exists
   */
  GamepadSelectable* GetSelectedSelectable();

  /**
   * Gets the selectable currently controlled by D-pad left and right.
   *
   * @return the selected selectable, or nullptr if no selectable exists
   */
  const GamepadSelectable* GetSelectedSelectable() const;

 private:
  GamepadSelectable* GetSelectable(std::string_view name);
  const GamepadSelectable* GetSelectable(std::string_view name) const;

  static void RequireSelectableName(std::string_view name);
  static int Wrap(int value, int size);
  static std::string FormatDisplayLine(const GamepadSelectable& selectable,
                                       bool selected);

  Gamepad* m_gamepad;
  std::vector<std::unique_ptr<GamepadSelectable>> m_selectables;
  std::unordered_map<std::string, GamepadSelectable*> m_selectableMap;
  std::string m_captionPrefix;
  int m_selectedSelectable = 0;
};

}  // namespace wpi
