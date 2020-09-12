/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "glass/View.h"

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
