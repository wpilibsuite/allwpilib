// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <array>
#include <atomic>
#include <functional>
#include <string>
#include <string_view>
#include <vector>

#include <imgui.h>
#include <implot.h>
#include <units/time.h>
#include <wpi/Logger.h>
#include <wpi/spinlock.h>

#include "sysid/analysis/AnalysisManager.h"
#include "sysid/analysis/AnalysisType.h"
#include "sysid/analysis/Storage.h"

namespace sysid {
/**
 * Class that helps with plotting data in the analyzer view.
 */
class AnalyzerPlot {
 public:
  float m_pointSize = 1.25;
  // 0 for forward, 1 for reverse
  int m_direction = 0;

  /**
   * Constructs an instance of the analyzer plot helper and allocates memory for
   * all data vectors.
   *
   * @param logger The program logger
   */
  explicit AnalyzerPlot(wpi::Logger& logger);

  /**
   * Sets the data to be displayed on the plots.
   *
   * @param rawData      Raw data storage.
   * @param filteredData Filtered data storage.
   * @param unit         Unit of the dataset
   * @param ff           Feedforward gains (Ks, Kv, Ka, optionally
   *                     Kg and offset).
   * @param startTimes   Array of dataset start times.
   * @param type         Type of analysis.
   * @param abort        Aborts analysis early if set to true from another
   *                     thread.
   */
  void SetData(const Storage& rawData, const Storage& filteredData,
               std::string_view unit,
               const AnalysisManager::FeedforwardGains& ff,
               const std::array<units::second_t, 4>& startTimes,
               AnalysisType type, std::atomic<bool>& abort);

  /**
   * Utility method to plot the raw time series data
   *
   * @param rawSlow The raw slow (quasistatic) test data
   * @param rawFast The raw fast (dynamic) test data
   * @param abort   Aborts analysis early if set to true from another thread
   */
  void SetRawTimeData(const std::vector<PreparedData>& rawSlow,
                      const std::vector<PreparedData>& rawFast,
                      std::atomic<bool>& abort);

  /**
   * Utility method to reset everything before generating the points to plot.
   */
  void ResetData();

  /**
   * Utility method to get set the graph labels based off of the units
   *
   * @param unit Unit of the dataset
   */
  void SetGraphLabels(std::string_view unit);

  /**
   * Sets up only the raw time series data to be plotted. This is mainly
   * intended to be used if the filtered data has issues with it.
   *
   * @param data    The raw data.
   * @param unit    Unit of the dataset.
   * @param abort   Aborts analysis early if set to true from another thread.
   */
  void SetRawData(const Storage& data, std::string_view unit,
                  std::atomic<bool>& abort);

  /**
   * Displays time domain plots.
   *
   * @return Returns true if plots aren't in the loading state
   */
  bool DisplayPlots();

  /**
   * Sets certain flags to true so that the GUI automatically fits the plots
   */
  void FitPlots();

  /**
   * Gets the pointer to the stored Root Mean Squared Error for display
   *
   * @return A pointer to the RMSE
   */
  double* GetSimRMSE();

  /**
   * Gets the pointer to the stored simulated velocity R-squared for display
   *
   * @return A pointer to the R-squared
   */
  double* GetSimRSquared();

 private:
  // The maximum size of each vector (dataset to plot)
  static constexpr size_t kMaxSize = 2048;

  struct FilteredDataVsTimePlot {
    std::vector<ImPlotPoint> rawData;
    std::vector<ImPlotPoint> filteredData;

    // Simulated time domain data
    std::vector<std::vector<ImPlotPoint>> simData;

    // Stores whether this was the first call to Plot() since setting data
    bool fitNextPlot = false;

    FilteredDataVsTimePlot();

    /**
     * Plots data and fit line.
     *
     * @param title Plot title.
     * @param size Plot size.
     * @param yLabel Y axis label.
     * @param pointSize The size of the data point markers (in pixels).
     */
    void Plot(const char* title, const ImVec2& size, const char* yLabel,
              float pointSize);

    /**
     * Clears plot.
     */
    void Clear();
  };

  struct DataWithFitLinePlot {
    std::vector<ImPlotPoint> data;
    std::array<ImPlotPoint, 2> fitLine;

    // Stores whether this was the first call to Plot() since setting data
    bool fitNextPlot = false;

    DataWithFitLinePlot();

    /**
     * Plots data and fit line.
     *
     * @param title Plot title.
     * @param size Plot size.
     * @param xLabel X axis label.
     * @param yLabel Y axis label.
     * @param fitX True if X axis should be autofitted.
     * @param fitY True if Y axis should be autofitted.
     * @param pointSize The size of the data point markers (in pixels).
     * @param setup Callback within BeginPlot() block that performs custom plot
     *              setup.
     */
    void Plot(
        const char* title, const ImVec2& size, const char* xLabel,
        const char* yLabel, bool fitX, bool fitY, float pointSize,
        std::function<void()> setup = [] {});

    /**
     * Clears plot.
     */
    void Clear();
  };

  std::string m_velocityLabel;
  std::string m_accelerationLabel;
  std::string m_velPortionAccelLabel;

  // Thread safety
  wpi::spinlock m_mutex;

  // Logger
  wpi::Logger& m_logger;

  FilteredDataVsTimePlot m_quasistaticData;
  FilteredDataVsTimePlot m_dynamicData;
  DataWithFitLinePlot m_regressionData;
  DataWithFitLinePlot m_timestepData;

  double m_RMSE;
  double m_accelRSquared;
};
}  // namespace sysid
