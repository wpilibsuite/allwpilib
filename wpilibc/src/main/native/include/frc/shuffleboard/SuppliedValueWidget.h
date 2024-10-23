// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <memory>
#include <string>
#include <string_view>

#include <networktables/BooleanTopic.h>
#include <networktables/GenericEntry.h>
#include <networktables/NetworkTable.h>

#include "frc/shuffleboard/ShuffleboardComponent.h"
#include "frc/shuffleboard/ShuffleboardComponentBase.h"
#include "frc/shuffleboard/ShuffleboardWidget.h"

namespace frc {
class ShuffleboardContainer;

template <typename T>
class SuppliedValueWidget : public ShuffleboardWidget<SuppliedValueWidget<T>> {
 public:
  SuppliedValueWidget(ShuffleboardContainer& parent, std::string_view title,
                      std::string_view typeString, std::function<T()> supplier,
                      std::function<void(nt::GenericPublisher&, T)> setter)
      : ShuffleboardValue(title),
        ShuffleboardWidget<SuppliedValueWidget<T>>(parent, title),
        m_typeString(typeString),
        m_supplier(supplier),
        m_setter(setter) {}

  void BuildInto(std::shared_ptr<nt::NetworkTable> parentTable,
                 std::shared_ptr<nt::NetworkTable> metaTable) override {
    this->BuildMetadata(metaTable);
    if (!m_controllablePub) {
      m_controllablePub =
          nt::BooleanTopic{metaTable->GetTopic("Controllable")}.Publish();
      m_controllablePub.Set(false);
    }

    if (!m_entry) {
      m_entry =
          parentTable->GetTopic(this->GetTitle()).GenericPublish(m_typeString);
    }
    m_setter(m_entry, m_supplier());
  }

 private:
  std::string m_typeString;
  std::function<T()> m_supplier;
  std::function<void(nt::GenericPublisher&, T)> m_setter;
  nt::BooleanPublisher m_controllablePub;
  nt::GenericPublisher m_entry;
};
}  // namespace frc
