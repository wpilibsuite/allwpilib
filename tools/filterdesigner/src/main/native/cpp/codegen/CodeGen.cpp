// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/codegen/CodeGen.hpp"

#include <string>
#include <string_view>

#include <fmt/format.h>

#include "CodeGenLines.hpp"

namespace wpi::filterdesigner {

using detail::Join;
using detail::Lines;

namespace {

Lines EmitCpp(const Sections& sections, std::string_view varName) {
  Lines out;
  out.push_back(fmt::format("wpi::math::BiquadFilter {}{{", varName));
  for (const Section& s : sections) {
    out.push_back(
        fmt::format("    {{{:.17g}, {:.17g}, {:.17g}, {:.17g}, {:.17g}}},",
                    s.b0, s.b1, s.b2, s.a1, s.a2));
  }
  out.emplace_back("};");
  return out;
}

Lines EmitPython(const Sections& sections, std::string_view varName) {
  Lines out;
  out.emplace_back("from wpimath.filter import BiquadFilter");
  out.emplace_back("");
  out.push_back(fmt::format("{} = BiquadFilter([", varName));
  for (const Section& s : sections) {
    out.push_back(fmt::format(
        "    BiquadFilter.Section(b0={:.17g}, b1={:.17g}, b2={:.17g}, "
        "a1={:.17g}, a2={:.17g}),",
        s.b0, s.b1, s.b2, s.a1, s.a2));
  }
  out.emplace_back("])");
  return out;
}

Lines EmitJava(const Sections& sections, std::string_view varName) {
  Lines out;
  out.push_back(fmt::format("BiquadFilter {} = new BiquadFilter(", varName));
  for (size_t i = 0; i < sections.size(); ++i) {
    const Section& s = sections[i];
    std::string_view sep = (i + 1 < sections.size()) ? "," : "";
    out.push_back(fmt::format(
        "    new BiquadFilter.Section({:.17g}, {:.17g}, {:.17g}, {:.17g}, "
        "{:.17g}){}",
        s.b0, s.b1, s.b2, s.a1, s.a2, sep));
  }
  out.emplace_back(");");
  return out;
}

}  // namespace

std::string EmitCode(const Sections& sections, Language lang,
                     std::string_view varName) {
  if (sections.empty()) {
    return {};
  }
  switch (lang) {
    case Language::Cpp:
      return Join(EmitCpp(sections, varName));
    case Language::Java:
      return Join(EmitJava(sections, varName));
    case Language::Python:
      return Join(EmitPython(sections, varName));
  }
  return {};
}

}  // namespace wpi::filterdesigner
