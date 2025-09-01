#pragma once

#include <pybind11/pybind11.h>

#include "hal/DriverStationTypes.h"

namespace pybind11 {

template <>
struct format_descriptor<HAL_JoystickPOV> {
    static constexpr const char c = 'B';
    static constexpr const char value[2] = {c, '\0'};
    static std::string format() { return std::string(1, c); }
};

}