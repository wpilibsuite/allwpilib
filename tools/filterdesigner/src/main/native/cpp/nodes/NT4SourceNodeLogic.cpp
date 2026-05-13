// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/nodes/NT4SourceNodeLogic.hpp"

#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace wpi::filterdesigner {

NT4SourceNodeLogic::NT4SourceNodeLogic()
    : m_source{[this]() -> std::vector<NT4Source::Sample> {
        if (m_drain) {
          return m_drain();
        }
        return {};
      }} {}

void NT4SourceNodeLogic::SetDrain(NT4Source::DrainFn fn) {
  m_drain = std::move(fn);
}

void NT4SourceNodeLogic::Update() {
  m_source.Update();
}

const Signal* NT4SourceNodeLogic::Signal() const {
  // Mirror the WpiLogSource / NT4SourceView contract: a non-null pointer
  // means there's actually data downstream nodes can render. Returning the
  // raw buffer pointer with zero samples would force every sink to gate on
  // size() instead.
  if (m_source.SampleCount() == 0) {
    return nullptr;
  }
  return m_source.GetSignal();
}

void NT4SourceNodeLogic::Clear() {
  m_source.Clear();
}

void NT4SourceNodeLogic::SetTopicName(std::string_view name) {
  m_topicName = std::string{name};
  m_source.SetName(m_topicName);
}

}  // namespace wpi::filterdesigner
