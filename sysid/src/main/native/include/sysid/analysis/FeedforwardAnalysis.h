// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <tuple>
#include <vector>

#include "sysid/analysis/AnalysisType.h"
#include "sysid/analysis/Storage.h"

namespace sysid {

/**
 * Calculates feedforward gains given the data and the type of analysis to
 * perform.
 *
 * @return Tuple containing the coefficients of the analysis along with the
 *         r-squared (coefficient of determination) and RMSE (standard deviation
 * of the residuals) of the fit.
 */
std::tuple<std::vector<double>, double, double> CalculateFeedforwardGains(
    const Storage& data, const AnalysisType& type);
}  // namespace sysid
