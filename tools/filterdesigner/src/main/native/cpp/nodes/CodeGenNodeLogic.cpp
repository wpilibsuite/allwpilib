// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/nodes/CodeGenNodeLogic.hpp"

#include "wpi/filterdesigner/codegen/CodeGen.hpp"
#include "wpi/filterdesigner/model/DesignedFilter.hpp"

namespace wpi::filterdesigner {

std::string CodeGenNodeLogic::Generate(const DesignedFilter* filter) const {
  if (!filter || filter->sections.empty()) {
    return {};
  }
  return EmitCode(filter->sections, lang, varName);
}

}  // namespace wpi::filterdesigner
