// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <tuple>
#include <vector>

#include "sysid/analysis/AnalysisType.h"
#include "sysid/analysis/OLS.h"
#include "sysid/analysis/Storage.h"

namespace sysid {

/**
 * Calculates feedforward gains given the data and the type of analysis to
 * perform.
 */
OLSResult CalculateFeedforwardGains(const Storage& data,
                                    const AnalysisType& type);

}  // namespace sysid
