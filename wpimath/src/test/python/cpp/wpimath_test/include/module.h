
#pragma once

#include "wpi/units/length.hpp"
#include "wpi/units/time.hpp"

struct SomeClass {
  static constexpr auto s_constant = 2_s;
  static constexpr auto ms_constant1 = 20_ms;
  static constexpr wpi::units::seconds<> ms_constant2 = 50_ms;
  static constexpr wpi::units::milliseconds<> ms_constant3 = 0.20_s;

  bool checkDefaultByName1(wpi::units::seconds<> period = ms_constant1);
  bool checkDefaultByName2(wpi::units::seconds<> period = ms_constant2);
  bool checkDefaultByNum1(wpi::units::seconds<> period = 50_ms);
  bool checkDefaultByNum2(wpi::units::seconds<> period = 0.5_s);

  wpi::units::seconds<> ms2s(wpi::units::milliseconds<> ms);
  wpi::units::milliseconds<> s2ms(wpi::units::seconds<> s);

  static constexpr wpi::units::feet<> five_ft = 5_ft;

  wpi::units::meters<> ft2m(wpi::units::feet<> f);
};
