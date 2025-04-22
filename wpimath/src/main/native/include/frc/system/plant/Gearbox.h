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

        /**
         * Create a gearbox with the given DCMotor, number of motors, gearing
         * reduction, and moment of inertia.
         *
         * @param dcMotor The DC motor attached to the gearbox.
         * @param numMotors The number of motors in the gearbox.
         * @param reduction The gearing reduction of the gearbox.
         * @param J The moment of inertia J of the gearbox.
         * @throws std::domain_error if J <= 0 or gearing <= 0.
         */
        constexpr explicit Gearbox(const DCMotor &dcMotor, const int numMotors = 1,
                                   const double reduction = 1.0,
                                   const units::kilogram_square_meter_t J = 1_kg_sq_m)
            : dcMotor(dcMotor), numMotors(numMotors), reduction(reduction), J(J) {
            if (J <= 0_kg_sq_m) {
                throw std::domain_error("J must be greater than zero.");
            }
            if (reduction <= 0.0) {
                throw std::domain_error("gearing must be greater than zero.");
            }
        }

        /**
         * Returns a copy of this gearbox with the given gearbox reduction applied.
         *
         * @param reduction  The gearbox reduction.
         */
        [[nodiscard]]
        constexpr Gearbox WithReduction(const double reduction) const {
            return Gearbox{dcMotor, numMotors, reduction, J};
        }

        /**
         * Returns current drawn by gearbox motors with given speed and input voltage.
         *
         * @param speed        The current angular velocity of the gearbox.
         * @param inputVoltage The voltage being applied to the gearbox motors.
         */
        [[nodiscard]]
        constexpr units::newton_meter_t Acceleration(
            units::radians_per_second_t speed, units::volt_t inputVoltage) const {
            return units::newton_meter_t{
                numMotors * reduction *
                dcMotor.Torque(speed * reduction, inputVoltage).value() / J.value()
            };
        }

        /**
         * Returns torque produced by the gearbox with a given current.
         *
         * @param current     The current drawn by the gearbox motors.
         */
        [[nodiscard]]
        constexpr units::newton_meter_t Acceleration(units::ampere_t current) const {
            return units::newton_meter_t
                    {numMotors * reduction * dcMotor.Torque(current / numMotors).value() / J.value()};
        }

        /**
         * Returns current drawn by gearbox motors with given speed and input voltage.
         *
         * @param speed        The current angular velocity of the gearbox.
         * @param inputVoltage The voltage being applied to the gearbox motors.
         */
        [[nodiscard]]
        constexpr units::ampere_t Current(units::radians_per_second_t speed,
                                          units::volt_t inputVoltage) const {
            return numMotors * dcMotor.Current(speed * reduction, inputVoltage);
        }

        /**
         * Returns current drawn by gearbox motors for a given torque.
         *
         * @param torque The torque produced by the gearbox.
         */
        [[nodiscard]]
        constexpr units::ampere_t Current(units::newton_meter_t torque) const {
            return numMotors * dcMotor.Current(torque / numMotors / reduction);
        }

        /**
         * Returns torque produced by the gearbox with given speed and input voltage.
         *
         * @param speed        The current angular velocity of the gearbox.
         * @param inputVoltage The voltage being applied to the gearbox motors.
         */
        [[nodiscard]]
        constexpr units::newton_meter_t Torque(units::radians_per_second_t speed,
                                               units::volt_t inputVoltage) const {
            return numMotors * reduction *
                   dcMotor.Torque(speed * reduction, inputVoltage);
        }

        /**
         * Returns torque produced by the gearbox with a given current.
         *
         * @param current     The current drawn by the gearbox motors.
         */
        [[nodiscard]]
        constexpr units::newton_meter_t Torque(units::ampere_t current) const {
            return numMotors * reduction * dcMotor.Torque(current / numMotors);
        }

        /**
         * Returns the voltage provided to the gearbox motors for a given torque and
         * angular velocity.
         *
         * @param torque      The torque produced by the gearbox.
         * @param speed       The current angular velocity of the gearbox.
         */
        [[nodiscard]]
        constexpr units::volt_t Voltage(units::newton_meter_t torque,
                                        units::radians_per_second_t speed) const {
            return dcMotor.Voltage(torque / numMotors / reduction, speed * reduction);
        }

        /**
         * Returns the voltage provided to the gearbox motors for a given current and
         * angular velocity.
         *
         * @param current     The current drawn by the gearbox motors.
         * @param speed       The current angular velocity of the gearbox.
         */
        [[nodiscard]]
        constexpr units::volt_t Voltage(units::ampere_t current,
                                        units::radians_per_second_t speed) const {
            return dcMotor.Voltage(current / numMotors, speed * reduction);
        }

        /**
         * Returns the angular speed produced by the gearbox at a given torque and
         * input voltage.
         *
         * @param torque        The torque produced by the gearbox.
         * @param inputVoltage  The input voltage provided to the gearbox motors.
         */
        [[nodiscard]]
        constexpr units::radians_per_second_t Speed(
            units::newton_meter_t torque, units::volt_t inputVoltage) const {
            return dcMotor.Speed(torque / numMotors / reduction, inputVoltage) /
                   reduction;
        }

        /**
         * Returns the angular speed produced by the gearbox at a given torque and
         * input voltage.
         *
         * @param current     The current drawn by the gearbox motors.
         * @param inputVoltage  The input voltage provided to the gearbox motors.
         */
        [[nodiscard]]
        constexpr units::radians_per_second_t Speed(
            units::ampere_t current, units::volt_t inputVoltage) const {
            return dcMotor.Speed(current / numMotors, inputVoltage) / reduction;
        }
    };
} // namespace frc
