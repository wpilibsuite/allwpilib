// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/codegen/CodeGen.hpp"

#include <cstdio>
#include <string>

namespace wpi::filterdesigner {

std::string FormatCoeff(double v) {
  char buf[32];
  std::snprintf(buf, sizeof(buf), "%.17g", v);
  return buf;
}

namespace {

std::string EmitCpp(const Sections& sections, std::string_view varName) {
  std::string out;
  out += "wpi::math::BiquadFilter ";
  out.append(varName);
  out += "{\n";
  for (const Section& s : sections) {
    out += "    {";
    out += FormatCoeff(s.b0);
    out += ", ";
    out += FormatCoeff(s.b1);
    out += ", ";
    out += FormatCoeff(s.b2);
    out += ", ";
    out += FormatCoeff(s.a1);
    out += ", ";
    out += FormatCoeff(s.a2);
    out += "},\n";
  }
  out += "};\n";
  return out;
}

std::string EmitPython(const Sections& sections, std::string_view varName) {
  std::string out;
  out += "from wpimath.filter import BiquadFilter\n\n";
  out.append(varName);
  out += " = BiquadFilter([\n";
  for (const Section& s : sections) {
    out += "    BiquadFilter.Section(b0=";
    out += FormatCoeff(s.b0);
    out += ", b1=";
    out += FormatCoeff(s.b1);
    out += ", b2=";
    out += FormatCoeff(s.b2);
    out += ", a1=";
    out += FormatCoeff(s.a1);
    out += ", a2=";
    out += FormatCoeff(s.a2);
    out += "),\n";
  }
  out += "])\n";
  return out;
}

std::string EmitJava(const Sections& sections, std::string_view varName) {
  std::string out;
  out += "BiquadFilter ";
  out.append(varName);
  out += " = new BiquadFilter(\n";
  for (size_t i = 0; i < sections.size(); ++i) {
    const Section& s = sections[i];
    out += "    new BiquadFilter.Section(";
    out += FormatCoeff(s.b0);
    out += ", ";
    out += FormatCoeff(s.b1);
    out += ", ";
    out += FormatCoeff(s.b2);
    out += ", ";
    out += FormatCoeff(s.a1);
    out += ", ";
    out += FormatCoeff(s.a2);
    out += ")";
    if (i + 1 < sections.size()) {
      out += ',';
    }
    out += '\n';
  }
  out += ");\n";
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
      return EmitCpp(sections, varName);
    case Language::Java:
      return EmitJava(sections, varName);
    case Language::Python:
      return EmitPython(sections, varName);
  }
  return {};
}

}  // namespace wpi::filterdesigner
