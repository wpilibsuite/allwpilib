/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

namespace frc {

/**
 * Appends input and associated sign to array.
 *
 * @param input    The INode object to add to the array for round robin.
 * @param positive If true, adds input; otherwise, subtracts input.
 * @param inputs   The other INode objects and bools.
 */
template <class... INodes>
SumNode::SumNode(INode& input, bool positive, INodes&&... inputs)
    : SumNode(inputs...) {
  m_inputs.emplace_back(input, positive);
}

}  // namespace frc
