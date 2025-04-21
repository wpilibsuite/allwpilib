// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once
#include "DCMotor.h"
#include "units/moment_of_inertia.h"

namespace frc {
    class WPILIB_DLLEXPORT Gearbox {
    public:
        DCMotor dcMotor;
        int numMotors;
        double reduction;
        units::kilogram_square_meter_t J;

        constexpr explicit Gearbox(const DCMotor& dcMotor,
                          const int numMotors = 1,
                          const double reduction = 1.0,
                          const units::kilogram_square_meter_t J = 1_kg_sq_m)
            : dcMotor(dcMotor), numMotors(numMotors), reduction(reduction), J(J) {
        }

        /**
         * Returns a copy of this gearbox with the given gearbox reduction applied.
         *
         * @param reduction  The gearbox reduction.
         */
        [[nodiscard]] constexpr Gearbox WithReduction(const double reduction) const {
            return Gearbox{dcMotor, numMotors, reduction, J};
        }
    };
}
