// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "sysid/telemetry/TelemetryManager.h"

#include <algorithm>
#include <cctype>  // for ::tolower
#include <numbers>
#include <stdexcept>
#include <string>
#include <utility>

#include <fmt/chrono.h>
#include <networktables/NetworkTableInstance.h>
#include <wpi/Logger.h>
#include <wpi/SmallVector.h>
#include <wpi/StringExtras.h>
#include <wpi/raw_ostream.h>
#include <wpi/timestamp.h>

#include "sysid/Util.h"
#include "sysid/analysis/AnalysisType.h"

using namespace sysid;

TelemetryManager::TelemetryManager(const Settings& settings,
                                   wpi::Logger& logger,
                                   nt::NetworkTableInstance instance)
    : m_settings(settings), m_logger(logger), m_inst(instance) {}

void TelemetryManager::BeginTest(std::string_view name) {
  // Create a new test params instance for this test.
  m_params =
      TestParameters{name.starts_with("fast"), name.ends_with("forward"),
                     m_settings.mechanism == analysis::kDrivetrainAngular,
                     State::WaitingForEnable};

  // Add this test to the list of running tests and set the running flag.
  m_tests.push_back(std::string{name});
  m_isRunningTest = true;

  // Set the Voltage Command Entry
  m_voltageCommand.Set((m_params.fast ? m_settings.stepVoltage
                                      : m_settings.quasistaticRampRate) *
                       (m_params.forward ? 1 : -1));

  // Set the test type
  m_testType.Set(m_params.fast ? "Dynamic" : "Quasistatic");

  // Set the rotate entry
  m_rotate.Set(m_params.rotate);

  // Set the current mechanism in NT.
  m_mechanism.Set(m_settings.mechanism.name);
  // Set Overflow to False
  m_overflowPub.Set(false);
  // Set Mechanism Error to False
  m_mechErrorPub.Set(false);
  m_inst.Flush();

  // Display the warning message.
  for (auto&& func : m_callbacks) {
    func(
        "Please enable the robot in autonomous mode, and then "
        "disable it "
        "before it runs out of space. \n Note: The robot will "
        "continue "
        "to move until you disable it - It is your "
        "responsibility to "
        "ensure it does not hit anything!");
  }

  WPI_INFO(m_logger, "Started {} test.", m_tests.back());
}

void TelemetryManager::EndTest() {
  // If there is no test running, this is a no-op
  if (!m_isRunningTest) {
    return;
  }

  // Disable the running flag and store the data in the JSON.
  m_isRunningTest = false;
  m_data[m_tests.back()] = m_params.data;

  // Call the cancellation callbacks.
  for (auto&& func : m_callbacks) {
    std::string msg;
    if (m_params.mechError) {
      msg +=
          "\nERROR: The robot indicated that you are using the wrong project "
          "for characterizing your mechanism. \nThis most likely means you "
          "are trying to characterize a mechanism like a Drivetrain with a "
          "deployed config for a General Mechanism (e.g. Arm, Flywheel, and "
          "Elevator) or vice versa. Please double check your settings and "
          "try again.";
    } else if (!m_params.data.empty()) {
      std::string units = m_settings.units;
      std::transform(m_settings.units.begin(), m_settings.units.end(),
                     units.begin(), ::tolower);

      if (std::string_view{m_settings.mechanism.name}.starts_with(
              "Drivetrain")) {
        double p = (m_params.data.back()[3] - m_params.data.front()[3]) *
                   m_settings.unitsPerRotation;
        double s = (m_params.data.back()[4] - m_params.data.front()[4]) *
                   m_settings.unitsPerRotation;
        double g = m_params.data.back()[7] - m_params.data.front()[7];

        msg = fmt::format(
            "The left and right encoders traveled {} {} and {} {} "
            "respectively.\nThe gyro angle delta was {} degrees.",
            p, units, s, units, g * 180.0 / std::numbers::pi);
      } else {
        double p = (m_params.data.back()[2] - m_params.data.front()[2]) *
                   m_settings.unitsPerRotation;
        msg = fmt::format("The encoder reported traveling {} {}.", p, units);
      }

      if (m_params.overflow) {
        msg +=
            "\nNOTE: the robot stopped recording data early because the entry "
            "storage was exceeded.";
      }
    } else {
      msg = "No data was detected.";
    }
    func(msg);
  }

  // Remove previously run test from list of tests if no data was detected.
  if (m_params.data.empty()) {
    m_tests.pop_back();
  }

  // Send a zero command over NT.
  m_voltageCommand.Set(0.0);
  m_inst.Flush();
}

void TelemetryManager::Update() {
  // If there is no test running, these is nothing to update.
  if (!m_isRunningTest) {
    return;
  }

  // Update the NT entries that we're reading.

  int currAckNumber = m_ackNumberSub.Get();
  std::string telemetryValue;

  // Get the FMS Control Word.
  for (auto tsValue : m_fmsControlData.ReadQueue()) {
    uint32_t ctrl = tsValue.value;
    m_params.enabled = ctrl & 0x01;
  }

  // Get the string in the data field.
  for (auto tsValue : m_telemetry.ReadQueue()) {
    telemetryValue = tsValue.value;
  }

  // Get the overflow flag
  for (auto tsValue : m_overflowSub.ReadQueue()) {
    m_params.overflow = tsValue.value;
  }

  // Get the mechanism error flag
  for (auto tsValue : m_mechErrorSub.ReadQueue()) {
    m_params.mechError = tsValue.value;
  }

  // Go through our state machine.
  if (m_params.state == State::WaitingForEnable) {
    if (m_params.enabled) {
      m_params.enableStart = wpi::Now() * 1E-6;
      m_params.state = State::RunningTest;
      m_ackNumber = currAckNumber;
      WPI_INFO(m_logger, "{}", "Transitioned to running test state.");
    }
  }

  if (m_params.state == State::RunningTest) {
    // If for some reason we've disconnected, end the test.
    if (!m_inst.IsConnected()) {
      WPI_WARNING(m_logger, "{}",
                  "NT connection was dropped when executing the test. The test "
                  "has been canceled.");
      EndTest();
    }

    // If the robot has disabled, then we can move on to the next step.
    if (!m_params.enabled) {
      m_params.disableStart = wpi::Now() * 1E-6;
      m_params.state = State::WaitingForData;
      WPI_INFO(m_logger, "{}", "Transitioned to waiting for data.");
    }
  }

  if (m_params.state == State::WaitingForData) {
    double now = wpi::Now() * 1E-6;
    m_voltageCommand.Set(0.0);
    m_inst.Flush();

    // Process valid data
    if (!telemetryValue.empty() && m_ackNumber < currAckNumber) {
      m_params.raw = std::move(telemetryValue);
      m_ackNumber = currAckNumber;
    }

    // We have the data that we need, so we can parse it and end the test.
    if (!m_params.raw.empty() &&
        wpi::starts_with(m_params.raw, m_tests.back())) {
      // Remove test type from start of string
      m_params.raw.erase(0, m_params.raw.find(';') + 1);

      // Clean up the string -- remove spaces if there are any.
      m_params.raw.erase(
          std::remove_if(m_params.raw.begin(), m_params.raw.end(), ::isspace),
          m_params.raw.end());

      // Split the string into individual components.
      wpi::SmallVector<std::string_view, 16> res;
      wpi::split(m_params.raw, res, ',');

      // Convert each string to double.
      std::vector<double> values;
      values.reserve(res.size());
      for (auto&& str : res) {
        values.push_back(wpi::parse_float<double>(str).value());
      }

      // Add the values to our result vector.
      for (size_t i = 0; i < values.size() - m_settings.mechanism.rawDataSize;
           i += m_settings.mechanism.rawDataSize) {
        std::vector<double> d(m_settings.mechanism.rawDataSize);

        std::copy_n(std::make_move_iterator(values.begin() + i),
                    m_settings.mechanism.rawDataSize, d.begin());
        m_params.data.push_back(std::move(d));
      }

      WPI_INFO(m_logger,
               "Received data with size: {} for the {} test in {} seconds.",
               m_params.data.size(), m_tests.back(),
               m_params.data.back()[0] - m_params.data.front()[0]);
      m_ackNumberPub.Set(++m_ackNumber);
      EndTest();
    }

    // If we timed out, end the test and let the user know.
    if (now - m_params.disableStart > 5.0) {
      WPI_WARNING(m_logger, "{}",
                  "TelemetryManager did not receieve data 5 seconds after "
                  "completing the test...");
      EndTest();
    }
  }
}

std::string TelemetryManager::SaveJSON(std::string_view location) {
  m_data["test"] = m_settings.mechanism.name;
  m_data["units"] = m_settings.units;
  m_data["unitsPerRotation"] = m_settings.unitsPerRotation;
  m_data["sysid"] = true;

  std::string loc = fmt::format("{}/sysid_data{:%Y%m%d-%H%M%S}.json", location,
                                std::chrono::system_clock::now());

  sysid::SaveFile(m_data.dump(2), std::filesystem::path{loc});
  WPI_INFO(m_logger, "Wrote JSON to: {}", loc);

  return loc;
}
