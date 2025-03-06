package edu.wpi.first.wpilibj.simulation;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.Servo;
import org.junit.jupiter.api.Test;

class ServoSimTest {
  @Test
  void testServo() {
    HAL.initialize(500, 0);

    try (Servo servo = new Servo(0)) {
      ServoSim sim = new ServoSim(servo);

      servo.set(0);
      assertEquals(0, sim.getPosition());

      servo.set(0.354);
      assertEquals(0.354, sim.getPosition());

      servo.setAngle(10);
      assertEquals(10, sim.getAngle());

      servo.setAngle(90);
      assertEquals(90, sim.getAngle());

      servo.setAngle(170);
      assertEquals(170, sim.getAngle());
    }
  }
}
