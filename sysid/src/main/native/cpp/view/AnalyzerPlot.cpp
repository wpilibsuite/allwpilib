// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "sysid/view/AnalyzerPlot.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <mutex>
#include <utility>
#include <vector>

#include <fmt/format.h>
#include <units/math.h>

#include "sysid/Util.h"
#include "sysid/analysis/AnalysisManager.h"
#include "sysid/analysis/ArmSim.h"
#include "sysid/analysis/ElevatorSim.h"
#include "sysid/analysis/FilteringUtils.h"
#include "sysid/analysis/SimpleMotorSim.h"

using namespace sysid;

static ImPlotPoint Getter(int idx, void* data) {
  return static_cast<ImPlotPoint*>(data)[idx];
}

template <typename Model>
static std::vector<std::vector<ImPlotPoint>> PopulateTimeDomainSim(
    const std::vector<PreparedData>& data,
    const std::array<units::second_t, 4>& startTimes, size_t step, Model model,
    double* simSquaredErrorSum, double* squaredVariationSum,
    int* timeSeriesPoints) {
  // Create the vector of ImPlotPoints that will contain our simulated data.
  std::vector<std::vector<ImPlotPoint>> pts;
  std::vector<ImPlotPoint> tmp;

  auto startTime = data[0].timestamp;

  tmp.emplace_back(startTime.value(), data[0].velocity);

  model.Reset(data[0].position, data[0].velocity);
  units::second_t t = 0_s;

  for (size_t i = 1; i < data.size(); ++i) {
    const auto& now = data[i];
    const auto& pre = data[i - 1];

    t += now.timestamp - pre.timestamp;

    // If the current time stamp and previous time stamp are across a test's
    // start timestamp, it is the start of a new test and the model needs to be
    // reset.
    if (std::find(startTimes.begin(), startTimes.end(), now.timestamp) !=
        startTimes.end()) {
      pts.emplace_back(std::move(tmp));
      model.Reset(now.position, now.velocity);
      continue;
    }

    model.Update(units::volt_t{pre.voltage}, now.timestamp - pre.timestamp);
    tmp.emplace_back((startTime + t).value(), model.GetVelocity());
    *simSquaredErrorSum += std::pow(now.velocity - model.GetVelocity(), 2);
    *squaredVariationSum += std::pow(now.velocity, 2);
    ++(*timeSeriesPoints);
  }

  pts.emplace_back(std::move(tmp));
  return pts;
}

AnalyzerPlot::AnalyzerPlot(wpi::Logger& logger) : m_logger(logger) {}

void AnalyzerPlot::SetRawTimeData(const std::vector<PreparedData>& rawSlow,
                                  const std::vector<PreparedData>& rawFast,
                                  std::atomic<bool>& abort) {
  auto rawSlowStep = std::ceil(rawSlow.size() * 1.0 / kMaxSize * 4);
  auto rawFastStep = std::ceil(rawFast.size() * 1.0 / kMaxSize * 4);
  // Populate Raw Slow Time Series Data
  for (size_t i = 0; i < rawSlow.size(); i += rawSlowStep) {
    if (abort) {
      return;
    }
    m_quasistaticData.rawData.emplace_back((rawSlow[i].timestamp).value(),
                                           rawSlow[i].velocity);
  }

  // Populate Raw fast Time Series Data
  for (size_t i = 0; i < rawFast.size(); i += rawFastStep) {
    if (abort) {
      return;
    }
    m_dynamicData.rawData.emplace_back((rawFast[i].timestamp).value(),
                                       rawFast[i].velocity);
  }
}

void AnalyzerPlot::ResetData() {
  m_quasistaticData.Clear();
  m_dynamicData.Clear();
  m_regressionData.Clear();
  m_timestepData.Clear();

  FitPlots();
}

void AnalyzerPlot::SetGraphLabels(std::string_view unit) {
  std::string_view abbreviation = GetAbbreviation(unit);
  m_velocityLabel = fmt::format("Velocity ({}/s)", abbreviation);
  m_accelerationLabel = fmt::format("Acceleration ({}/s²)", abbreviation);
  m_velPortionAccelLabel =
      fmt::format("Velocity-Portion Accel ({}/s²)", abbreviation);
}

void AnalyzerPlot::SetRawData(const Storage& data, std::string_view unit,
                              std::atomic<bool>& abort) {
  const auto& [slowForward, slowBackward, fastForward, fastBackward] = data;
  const auto& slow = m_direction == 0 ? slowForward : slowBackward;
  const auto& fast = m_direction == 0 ? fastForward : fastBackward;

  SetGraphLabels(unit);

  std::scoped_lock lock(m_mutex);

  ResetData();
  SetRawTimeData(slow, fast, abort);
}

void AnalyzerPlot::SetData(const Storage& rawData, const Storage& filteredData,
                           std::string_view unit,
                           const AnalysisManager::FeedforwardGains& ffGains,
                           const std::array<units::second_t, 4>& startTimes,
                           AnalysisType type, std::atomic<bool>& abort) {
  double simSquaredErrorSum = 0;
  double squaredVariationSum = 0;
  int timeSeriesPoints = 0;

  const auto& Ks = ffGains.Ks.gain;
  const auto& Kv = ffGains.Kv.gain;
  const auto& Ka = ffGains.Ka.gain;

  auto& [slowForward, slowBackward, fastForward, fastBackward] = filteredData;
  auto& [rawSlowForward, rawSlowBackward, rawFastForward, rawFastBackward] =
      rawData;

  const auto slow = AnalysisManager::DataConcat(slowForward, slowBackward);
  const auto fast = AnalysisManager::DataConcat(fastForward, fastBackward);
  const auto rawSlow =
      AnalysisManager::DataConcat(rawSlowForward, rawSlowBackward);
  const auto rawFast =
      AnalysisManager::DataConcat(rawFastForward, rawFastBackward);

  SetGraphLabels(unit);

  std::scoped_lock lock(m_mutex);

  ResetData();

  // Calculate step sizes to ensure that we only use the memory that we
  // allocated.
  auto slowStep = std::ceil(slow.size() * 1.0 / kMaxSize * 4);
  auto fastStep = std::ceil(fast.size() * 1.0 / kMaxSize * 4);

  units::second_t dtMean = GetMeanTimeDelta(filteredData);

  // Velocity-vs-time plots
  {
    const auto& slow = m_direction == 0 ? slowForward : slowBackward;
    const auto& fast = m_direction == 0 ? fastForward : fastBackward;
    const auto& rawSlow = m_direction == 0 ? rawSlowForward : rawSlowBackward;
    const auto& rawFast = m_direction == 0 ? rawFastForward : rawFastBackward;

    // Populate quasistatic time-domain graphs
    for (size_t i = 0; i < slow.size(); i += slowStep) {
      if (abort) {
        return;
      }

      m_quasistaticData.filteredData.emplace_back((slow[i].timestamp).value(),
                                                  slow[i].velocity);

      if (i > 0) {
        // If the current timestamp is not in the startTimes array, it is the
        // during a test and should be included. If it is in the startTimes
        // array, it is the beginning of a new test and the dt will be inflated.
        // Therefore we skip those to exclude that dt and effectively reset dt
        // calculations.
        if (slow[i].dt > 0_s &&
            std::find(startTimes.begin(), startTimes.end(),
                      slow[i].timestamp) == startTimes.end()) {
          m_timestepData.data.emplace_back(
              (slow[i].timestamp).value(),
              units::millisecond_t{slow[i].dt}.value());
        }
      }
    }

    // Populate dynamic time-domain graphs
    for (size_t i = 0; i < fast.size(); i += fastStep) {
      if (abort) {
        return;
      }

      m_dynamicData.filteredData.emplace_back((fast[i].timestamp).value(),
                                              fast[i].velocity);

      if (i > 0) {
        // If the current timestamp is not in the startTimes array, it is the
        // during a test and should be included. If it is in the startTimes
        // array, it is the beginning of a new test and the dt will be inflated.
        // Therefore we skip those to exclude that dt and effectively reset dt
        // calculations.
        if (fast[i].dt > 0_s &&
            std::find(startTimes.begin(), startTimes.end(),
                      fast[i].timestamp) == startTimes.end()) {
          m_timestepData.data.emplace_back(
              (fast[i].timestamp).value(),
              units::millisecond_t{fast[i].dt}.value());
        }
      }
    }

    SetRawTimeData(rawSlow, rawFast, abort);

    // Populate simulated time domain data
    if (type == analysis::kElevator) {
      const auto& Kg = ffGains.Kg.gain;
      m_quasistaticData.simData = PopulateTimeDomainSim(
          rawSlow, startTimes, fastStep, sysid::ElevatorSim{Ks, Kv, Ka, Kg},
          &simSquaredErrorSum, &squaredVariationSum, &timeSeriesPoints);
      m_dynamicData.simData = PopulateTimeDomainSim(
          rawFast, startTimes, fastStep, sysid::ElevatorSim{Ks, Kv, Ka, Kg},
          &simSquaredErrorSum, &squaredVariationSum, &timeSeriesPoints);
    } else if (type == analysis::kArm) {
      const auto& Kg = ffGains.Kg.gain;
      const auto& offset = ffGains.offset.gain;
      m_quasistaticData.simData = PopulateTimeDomainSim(
          rawSlow, startTimes, fastStep, sysid::ArmSim{Ks, Kv, Ka, Kg, offset},
          &simSquaredErrorSum, &squaredVariationSum, &timeSeriesPoints);
      m_dynamicData.simData = PopulateTimeDomainSim(
          rawFast, startTimes, fastStep, sysid::ArmSim{Ks, Kv, Ka, Kg, offset},
          &simSquaredErrorSum, &squaredVariationSum, &timeSeriesPoints);
    } else {
      m_quasistaticData.simData = PopulateTimeDomainSim(
          rawSlow, startTimes, fastStep, sysid::SimpleMotorSim{Ks, Kv, Ka},
          &simSquaredErrorSum, &squaredVariationSum, &timeSeriesPoints);
      m_dynamicData.simData = PopulateTimeDomainSim(
          rawFast, startTimes, fastStep, sysid::SimpleMotorSim{Ks, Kv, Ka},
          &simSquaredErrorSum, &squaredVariationSum, &timeSeriesPoints);
    }
  }

  // Acceleration-vs-velocity plot

  // Find minimum velocity of slow and fast datasets, then find point for line
  // of best fit
  auto slowMinVel =
      std::min_element(slow.cbegin(), slow.cend(), [](auto& a, auto& b) {
        return a.velocity < b.velocity;
      })->velocity;
  auto fastMinVel =
      std::min_element(fast.cbegin(), fast.cend(), [](auto& a, auto& b) {
        return a.velocity < b.velocity;
      })->velocity;
  auto minVel = std::min(slowMinVel, fastMinVel);
  m_regressionData.fitLine[0] = ImPlotPoint{minVel, -Kv / Ka * minVel};

  // Find maximum velocity of slow and fast datasets, then find point for line
  // of best fit
  auto slowMaxVel =
      std::max_element(slow.cbegin(), slow.cend(), [](auto& a, auto& b) {
        return a.velocity < b.velocity;
      })->velocity;
  auto fastMaxVel =
      std::max_element(fast.cbegin(), fast.cend(), [](auto& a, auto& b) {
        return a.velocity < b.velocity;
      })->velocity;
  auto maxVel = std::max(slowMaxVel, fastMaxVel);
  m_regressionData.fitLine[1] = ImPlotPoint{maxVel, -Kv / Ka * maxVel};

  // Populate acceleration vs velocity graph
  for (size_t i = 0; i < slow.size(); i += slowStep) {
    if (abort) {
      return;
    }

    // Calculate portion of acceleration caused by back-EMF
    double accelPortion = slow[i].acceleration - 1.0 / Ka * slow[i].voltage +
                          std::copysign(Ks / Ka, slow[i].velocity);

    if (type == analysis::kElevator) {
      const auto& Kg = ffGains.Kg.gain;
      accelPortion -= Kg / Ka;
    } else if (type == analysis::kArm) {
      const auto& Kg = ffGains.Kg.gain;
      accelPortion -= Kg / Ka * slow[i].cos;
    }

    m_regressionData.data.emplace_back(slow[i].velocity, accelPortion);
  }
  for (size_t i = 0; i < fast.size(); i += fastStep) {
    if (abort) {
      return;
    }

    // Calculate portion of voltage that corresponds to change in acceleration.
    double accelPortion = fast[i].acceleration - 1.0 / Ka * fast[i].voltage +
                          std::copysign(Ks / Ka, fast[i].velocity);

    if (type == analysis::kElevator) {
      const auto& Kg = ffGains.Kg.gain;
      accelPortion -= Kg / Ka;
    } else if (type == analysis::kArm) {
      const auto& Kg = ffGains.Kg.gain;
      accelPortion -= Kg / Ka * fast[i].cos;
    }

    m_regressionData.data.emplace_back(fast[i].velocity, accelPortion);
  }

  // Timestep-vs-time plot

  for (size_t i = 0; i < slow.size(); i += slowStep) {
    if (i > 0) {
      // If the current timestamp is not in the startTimes array, it is the
      // during a test and should be included. If it is in the startTimes
      // array, it is the beginning of a new test and the dt will be inflated.
      // Therefore we skip those to exclude that dt and effectively reset dt
      // calculations.
      if (slow[i].dt > 0_s &&
          std::find(startTimes.begin(), startTimes.end(), slow[i].timestamp) ==
              startTimes.end()) {
        m_timestepData.data.emplace_back(
            (slow[i].timestamp).value(),
            units::millisecond_t{slow[i].dt}.value());
      }
    }
  }

  for (size_t i = 0; i < fast.size(); i += fastStep) {
    if (i > 0) {
      // If the current timestamp is not in the startTimes array, it is the
      // during a test and should be included. If it is in the startTimes
      // array, it is the beginning of a new test and the dt will be inflated.
      // Therefore we skip those to exclude that dt and effectively reset dt
      // calculations.
      if (fast[i].dt > 0_s &&
          std::find(startTimes.begin(), startTimes.end(), fast[i].timestamp) ==
              startTimes.end()) {
        m_timestepData.data.emplace_back(
            (fast[i].timestamp).value(),
            units::millisecond_t{fast[i].dt}.value());
      }
    }
  }

  auto minTime =
      units::math::min(slow.front().timestamp, fast.front().timestamp);
  m_timestepData.fitLine[0] =
      ImPlotPoint{minTime.value(), units::millisecond_t{dtMean}.value()};

  auto maxTime = units::math::max(slow.back().timestamp, fast.back().timestamp);
  m_timestepData.fitLine[1] =
      ImPlotPoint{maxTime.value(), units::millisecond_t{dtMean}.value()};

  // RMSE = std::sqrt(sum((x_i - x^_i)^2) / N) where sum represents the sum of
  // all time series points, x_i represents the velocity at a timestep, x^_i
  // represents the prediction at the timestep, and N represents the number of
  // points
  m_RMSE = std::sqrt(simSquaredErrorSum / timeSeriesPoints);
  m_accelRSquared =
      1 - m_RMSE / std::sqrt(squaredVariationSum / timeSeriesPoints);
  FitPlots();
}

void AnalyzerPlot::FitPlots() {
  // Set the "fit" flag to true.
  m_quasistaticData.fitNextPlot = true;
  m_dynamicData.fitNextPlot = true;
  m_regressionData.fitNextPlot = true;
  m_timestepData.fitNextPlot = true;
}

double* AnalyzerPlot::GetSimRMSE() {
  return &m_RMSE;
}

double* AnalyzerPlot::GetSimRSquared() {
  return &m_accelRSquared;
}

static void PlotSimData(std::vector<std::vector<ImPlotPoint>>& data) {
  for (auto&& pts : data) {
    ImPlot::SetNextLineStyle(IMPLOT_AUTO_COL, 1.5);
    ImPlot::PlotLineG("Simulation", Getter, pts.data(), pts.size());
  }
}

bool AnalyzerPlot::DisplayPlots() {
  std::unique_lock lock(m_mutex, std::defer_lock);

  if (!lock.try_lock()) {
    ImGui::Text("Loading %c",
                "|/-\\"[static_cast<int>(ImGui::GetTime() / 0.05f) & 3]);
    return false;
  }

  ImVec2 plotSize = ImGui::GetContentRegionAvail();

  // Fit two plots horizontally
  plotSize.x = (plotSize.x - ImGui::GetStyle().ItemSpacing.x) / 2.f;

  // Fit two plots vertically while leaving room for three text boxes
  const float textBoxHeight = ImGui::GetFontSize() * 1.75;
  plotSize.y =
      (plotSize.y - textBoxHeight * 3 - ImGui::GetStyle().ItemSpacing.y) / 2.f;

  m_quasistaticData.Plot("Quasistatic Velocity vs. Time", plotSize,
                         m_velocityLabel.c_str(), m_pointSize);
  ImGui::SameLine();
  m_dynamicData.Plot("Dynamic Velocity vs. Time", plotSize,
                     m_velocityLabel.c_str(), m_pointSize);

  m_regressionData.Plot("Acceleration vs. Velocity", plotSize,
                        m_velocityLabel.c_str(), m_velPortionAccelLabel.c_str(),
                        true, true, m_pointSize);
  ImGui::SameLine();
  m_timestepData.Plot("Timesteps vs. Time", plotSize, "Time (s)",
                      "Timestep duration (ms)", true, false, m_pointSize,
                      [] { ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 50); });

  return true;
}

AnalyzerPlot::FilteredDataVsTimePlot::FilteredDataVsTimePlot() {
  rawData.reserve(kMaxSize);
  filteredData.reserve(kMaxSize);
  simData.reserve(kMaxSize);
}

void AnalyzerPlot::FilteredDataVsTimePlot::Plot(const char* title,
                                                const ImVec2& size,
                                                const char* yLabel,
                                                float pointSize) {
  // Generate Sim vs Filtered Plot
  if (fitNextPlot) {
    ImPlot::SetNextAxesToFit();
  }

  if (ImPlot::BeginPlot(title, size)) {
    ImPlot::SetupAxis(ImAxis_X1, "Time (s)", ImPlotAxisFlags_NoGridLines);
    ImPlot::SetupAxis(ImAxis_Y1, yLabel, ImPlotAxisFlags_NoGridLines);
    ImPlot::SetupLegend(ImPlotLocation_NorthEast);

    // Plot Raw Data
    ImPlot::SetNextMarkerStyle(IMPLOT_AUTO, 1, IMPLOT_AUTO_COL, 0);
    ImPlot::SetNextMarkerStyle(ImPlotStyleVar_MarkerSize, pointSize);
    ImPlot::PlotScatterG("Raw Data", Getter, rawData.data(), rawData.size());

    // Plot Filtered Data after Raw data
    ImPlot::SetNextMarkerStyle(IMPLOT_AUTO, 1, IMPLOT_AUTO_COL, 0);
    ImPlot::SetNextMarkerStyle(ImPlotStyleVar_MarkerSize, pointSize);
    ImPlot::PlotScatterG("Filtered Data", Getter, filteredData.data(),
                         filteredData.size());

    // Plot Simulation Data for Velocity Data
    PlotSimData(simData);

    // Disable constant resizing
    if (fitNextPlot) {
      fitNextPlot = false;
    }

    ImPlot::EndPlot();
  }
}

void AnalyzerPlot::FilteredDataVsTimePlot::Clear() {
  rawData.clear();
  filteredData.clear();
  simData.clear();
}

AnalyzerPlot::DataWithFitLinePlot::DataWithFitLinePlot() {
  data.reserve(kMaxSize);
}

void AnalyzerPlot::DataWithFitLinePlot::Plot(const char* title,
                                             const ImVec2& size,
                                             const char* xLabel,
                                             const char* yLabel, bool fitX,
                                             bool fitY, float pointSize,
                                             std::function<void()> setup) {
  if (fitNextPlot) {
    if (fitX && fitY) {
      ImPlot::SetNextAxesToFit();
    } else if (fitX && !fitY) {
      ImPlot::SetNextAxisToFit(ImAxis_X1);
    } else if (!fitX && fitY) {
      ImPlot::SetNextAxisToFit(ImAxis_Y1);
    }
  }

  if (ImPlot::BeginPlot(title, size)) {
    setup();
    ImPlot::SetupAxis(ImAxis_X1, xLabel, ImPlotAxisFlags_NoGridLines);
    ImPlot::SetupAxis(ImAxis_Y1, yLabel, ImPlotAxisFlags_NoGridLines);
    ImPlot::SetupLegend(ImPlotLocation_NorthEast);

    // Get a reference to the data that we are plotting.
    ImPlot::SetNextMarkerStyle(IMPLOT_AUTO, 1, IMPLOT_AUTO_COL, 0);
    ImPlot::SetNextMarkerStyle(ImPlotStyleVar_MarkerSize, pointSize);
    ImPlot::PlotScatterG("Filtered Data", Getter, data.data(), data.size());

    ImPlot::SetNextLineStyle(IMPLOT_AUTO_COL, 1.5);
    ImPlot::PlotLineG("Fit", Getter, fitLine.data(), fitLine.size());

    ImPlot::EndPlot();

    if (fitNextPlot) {
      fitNextPlot = false;
    }
  }
}

void AnalyzerPlot::DataWithFitLinePlot::Clear() {
  data.clear();

  // Reset line of best fit
  fitLine[0] = ImPlotPoint{0, 0};
  fitLine[1] = ImPlotPoint{0, 0};
}
