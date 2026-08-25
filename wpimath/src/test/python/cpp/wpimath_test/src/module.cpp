
#include <stdexcept>

#include <module.h>

#include "semiwrap_init.wpimath_test._wpimath_test.hpp"

SEMIWRAP_PYBIND11_MODULE(m) {
  initWrapper(m);
}

bool SomeClass::checkDefaultByName1(wpi::units::seconds<> period) {
  if (period != SomeClass::ms_constant1) {
    throw std::runtime_error(wpi::units::to_string(period));
  }
  return true;
}

bool SomeClass::checkDefaultByName2(wpi::units::seconds<> period) {
  if (period != SomeClass::ms_constant2) {
    throw std::runtime_error(wpi::units::to_string(period));
  }
  return true;
}

bool SomeClass::checkDefaultByNum1(wpi::units::seconds<> period) {
  if (period != 50_ms) {
    throw std::runtime_error(wpi::units::to_string(period));
  }
  return true;
}

bool SomeClass::checkDefaultByNum2(wpi::units::seconds<> period) {
  if (period != 50_ms) {
    throw std::runtime_error(wpi::units::to_string(period));
  }
  return true;
}

wpi::units::meters<> SomeClass::ft2m(wpi::units::feet<> f) {
  return f;
}

wpi::units::seconds<> SomeClass::ms2s(wpi::units::milliseconds<> ms) {
  return ms;
}

wpi::units::milliseconds<> SomeClass::s2ms(wpi::units::seconds<> s) {
  return s;
}
