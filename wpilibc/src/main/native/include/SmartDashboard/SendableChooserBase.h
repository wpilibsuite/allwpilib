/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>
#include <string>

#include "SmartDashboard/Sendable.h"
#include "tables/ITable.h"

namespace frc {

/**
 * This class is a non-template base class for {@link SendableChooser}.
 *
 * It contains static, non-templated variables to avoid their duplication in the
 * template class.
 */
class SendableChooserBase : public Sendable {
 public:
  virtual ~SendableChooserBase() = default;

  std::shared_ptr<ITable> GetTable() const override;
  std::string GetSmartDashboardType() const override;

 protected:
  static const char* kDefault;
  static const char* kOptions;
  static const char* kSelected;

  std::string m_defaultChoice;
  std::shared_ptr<ITable> m_table;
};

}  // namespace frc
