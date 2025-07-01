// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/tunable/Selectable.hpp"

#include "wpi/tunable/TunableConfig.hpp"
#include "wpi/tunable/TunableTable.hpp"

using namespace wpi;

static constexpr const char* kDefault = "default";
static constexpr const char* kOptions = "options";
static constexpr const char* kSelected = "selected";

detail::SelectableBase::SelectableBase()
    : m_defaultChoice{TunableConfig{.isMutable = false}},
      m_options{TunableConfig{.isMutable = false}},
      m_selected{TunableConfig{
          .robust = true,
          .onTune =
              [](TunableBase& tunable, ComplexTunable* self) {
                if (auto selectable = static_cast<SelectableBase*>(self)) {
                  auto& selected =
                      static_cast<wpi::Tunable<std::string>&>(tunable);
                  selectable->Changed(selected.Get());
                }
              },
          .parent = this}} {}

void detail::SelectableBase::PublishTunable(wpi::TunableTable& table) {
  table.Publish(kDefault, m_defaultChoice);
  table.Publish(kOptions, m_options);
  table.Publish(kSelected, m_selected);
}

std::string_view detail::SelectableBase::GetTunableType() const {
  return "Selectable";
}
