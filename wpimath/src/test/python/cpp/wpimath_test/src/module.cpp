
#include <stdexcept>

#include <module.h>

#include "semiwrap_init.wpimath_test._wpimath_test.hpp"

SEMIWRAP_PYBIND11_MODULE(m) {
  initWrapper(m);
}

bool SomeClass::checkDefaultByName1(wpi::units::second_t period) {
  if (period != SomeClass::ms_constant1) {
    throw std::runtime_error(wpi::units::to_string(period));
  }
  return true;
}

bool SomeClass::checkDefaultByName2(wpi::units::second_t period) {
  if (period != SomeClass::ms_constant2) {
    throw std::runtime_error(wpi::units::to_string(period));
  }
  return true;
}

bool SomeClass::checkDefaultByNum1(wpi::units::second_t period) {
  if (period != 50_ms) {
    throw std::runtime_error(wpi::units::to_string(period));
  }
  return true;
}

bool SomeClass::checkDefaultByNum2(wpi::units::second_t period) {
  if (period != 50_ms) {
    throw std::runtime_error(wpi::units::to_string(period));
  }
  return true;
}

wpi::units::meter_t SomeClass::ft2m(wpi::units::foot_t f) {
  return f;
}

wpi::units::second_t SomeClass::ms2s(wpi::units::millisecond_t ms) {
  return ms;
}

wpi::units::millisecond_t SomeClass::s2ms(wpi::units::second_t s) {
  return s;
}
