/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <functional>
#include <memory>
#include <mutex>

#include "INode.h"

namespace frc {

class Output;

/**
 * A node for reference inputs (e.g., setpoints).
 */
class RefInput : public INode {
 public:
  explicit RefInput(double reference = 0.0);
  virtual ~RefInput() = default;

  void SetCallback(Output& output) override;

  double GetOutput() override;
  double GetOutput() const;

  void Set(double reference);

 private:
  double m_reference;
  std::function<void()> m_callback = [] {};

  mutable std::mutex m_mutex;
};

}  // namespace frc
