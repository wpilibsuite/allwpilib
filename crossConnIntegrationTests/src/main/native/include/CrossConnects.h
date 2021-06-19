// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <array>
#include <utility>

namespace hlt {

constexpr static std::array<std::pair<int, int>, 22> DIOCrossConnects{
    std::pair{20, 25},
    std::pair{19, 24},
    std::pair{17, 13},
    std::pair{16, 12},
    std::pair{15, 11},
    std::pair{14, 10},
    std::pair{26, 2},
    std::pair{27, 1},
    std::pair{28, 0},
    std::pair{29, 3},
    std::pair{30, 4},

    // Opposite direction
    std::pair{25, 20},
    std::pair{24, 19},
    std::pair{13, 17},
    std::pair{12, 16},
    std::pair{11, 15},
    std::pair{10, 14},
    std::pair{2, 26},
    std::pair{1, 27},
    std::pair{0, 28},
    std::pair{3, 29},
    std::pair{4, 30},
};

// PWM on left, DIO on right
constexpr static std::array<std::pair<int, int>, 2> PWMCrossConnects{
    std::pair{0, 18},
    std::pair{16, 25},
};

// FWD only, relay on left
constexpr static std::array<std::pair<int, int>, 2> RelayAnalogCrossConnects{
    std::pair{2, 0}, std::pair{3, 1}};

struct RelayCross {
  int Relay;
  int FwdDio;
  int RevDio;
};

constexpr static std::array<RelayCross, 1> RelayCrossConnects{
    RelayCross{0, 23, 22}};

// input on left
constexpr static std::array<std::pair<int, int>, 2> AnalogCrossConnects{
    std::pair{2, 0}, std::pair{4, 1}};

}  // namespace hlt
