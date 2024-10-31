// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <string_view>

#include "sysid/analysis/AnalysisType.h"
#include "sysid/analysis/OLS.h"
#include "sysid/analysis/Storage.h"

namespace sysid {

/**
 * Exception for data that doesn't sample enough of the state-input space.
 */
class InsufficientSamplesError : public std::exception {
 public:
  /**
   * Constructs an InsufficientSamplesError.
   *
   * @param message The error message
   */
  explicit InsufficientSamplesError(std::string_view message) {
    m_message = message;
  }

  const char* what() const noexcept override { return m_message.c_str(); }

 private:
  /**
   * Stores the error message
   */
  std::string m_message;
};

/**
 * Calculates feedforward gains given the data and the type of analysis to
 * perform.
 *
 * @param data The OLS input data.
 * @param type The analysis type.
 * @param throwOnRankDeficiency Whether to throw if the fit is going to be poor.
 *   This option is provided for unit testing purposes.
 */
OLSResult CalculateFeedforwardGains(const Storage& data,
                                    const AnalysisType& type,
                                    bool throwOnRankDeficiency = true);

}  // namespace sysid
