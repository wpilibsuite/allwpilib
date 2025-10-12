// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <concepts>
#include <cstdint>
#include <functional>
#include <memory>

#include <wpi/DenseMap.h>

#include "frc/DSControlWord.h"
#include "frc/RobotBase.h"
#include "frc/opmode/OpMode.h"

namespace frc {

class Color;

namespace detail {
template <typename T>
concept NoArgOpMode = std::constructible_from<T>;
template <typename T, typename R>
concept OneArgOpMode = std::constructible_from<T, R&>;
}  // namespace detail

/**
 *
 */
template <typename T, typename R>
concept ConstructibleOpMode =
    detail::NoArgOpMode<T> || detail::OneArgOpMode<T, R>;

/**
 * OpModeRobotBase is the non-templated base class for OpModeRobot. Users should
 * generally prefer using OpModeRobot instead of this class.
 *
 * Opmodes are constructed when selected on the driver station, and destroyed
 * when the robot is disabled after being enabled or a different opmode is
 * selected. When no opmode is selected, NonePeriodic() is called. The
 * DriverStationConnected() function is called the first time the driver station
 * connects to the robot.
 */
class OpModeRobotBase : public RobotBase {
 public:
  using OpModeFactory = std::function<std::unique_ptr<OpMode>()>;

  /**
   * Provide an alternate "main loop" via StartCompetition().
   */
  void StartCompetition() override;

  /**
   * Ends the main loop in StartCompetition().
   */
  void EndCompetition() override;

  /**
   * Constructor.
   */
  OpModeRobotBase() = default;
  OpModeRobotBase(OpModeRobotBase&&) = default;
  OpModeRobotBase& operator=(OpModeRobotBase&&) = default;

  ~OpModeRobotBase() override;

  /**
   * Function called exactly once after the DS is connected.
   *
   * Code that needs to know the DS state should go here.
   *
   * Users should override this method for initialization that needs to occur
   * after the DS is connected, such as needing the alliance information.
   */
  virtual void DriverStationConnected() {}

  /**
   * Function called periodically anytime when no opmode is selected, including
   * when the Driver Station is disconnected.
   */
  virtual void NonePeriodic() {}

  /**
   * Adds an operating mode option. It's necessary to call PublishOpModes() to
   * make the added modes visible to the driver station.
   *
   * @param factory factory function
   * @param mode robot mode
   * @param name name of the operating mode
   * @param group group of the operating mode
   * @param description description of the operating mode
   * @param textColor text color
   * @param backgroundColor background color
   */
  void AddOpModeFactory(OpModeFactory factory, RobotMode mode,
                        std::string_view name, std::string_view group,
                        std::string_view description, const Color& textColor,
                        const Color& backgroundColor);

  /**
   * Adds an operating mode option. It's necessary to call PublishOpModes() to
   * make the added modes visible to the driver station.
   *
   * @param factory factory function
   * @param mode robot mode
   * @param name name of the operating mode
   * @param group group of the operating mode
   * @param description description of the operating mode
   */
  void AddOpModeFactory(OpModeFactory factory, RobotMode mode,
                        std::string_view name, std::string_view group = {},
                        std::string_view description = {});

  /**
   * Removes an operating mode option. It's necessary to call PublishOpModes()
   * to make the removed mode no longer visible to the driver station.
   *
   * @param mode robot mode
   * @param name name of the operating mode
   */
  void RemoveOpMode(RobotMode mode, std::string_view name);

  /**
   * Publishes the operating mode options to the driver station.
   */
  void PublishOpModes();

  /**
   * Clears all operating mode options and publishes an empty list to the driver
   * station.
   */
  void ClearOpModes();

 private:
  wpi::DenseMap<int64_t, OpModeFactory> m_opModes;
  std::unique_ptr<OpMode> m_activeOpMode;
};

/**
 * OpModeRobot implements the opmode-based robot program framework.
 *
 * The OpModeRobot class is intended to be subclassed by a user creating a robot
 * program. Users must provide their derived class as a template parameter to
 * this class.
 *
 * Opmodes are constructed when selected on the driver station, and destroyed
 * when the robot is disabled after being enabled or a different opmode is
 * selected. When no opmode is selected, NonePeriodic() is called. The
 * DriverStationConnected() function is called the first time the driver station
 * connects to the robot.
 *
 * @tparam Derived derived class
 */
template <typename Derived>
class OpModeRobot : public OpModeRobotBase {
 public:
  /**
   * Adds an operating mode option. It's necessary to call PublishOpModes() to
   * make the added modes visible to the driver station.
   *
   * @tparam T opmode class
   * @param mode robot mode
   * @param name name of the operating mode
   * @param group group of the operating mode
   * @param description description of the operating mode
   * @param textColor text color
   * @param backgroundColor background color
   */
  template <ConstructibleOpMode<Derived> T>
  void AddOpMode(RobotMode mode, std::string_view name, std::string_view group,
                 std::string_view description, const Color& textColor,
                 const Color& backgroundColor) {
    if constexpr (detail::OneArgOpMode<T, Derived>) {
      AddOpModeFactory([this] { return std::make_unique<T>(*this); }, mode,
                       name, group, description, textColor, backgroundColor);
    } else if constexpr (detail::NoArgOpMode<T>) {
      AddOpModeFactory([] { return std::make_unique<T>(); }, mode, name, group,
                       description, textColor, backgroundColor);
    }
  }

  /**
   * Adds an operating mode option. It's necessary to call PublishOpModes() to
   * make the added modes visible to the driver station.
   *
   * @tparam T opmode class
   * @param mode robot mode
   * @param name name of the operating mode
   * @param group group of the operating mode
   * @param description description of the operating mode
   */
  template <ConstructibleOpMode<Derived> T>
  void AddOpMode(RobotMode mode, std::string_view name,
                 std::string_view group = {},
                 std::string_view description = {}) {
    if constexpr (detail::OneArgOpMode<T, Derived>) {
      AddOpModeFactory([this] { return std::make_unique<T>(*this); }, mode,
                       name, group, description);
    } else if constexpr (detail::NoArgOpMode<T>) {
      AddOpModeFactory([] { return std::make_unique<T>(); }, mode, name, group,
                       description);
    }
  }
};

}  // namespace frc
