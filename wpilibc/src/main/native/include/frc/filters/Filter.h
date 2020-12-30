// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include <wpi/deprecated.h>

#include "frc/PIDSource.h"

namespace frc {

/**
 * Interface for filters
 *
 * @deprecated only used by the deprecated LinearDigitalFilter
 */
class Filter : public PIDSource {
 public:
  WPI_DEPRECATED("This class is no longer used.")
  explicit Filter(PIDSource& source);
  WPI_DEPRECATED("This class is no longer used.")
  explicit Filter(std::shared_ptr<PIDSource> source);
  ~Filter() override = default;

  Filter(Filter&&) = default;
  Filter& operator=(Filter&&) = default;

  // PIDSource interface
  void SetPIDSourceType(PIDSourceType pidSource) override;
  PIDSourceType GetPIDSourceType() const override;
  double PIDGet() override = 0;

  /**
   * Returns the current filter estimate without also inserting new data as
   * PIDGet() would do.
   *
   * @return The current filter estimate
   */
  virtual double Get() const = 0;

  /**
   * Reset the filter state
   */
  virtual void Reset() = 0;

 protected:
  /**
   * Calls PIDGet() of source
   *
   * @return Current value of source
   */
  double PIDGetSource();

 private:
  std::shared_ptr<PIDSource> m_source;
};

}  // namespace frc
