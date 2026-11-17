// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/tunables/Selectable.hpp"

#include <string>

#include "wpi/tunables/Tunable.hpp"
#include "wpi/tunables/TunableConfig.hpp"
#include "wpi/tunables/TunableTable.hpp"

using namespace wpi;
using namespace wpi::tunables;

static constexpr const char* DEFAULT = "default";
static constexpr const char* OPTIONS = "options";
static constexpr const char* SELECTED = "selected";

detail::SelectableBase::SelectableBase() = default;

void detail::SelectableBase::PublishTunable(
    wpi::tunables::TunableTable& table) {
  table.Publish(DEFAULT, this, &SelectableBase::m_defaultChoice,
                TunableConfig{.isMutable = false});
  table.Publish(OPTIONS, this, &SelectableBase::m_options,
                TunableConfig{.isMutable = false});
  table.Publish(
      SELECTED, this, &SelectableBase::m_selected,
      TunableConfig{.robust = true,
                    .onTune = [](detail::TunableBase&, ComplexTunable* self) {
                      if (auto selectable =
                              static_cast<SelectableBase*>(self)) {
                        selectable->Changed(selectable->m_selected);
                      }
                    }});
}

std::string_view detail::SelectableBase::GetTunableType() const {
  return "Selectable";
}
