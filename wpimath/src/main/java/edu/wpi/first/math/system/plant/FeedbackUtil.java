// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.system.plant;

import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.controller.ControllerConstants;
import edu.wpi.first.math.controller.LinearQuadraticRegulator;
import edu.wpi.first.math.controller.ControllerConstants.FeedbackContants;
import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.numbers.N2;
import edu.wpi.first.math.system.LinearSystem;

public class FeedbackUtil {

  private FeedbackUtil() {
    // prevent instantiation
  }

  public static FeedbackContants createVelocitySystemFeedbackConstants(
      ControllerConstants.FeedforwardConstants feedforwardConstants) {

    if (!Double.isFinite(feedforwardConstants.kv()) || !Double.isFinite(feedforwardConstants.ka())) {
      return new FeedbackContants(0.0, 0.0, 0.0);
    }

    if (feedforwardConstants.ka() < 1e-7) {
      return new FeedbackContants(0.0, 0.0, 0.0);
    }

    LinearSystem<N1, N1, N1> plant = LinearSystemId.identifyVelocitySystem(feedforwardConstants.kv(),
        feedforwardConstants.ka());
    LinearQuadraticRegulator<N1, N1, N1> controller = new LinearQuadraticRegulator<>(
        plant,
        VecBuilder.fill(1.5),
        VecBuilder.fill(7.0),
        0.020);

        return new FeedbackContants(controller.getK().get(0, 0), 0.0, 0.0);
      }

  public static FeedbackContants createPositionSystemFeedbackConstants(
    ControllerConstants.FeedforwardConstants feedforwardConstants) {

  if (!Double.isFinite(feedforwardConstants.kv()) || !Double.isFinite(feedforwardConstants.ka())) {
    return new FeedbackContants(0.0, 0.0, 0.0);
  }

  if (feedforwardConstants.ka() < 1e-7) {
    return new FeedbackContants(0.0, 0.0, 0.0);
  }

  LinearSystem<N2, N1, N2> plant = LinearSystemId.identifyPositionSystem(feedforwardConstants.kv(),
      feedforwardConstants.ka());
  LinearQuadraticRegulator<N2, N1, N2> controller = new LinearQuadraticRegulator<>(
      plant,
      VecBuilder.fill(0.1, 1.5),
      VecBuilder.fill(7.0),
      0.020);

      return new FeedbackContants(controller.getK().get(0, 0), 0.0, controller.getK().get(0, 1));
    }

}
