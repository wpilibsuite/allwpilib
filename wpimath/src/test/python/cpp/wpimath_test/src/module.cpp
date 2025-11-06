
#include "semiwrap_init.wpimath_test._wpimath_test.hpp"
#include <module.h>
#include <stdexcept>

SEMIWRAP_PYBIND11_MODULE(m)
{
    initWrapper(m);
}

bool SomeClass::checkDefaultByName1(units::second_t period)
{
    if (period != SomeClass::ms_constant1) {
        throw std::runtime_error(units::to_string(period));
    }
    return true;
}

bool SomeClass::checkDefaultByName2(units::second_t period)
{
    if (period != SomeClass::ms_constant2) {
        throw std::runtime_error(units::to_string(period));
    }
    return true;
}

bool SomeClass::checkDefaultByNum1(units::second_t period)
{
    if (period != 50_ms) {
        throw std::runtime_error(units::to_string(period));
    }
    return true;
}

bool SomeClass::checkDefaultByNum2(units::second_t period)
{
    if (period != 50_ms) {
        throw std::runtime_error(units::to_string(period));
    }
    return true;
}

units::meter_t SomeClass::ft2m(units::foot_t f) {
    return f;
}

units::second_t SomeClass::ms2s(units::millisecond_t ms) {
    return ms;
}

units::millisecond_t SomeClass::s2ms(units::second_t s) {
    return s;
}
