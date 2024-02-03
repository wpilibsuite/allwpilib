// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef WPIUTIL_WPI_GLOBALSTATE_H_
#define WPIUTIL_WPI_GLOBALSTATE_H_

#include <functional>
#include <utility>

namespace wpi {

/**
 * Calls all registered reset functions.
 *
 * Intended to be called before and/or after a unit test. May crash and/or
 * segfault your robot code. Does nothing on a RoboRIO.
 */
void ResetGlobalState();

namespace impl {

using ResetCallback = std::function<void()>;

//
// Reset priorities. These should be ordered such that it is less likely
// that a segfault will occur.
//

// networktables should be reset first, as resetting some of the later items
// may setup networktables listeners and/or callbacks
static constexpr int GSPriorityNT = 100;
// various singletons
static constexpr int GSPrioritySmartDashboard = 150;
static constexpr int GSPriorityShuffleboard = 200;
static constexpr int GSPriorityLiveWindow = 250;
static constexpr int GSPriorityMotorSafety = 300;
static constexpr int GSPrioritySendableRegistry = 350;
// HAL related data
static constexpr int GSPriorityHalCallbacks = 600;
static constexpr int GSPriorityHalHandles = 650;
static constexpr int GSPriorityHalSimData = 700;

// vendors should be reset after all WPILib things. They are generally
// not going to depend on each other
static constexpr int GSPriorityVendor = 900;

/**
 * Registers a reset function. Intended to be used internally and by vendors.
 *
 * Reset functions should be registered once when program execution begins.
 * Reset functions may be called at any time from any thread, but will never
 * be called on a RoboRIO, and will never be called concurrently. It is not
 * guaranteed that they ever will be called.
 *
 * Reset functions should:
 *
 * - Run quickly
 * - Reset all global singletons such that the old state is torn down and
 *   the new state is immediately usable once the reset finishes. We
 *   suggest using std::unique_ptr to hold your singletons.
 *
 * Reset functions should not:
 *
 * - Throw exceptions
 */
void RegisterGlobalStateReset(int priority, ResetCallback fn);

/**
 * Helper for statically registering global state functions:
 *
 * @code{.cpp}
 * static RegisterGlobalStateResetHelper my_helper(GSPriorityVendor, []() {
 *   // do some reset thing here
 *   // my_singleton = std::make_unique<MySingleton>();
 * });
 * @endcode
 */
struct RegisterGlobalStateResetHelper {
  RegisterGlobalStateResetHelper(int priority, ResetCallback fn) {
    RegisterGlobalStateReset(priority, std::move(fn));
  }
};

}  // namespace impl
}  // namespace wpi

#endif  // WPIUTIL_WPI_GLOBALSTATE_H_
