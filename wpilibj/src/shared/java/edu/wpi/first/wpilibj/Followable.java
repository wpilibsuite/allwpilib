/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2017. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

/**
 * A follower will let {@code SpeedController}s of type T to follow its state.
 *
 * @param <T> The type that can follow this Object
 */
public interface Followable<T extends SpeedController> extends SpeedController {

  /**
   * Add an Object to follow this Followable's state.
   *
   * @param follower The follower to add
   */
  void addFollower(T follower);

}
