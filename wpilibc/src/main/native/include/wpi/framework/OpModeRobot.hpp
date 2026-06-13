// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <concepts>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "wpi/driverstation/Alert.hpp"
#include "wpi/framework/RobotBase.hpp"
#include "wpi/hal/DriverStationTypes.hpp"
#include "wpi/internal/PeriodicPriorityQueue.hpp"
#include "wpi/opmode/OpMode.hpp"
#include "wpi/system/Watchdog.hpp"
#include "wpi/units/time.hpp"
#include "wpi/util/DenseMap.hpp"
#include "wpi/util/mutex.hpp"

namespace wpi::util {
class Color;
}  // namespace wpi::util

namespace wpi {

using RobotMode = wpi::hal::RobotMode;

namespace detail {
template <typename T>
concept OpModeDerived = std::derived_from<T, OpMode>;
template <typename T>
concept NoArgOpMode = std::constructible_from<T> && OpModeDerived<T>;
template <typename T, typename R>
concept OneArgOpMode = std::constructible_from<T, R&> && OpModeDerived<T>;
}  // namespace detail

/**
 * Concept indicating a class is derived from OpMode and has either a
 * no-argument constructor or a constructor that accepts R&.
 *
 * @tparam T opmode class
 * @tparam R robot class
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

  /// Default loop period.
  static constexpr auto DEFAULT_PERIOD = 20_ms;

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
   *
   * @param period The period of the robot loop function.
   */
  explicit OpModeRobotBase(wpi::units::second_t period);

  /**
   * Constructor for an OpModeRobot with a default loop time of 0.02 seconds.
   */
  explicit OpModeRobotBase();

  OpModeRobotBase(OpModeRobotBase&&) = delete;
  OpModeRobotBase& operator=(OpModeRobotBase&&) = delete;

  /**
   * Code that needs to know the DS state should go here.
   *
   * Users should override this method for initialization that needs to occur
   * after the DS is connected, such as needing the alliance information.
   */
  virtual void DriverStationConnected() {}

  /**
   * Function called periodically every loop, regardless of enabled state or
   * OpMode selection.
   */
  virtual void RobotPeriodic() {}

  /**
   * Function called once during robot initialization in simulation.
   */
  virtual void SimulationInit() {}

  /**
   * Function called periodically in simulation.
   */
  virtual void SimulationPeriodic() {}

  /**
   * Function called once when the robot becomes disabled.
   */
  virtual void DisabledInit() {}

  /**
   * Function called periodically while the robot is disabled.
   */
  virtual void DisabledPeriodic() {}

  /**
   * Function called once when the robot exits disabled state.
   */
  virtual void DisabledExit() {}

  /**
   * Function called periodically anytime when no opmode is selected, including
   * when the Driver Station is disconnected.
   */
  virtual void NonePeriodic() {}

  /**
   * Add a callback to run at a specific period.
   *
   * @param callback The callback to run.
   * @param period The period at which to run the callback.
   */
  void AddPeriodic(std::function<void()> callback, wpi::units::second_t period);

  /**
   * Return the system clock time in microseconds for the start of the current
   * periodic loop. This is in the same time base as
   * Timer.getMonotonicTimeStamp(), but is stable through a loop. It is updated
   * at the beginning of every periodic callback (including the normal periodic
   * loop).
   *
   * @return Robot running time in microseconds, as of the start of the current
   * periodic function.
   */
  wpi::units::microsecond_t GetLoopStartTime() const {
    return m_callbacks.GetLoopStartTime();
  }

  /**
   * Adds an operating mode option using a factory function that creates the
   * opmode. It's necessary to call PublishOpModes() to make the added modes
   * visible to the driver station.
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
                        std::string_view description,
                        const wpi::util::Color& textColor,
                        const wpi::util::Color& backgroundColor);

  /**
   * Adds an operating mode option using a factory function that creates the
   * opmode. It's necessary to call PublishOpModes() to make the added modes
   * visible to the driver station.
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

 protected:
  /**
   * Main robot loop function. Handles disabled state logic.
   */
  void LoopFunc();

 private:
  struct OpModeData {
    std::string name;
    OpModeFactory factory;
  };

  wpi::util::DenseMap<int64_t, OpModeData> m_opModes;
  wpi::util::mutex m_opModeMutex;

  wpi::internal::PeriodicPriorityQueue m_callbacks;
  HAL_NotifierHandle m_notifier;
  wpi::units::second_t m_period;
  std::chrono::microseconds m_startTime;
  Alert m_loopOverrunAlert;
  Watchdog m_watchdog;

  // OpMode lifecycle state
  int64_t m_lastModeId = -1;
  bool m_calledDriverStationConnected = false;
  bool m_lastEnabledState = false;
  std::shared_ptr<OpMode> m_currentOpMode;
  std::vector<wpi::internal::PeriodicPriorityQueue::Callback>
      m_activeOpModeCallbacks;
  std::optional<wpi::internal::PeriodicPriorityQueue::Callback>
      m_opmodePeriodic;
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
   * Constructor.
   *
   * @param period The period of the robot loop function.
   */
  explicit OpModeRobot(wpi::units::second_t period) : OpModeRobotBase{period} {}

  /**
   * Constructor for an OpModeRobot with a default loop time of 0.02 seconds.
   */
  explicit OpModeRobot() : OpModeRobotBase{} {}

  /**
   * Adds an operating mode option. It's necessary to call PublishOpModes() to
   * make the added modes visible to the driver station.
   *
   * @tparam T opmode class; must be a public, non-abstract subclass of OpMode
   * with a public constructor that either takes no arguments or accepts a
   * single argument of this class's type (the latter is preferred).
   * @param mode robot mode
   * @param name name of the operating mode
   * @param group group of the operating mode
   * @param description description of the operating mode
   * @param textColor text color
   * @param backgroundColor background color
   */
  template <ConstructibleOpMode<Derived> T>
  void AddOpMode(RobotMode mode, std::string_view name, std::string_view group,
                 std::string_view description,
                 const wpi::util::Color& textColor,
                 const wpi::util::Color& backgroundColor) {
    if constexpr (detail::OneArgOpMode<T, Derived>) {
      AddOpModeFactory(
          [this] { return std::make_unique<T>(*static_cast<Derived*>(this)); },
          mode, name, group, description, textColor, backgroundColor);
    } else if constexpr (detail::NoArgOpMode<T>) {
      AddOpModeFactory([] { return std::make_unique<T>(); }, mode, name, group,
                       description, textColor, backgroundColor);
    }
  }

  /**
   * Adds an operating mode option. It's necessary to call PublishOpModes() to
   * make the added modes visible to the driver station.
   *
   * @tparam T opmode class; must be a public, non-abstract subclass of OpMode
   * with a public constructor that either takes no arguments or accepts a
   * single argument of this class's type (the latter is preferred).
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
      AddOpModeFactory(
          [this] { return std::make_unique<T>(*static_cast<Derived*>(this)); },
          mode, name, group, description);
    } else if constexpr (detail::NoArgOpMode<T>) {
      AddOpModeFactory([] { return std::make_unique<T>(); }, mode, name, group,
                       description);
    }
  }
};

}  // namespace wpi
