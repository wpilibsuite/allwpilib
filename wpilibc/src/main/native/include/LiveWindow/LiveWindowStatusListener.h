/*----------------------------------------------------------------------------*/
/* Copyright (c) 2012-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>

#include "tables/ITable.h"
#include "tables/ITableListener.h"

namespace frc {

class LiveWindowStatusListener : public ITableListener {
 public:
  virtual void ValueChanged(ITable* source, llvm::StringRef key,
                            std::shared_ptr<nt::Value> value, bool isNew);
};

}  // namespace frc
