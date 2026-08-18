
#pragma once

#include "wpi/units/length.hpp"
#include "wpi/units/time.hpp"

struct SomeClass {
  static constexpr auto s_constant = 2_s;
  static constexpr auto ms_constant1 = 20_ms;
  static constexpr wpi::units::second_t ms_constant2 = 50_ms;
  static constexpr wpi::units::millisecond_t ms_constant3 = 0.20_s;

  bool checkDefaultByName1(wpi::units::second_t period = ms_constant1);
  bool checkDefaultByName2(wpi::units::second_t period = ms_constant2);
  bool checkDefaultByNum1(wpi::units::second_t period = 50_ms);
  bool checkDefaultByNum2(wpi::units::second_t period = 0.5_s);

  wpi::units::second_t ms2s(wpi::units::millisecond_t ms);
  wpi::units::millisecond_t s2ms(wpi::units::second_t s);

  static constexpr wpi::units::foot_t five_ft = 5_ft;

  wpi::units::meter_t ft2m(wpi::units::foot_t f);
};
