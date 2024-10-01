// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.system.plant;

import edu.wpi.first.math.controller.ControllerConstants;
import edu.wpi.first.math.controller.ControllerConstants.FeedforwardConstants;
import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.numbers.N2;
import edu.wpi.first.math.system.LinearSystem;

public class FeedforwardUtil {

  private FeedforwardUtil() {
    // prevent instantiation
  }

  public static FeedforwardConstants createVelocitySystemFeedforwardConstants(LinearSystem<N1, N1, N1> plant) {
    double ka = 1.0 / plant.getB(0, 0);
    double kv = -plant.getA(0, 0) * ka;
    return new ControllerConstants.FeedforwardConstants(0.0, kv, ka, 0.0);
  }

  public static FeedforwardConstants createPositionSystemFeedforwardConstants(LinearSystem<N2, N1, N2> plant) {
    double ka = 1.0 / plant.getB(1, 0);
    double kv = -plant.getA(1, 1) * ka;
    return new FeedforwardConstants(0.0, kv, ka, 0.0);
  }

  public static FeedforwardConstants createElevatorFeedforwardConstants(LinearSystem<N2, N1, N2> plant) {
    FeedforwardConstants constants = createPositionSystemFeedforwardConstants(plant);
    return new FeedforwardConstants(0.0, constants.kv(), constants.ka(), 9.8 * constants.ka());
  }

  public static FeedforwardConstants createArmFeedforwardConstants(LinearSystem<N2, N1, N2> plant,
      double armLengthMeters) {
    FeedforwardConstants constants = createPositionSystemFeedforwardConstants(plant);
    return new FeedforwardConstants(0.0, constants.kv(), constants.ka(),
        3.0 * 9.8 * constants.ka() / 2.0 / armLengthMeters);
  }

}
