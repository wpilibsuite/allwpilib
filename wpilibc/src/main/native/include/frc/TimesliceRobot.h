// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>

#include <units/time.h>

#include "frc/TimedRobot.h"

namespace frc {

/**
 * TimesliceRobot extends the TimedRobot robot program framework to provide
 * timeslice scheduling of periodic functions.
 *
 * The TimesliceRobot class is intended to be subclassed by a user creating a
 * robot program.
 *
 * This class schedules robot operations serially in a timeslice format.
 * TimedRobot's periodic functions are the first in the timeslice table with 0
 * ms offset and 20 ms period. You can schedule additional controller periodic
 * functions at a shorter period (5 ms by default). You give each one a
 * timeslice duration, then they're run sequentially. The main benefit of this
 * approach is consistent starting times for each controller periodic, which can
 * make odometry and estimators more accurate and controller outputs change more
 * consistently.
 *
 * Here's an example of measured subsystem durations and their timeslice
 * allocations:
 *
 * <table>
 *   <tr>
 *     <td><b>Subsystem</b></td>
 *     <td><b>Duration (ms)</b></td>
 *     <td><b>Allocation (ms)</b></td>
 *   </tr>
 *   <tr>
 *     <td><b>Total</b></td>
 *     <td>5.0</td>
 *     <td>5.0</td>
 *   </tr>
 *   <tr>
 *     <td>TimedRobot</td>
 *     <td>?</td>
 *     <td>2.0</td>
 *   </tr>
 *   <tr>
 *     <td>Drivetrain</td>
 *     <td>1.32</td>
 *     <td>1.5</td>
 *   </tr>
 *   <tr>
 *     <td>Flywheel</td>
 *     <td>0.6</td>
 *     <td>0.7</td>
 *   </tr>
 *   <tr>
 *     <td>Turret</td>
 *     <td>0.6</td>
 *     <td>0.8</td>
 *   </tr>
 *   <tr>
 *     <td><b>Free</b></td>
 *     <td>0.0</td>
 *     <td>N/A</td>
 *   </tr>
 * </table>
 *
 * Since TimedRobot periodic functions only run every 20ms, that leaves a 2 ms
 * empty spot in the allocation table for three of the four 5 ms cycles
 * comprising 20 ms. That's OK because the OS needs time to do other things.
 *
 * If the robot periodic functions and the controller periodic functions have a
 * lot of scheduling jitter that cause them to occasionally overlap with later
 * timeslices, consider giving the main robot thread a real-time priority using
 * frc::SetCurrentThreadPriority(). An RT priority of 15 is a reasonable choice.
 *
 * If you do enable RT though, <i>make sure your periodic functions do not
 * block</i>. If they do, the operating system will lock up, and you'll have to
 * boot the roboRIO into safe mode and delete the robot program to recover.
 */
class TimesliceRobot : public TimedRobot {
 public:
  /**
   * Constructor for TimesliceRobot.
   *
   * @param robotPeriodicAllocation The allocation to give the TimesliceRobot
   *                                periodic functions.
   * @param controllerPeriod The controller period. The sum of all scheduler
   *                         allocations should be less than or equal to this
   *                         value.
   */
  explicit TimesliceRobot(units::second_t robotPeriodicAllocation,
                          units::second_t controllerPeriod);

  /**
   * Schedule a periodic function with the constructor's controller period and
   * the given allocation. The function's runtime allocation will be placed
   * after the end of the previous one's.
   *
   * If a call to this function makes the allocations exceed the controller
   * period, an exception will be thrown since that means the TimesliceRobot
   * periodic functions and the given function will have conflicting
   * timeslices.
   *
   * @param func       Function to schedule.
   * @param allocation The function's runtime allocation out of the controller
   *                   period.
   */
  void Schedule(std::function<void()> func, units::second_t allocation);

 private:
  units::second_t m_nextOffset;
  units::second_t m_controllerPeriod;
};

}  // namespace frc
