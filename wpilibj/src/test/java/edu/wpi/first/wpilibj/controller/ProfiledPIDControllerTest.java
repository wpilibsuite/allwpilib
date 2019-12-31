package edu.wpi.first.wpilibj.controller;

import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.api.Test;

import edu.wpi.first.wpilibj.trajectory.TrapezoidProfile;

public class ProfiledPIDControllerTest {

  @Test
  @SuppressWarnings("PMD")
  public void testStartFromNonZeroPosition() {
    ProfiledPIDController controller = new ProfiledPIDController(1.0, 0.0, 0.0,
        new TrapezoidProfile.Constraints(1.0, 1.0));

    controller.reset(20);

    Assertions.assertEquals(0.0, controller.calculate(20), 0.05);
  }

}
