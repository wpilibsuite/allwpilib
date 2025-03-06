package edu.wpi.first.wpilibj.simulation;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.motorcontrol.Spark;
import org.junit.jupiter.api.Test;

class PWMMotorControllerSimTest {
  @Test
  void testMotor() {
    HAL.initialize(500, 0);

    try (Spark spark = new Spark(0)) {
      PWMMotorControllerSim sim = new PWMMotorControllerSim(spark);

      spark.set(0);
      assertEquals(0, sim.getSpeed());

      spark.set(0.354);
      assertEquals(0.354, sim.getSpeed());

      spark.set(-0.785);
      assertEquals(-0.785, sim.getSpeed());
    }
  }
}
