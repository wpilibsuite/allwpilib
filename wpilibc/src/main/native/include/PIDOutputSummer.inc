/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

namespace frc {

/**
 * Appends buffer and associated sign to array.
 *
 * @param output   The output for the sum.
 * @param buffer   The PIDOutputBuffer object to add to the array for round
 *                 robin.
 * @param positive If true, adds buffer output; otherwise, subtracts buffer
 *                 output.
 * @param buffers  The other PIDOutputBuffer objects and bools.
 */
template <class... PIDOutputBuffers>
PIDOutputSummer::PIDOutputSummer(PIDOutput& output, PIDOutputBuffer& buffer,
                                 bool positive, PIDOutputBuffers&&... buffers)
    : PIDOutputSummer(output, buffers...) {
  m_buffers.emplace_back(buffer, positive);
}

}  // namespace frc
