// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <array>
#include <cstddef>
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <networktables/BooleanTopic.h>
#include <networktables/DoubleTopic.h>
#include <networktables/IntegerTopic.h>
#include <networktables/NetworkTableInstance.h>
#include <networktables/StringTopic.h>
#include <units/time.h>
#include <wpi/Logger.h>
#include <wpi/SmallVector.h>
#include <wpi/json.h>

#include "sysid/analysis/AnalysisType.h"

namespace sysid {
/**
 * This class is responsible for collecting data from the robot and storing it
 * inside a JSON.
 */
class TelemetryManager {
 public:
  /**
   * Represents settings for an instance of the TelemetryManager class. This
   * contains information about the quasistatic ramp rate for slow tests, the
   * step voltage for fast tests, and the mechanism type for characterization.
   */
  struct Settings {
    /**
     * The rate at which the voltage should increase during the quasistatic test
     * (V/s).
     */
    double quasistaticRampRate = 0.25;

    /**
     * The voltage that the dynamic test should run at (V).
     */
    double stepVoltage = 7.0;

    /**
     * The units the mechanism moves per recorded rotation. The sysid project
     * will be recording things in rotations of the shaft so the
     * unitsPerRotation is to convert those measurements to the units the user
     * wants to use.
     */
    double unitsPerRotation = 1.0;

    /**
     * The name of the units used.
     * Valid units:  "Meters", "Feet", "Inches", "Radians", "Degrees",
     * "Rotations"
     */
    std::string units = "Meters";

    /**
     * The type of mechanism that will be analyzed.
     * Supported mechanisms: Drivetrain, Angular Drivetrain, Elevator, Arm,
     * Simple motor.
     */
    AnalysisType mechanism = analysis::kDrivetrain;
  };

  /**
   * Constructs an instance of the telemetry manager with the provided settings
   * and NT instance to collect data over.
   *
   * @param settings The settings for this instance of the telemetry manager.
   * @param logger   The logger instance to use for log data.
   * @param instance The NT instance to collect data over. The default value of
   *                 this parameter should suffice in production; it should only
   *                 be changed during unit testing.
   */
  explicit TelemetryManager(const Settings& settings, wpi::Logger& logger,
                            nt::NetworkTableInstance instance =
                                nt::NetworkTableInstance::GetDefault());

  /**
   * Begins a test with the given parameters.
   *
   * @param name The name of the test.
   */
  void BeginTest(std::string_view name);

  /**
   * Ends the currently running test. If there is no test running, this is a
   * no-op.
   */
  void EndTest();

  /**
   * Updates the telemetry manager -- this adds a new autospeed entry and
   * collects newest data from the robot. This must be called periodically by
   * the user.
   */
  void Update();

  /**
   * Registers a callback that's called by the TelemetryManager when there is a
   * message to display to the user.
   *
   * @param callback Callback function that runs based off of the message
   */
  void RegisterDisplayCallback(std::function<void(std::string_view)> callback) {
    m_callbacks.emplace_back(std::move(callback));
  }

  /**
   * Saves a JSON with the stored data at the given location.
   *
   * @param location The location to save the JSON at (this is the folder that
   *                 should contain the saved JSON).
   * @return The full file path of the saved JSON.
   */
  std::string SaveJSON(std::string_view location);

  /**
   * Returns whether a test is currently running.
   *
   * @return Whether a test is currently running.
   */
  bool IsActive() const { return m_isRunningTest; }

  /**
   * Returns whether the specified test is running or has run.
   *
   * @param name The test to check.
   *
   * @return Whether the specified test is running or has run.
   */
  bool HasRunTest(std::string_view name) const {
    return std::find(m_tests.cbegin(), m_tests.cend(), name) != m_tests.end();
  }

  /**
   * Gets the size of the stored data.
   *
   * @return The size of the stored data
   */
  size_t GetCurrentDataSize() const { return m_params.data.size(); }

 private:
  enum class State { WaitingForEnable, RunningTest, WaitingForData };

  /**
   * Stores information about a currently running test. This information
   * includes whether the robot will be traveling quickly (dynamic) or slowly
   * (quasistatic), the direction of movement, the start time of the test,
   * whether the robot is enabled, the current speed of the robot, and the
   * collected data.
   */
  struct TestParameters {
    bool fast = false;
    bool forward = false;
    bool rotate = false;

    State state = State::WaitingForEnable;

    double enableStart = 0.0;
    double disableStart = 0.0;

    bool enabled = false;
    double speed = 0.0;

    std::string raw;
    std::vector<std::vector<double>> data{};
    bool overflow = false;
    bool mechError = false;

    TestParameters() = default;
    TestParameters(bool fast, bool forward, bool rotate, State state)
        : fast{fast}, forward{forward}, rotate{rotate}, state{state} {}
  };

  // Settings for this instance.
  const Settings& m_settings;

  // Logger.
  wpi::Logger& m_logger;

  // Test parameters for the currently running test.
  TestParameters m_params;
  bool m_isRunningTest = false;

  // A list of running or already run tests.
  std::vector<std::string> m_tests;

  // Stores the test data.
  wpi::json m_data;

  // Display callbacks.
  wpi::SmallVector<std::function<void(std::string_view)>, 1> m_callbacks;

  // NetworkTables instance and entries.
  nt::NetworkTableInstance m_inst;
  std::shared_ptr<nt::NetworkTable> table = m_inst.GetTable("SmartDashboard");
  nt::DoublePublisher m_voltageCommand =
      table->GetDoubleTopic("SysIdVoltageCommand").Publish();
  nt::StringPublisher m_testType =
      table->GetStringTopic("SysIdTestType").Publish();
  nt::BooleanPublisher m_rotate =
      table->GetBooleanTopic("SysIdRotate").Publish();
  nt::StringPublisher m_mechanism =
      table->GetStringTopic("SysIdTest").Publish();
  nt::BooleanPublisher m_overflowPub =
      table->GetBooleanTopic("SysIdOverflow").Publish();
  nt::BooleanSubscriber m_overflowSub =
      table->GetBooleanTopic("SysIdOverflow").Subscribe(false);
  nt::BooleanPublisher m_mechErrorPub =
      table->GetBooleanTopic("SysIdWrongMech").Publish();
  nt::BooleanSubscriber m_mechErrorSub =
      table->GetBooleanTopic("SysIdWrongMech").Subscribe(false);
  nt::StringSubscriber m_telemetry =
      table->GetStringTopic("SysIdTelemetry").Subscribe("");
  nt::IntegerSubscriber m_fmsControlData =
      m_inst.GetTable("FMSInfo")
          ->GetIntegerTopic("FMSControlData")
          .Subscribe(0);
  nt::DoublePublisher m_ackNumberPub =
      table->GetDoubleTopic("SysIdAckNumber").Publish();
  nt::DoubleSubscriber m_ackNumberSub =
      table->GetDoubleTopic("SysIdAckNumber").Subscribe(0);

  int m_ackNumber;
};
}  // namespace sysid
