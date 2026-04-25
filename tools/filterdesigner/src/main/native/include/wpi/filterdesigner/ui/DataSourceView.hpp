// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/filterdesigner/io/NT4SourceView.hpp"
#include "wpi/filterdesigner/io/WpiLogLoaderView.hpp"
#include "wpi/filterdesigner/model/Signal.hpp"

namespace wpi::filterdesigner {

/**
 * Tabbed container over the WPILOG and live-NT4 sources, exposing whichever
 * tab is currently active as a single @ref SelectedSignal. Downstream views
 * don't need to know which source is in use.
 */
class DataSourceView {
 public:
  /** Which data source the user most recently interacted with. */
  enum class Mode { WpiLog, Nt4 };

  void Display();

  /** Returns the signal for the active tab, or nullptr if none selected. */
  const Signal* SelectedSignal() const;

  Mode ActiveMode() const { return m_mode; }

 private:
  WpiLogLoaderView m_logLoader;
  NT4SourceView m_nt4;
  Mode m_mode = Mode::WpiLog;
};

}  // namespace wpi::filterdesigner
