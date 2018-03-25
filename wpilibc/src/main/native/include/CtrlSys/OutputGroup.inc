/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

namespace frc {

/**
 * Appends output to array.
 *
 * @param output the Output object to add to the array for round robin
 * @param outputs the other Output objects
 */
template <class... Outputs>
OutputGroup::OutputGroup(Output& output, Outputs&&... outputs)
    : OutputGroup(outputs...) {
  m_outputs.emplace_back(output);
}

}  // namespace frc
