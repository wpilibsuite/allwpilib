// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/nodes/ExportNodeLogic.hpp"

#include <filesystem>
#include <string>
#include <utility>

#include <fmt/format.h>

#include "wpi/filterdesigner/codegen/Export.hpp"
#include "wpi/filterdesigner/model/DesignedFilter.hpp"

namespace wpi::filterdesigner {

namespace {

std::string BuildDescription(const DesignedFilter& filter) {
  return fmt::format("Sample rate: {:g} Hz\nSections: {}\n", filter.sampleRate,
                     filter.sections.size());
}

}  // namespace

bool ExportNodeLogic::Export(const DesignedFilter* filter) {
  if (!filter || filter->sections.empty()) {
    lastOk = false;
    lastMessage = "No filter to export.";
    return false;
  }
  ExportSpec spec;
  spec.sampleRate = filter->sampleRate;
  spec.description = BuildDescription(*filter);

  ExportResult result = ExportFilter(filter->sections, lang, className,
                                     std::filesystem::path{projectRoot}, spec);
  lastOk = result.ok;
  lastMessage = std::move(result.message);
  return lastOk;
}

}  // namespace wpi::filterdesigner
