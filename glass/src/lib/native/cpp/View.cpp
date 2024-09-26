// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/View.h"

#include <memory>
#include <utility>

using namespace glass;

namespace {
class FunctionView : public View {
 public:
  explicit FunctionView(wpi::unique_function<void()> display)
      : m_display(std::move(display)) {}

  void Display() override { m_display(); }

 private:
  wpi::unique_function<void()> m_display;
};
}  // namespace

std::unique_ptr<View> glass::MakeFunctionView(
    wpi::unique_function<void()> display) {
  return std::make_unique<FunctionView>(std::move(display));
}

void View::Hidden() {}

void View::Settings() {}

bool View::HasSettings() {
  return false;
}
