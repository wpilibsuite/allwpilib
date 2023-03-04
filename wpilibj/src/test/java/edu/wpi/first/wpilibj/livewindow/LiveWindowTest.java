// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.livewindow;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertNotEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.networktables.BooleanSubscriber;
import edu.wpi.first.networktables.DoublePublisher;
import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.NetworkTableInstance;
import edu.wpi.first.networktables.TimestampedBoolean;
import edu.wpi.first.wpilibj.UtilityClassTest;
import edu.wpi.first.wpilibj.motorcontrol.PWMSparkMax;
import edu.wpi.first.wpilibj.simulation.DriverStationSim;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;
import java.util.concurrent.atomic.AtomicInteger;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.ValueSource;

class LiveWindowTest extends UtilityClassTest<LiveWindow> {
  private final NetworkTable m_smartDashboardTable =
      NetworkTableInstance.getDefault().getTable("SmartDashboard");
  private final NetworkTable m_liveWindowTable =
      NetworkTableInstance.getDefault().getTable("LiveWindow");

  LiveWindowTest() {
    super(LiveWindow.class);
  }

  @Test
  void enableDisableCallbacks() {
    AtomicInteger counter = new AtomicInteger(0);
    try (BooleanSubscriber subscriber =
        m_liveWindowTable.getSubTable(".status").getBooleanTopic("LW Enabled").subscribe(false)) {
      LiveWindow.setEnabled(false);
      LiveWindow.setEnabledListener(counter::incrementAndGet);
      LiveWindow.setDisabledListener(counter::decrementAndGet);

      assertEquals(0, counter.get());
      assertFalse(subscriber.get());

      LiveWindow.setEnabled(true);
      assertEquals(1, counter.get());
      assertTrue(subscriber.get());

      LiveWindow.setEnabled(false);
      assertEquals(0, counter.get());
      assertFalse(subscriber.get());
    }
  }

  @ValueSource(booleans = {false, true})
  @ParameterizedTest
  void controlPwm(boolean lwEnabled) {
    SmartDashboard.setNetworkTableInstance(NetworkTableInstance.getDefault());
    LiveWindow.setEnabled(false);
    DriverStationSim.setEnabled(true);
    DriverStationSim.notifyNewData();
    NetworkTable pwmTable = m_smartDashboardTable.getSubTable("MyPWM");
    try (PWMSparkMax pwm = new PWMSparkMax(0);
        BooleanSubscriber controllable =
            pwmTable.getBooleanTopic(".controllable").subscribe(false);
        DoublePublisher pwmValue = pwmTable.getDoubleTopic("Value").publish();
        BooleanSubscriber lwControllable =
            m_liveWindowTable
                .getSubTable("Ungrouped")
                .getSubTable("PWMSparkMax[0]")
                .getBooleanTopic(".controllable")
                .subscribe(false)) {
      // Shouldn't be published yet
      assertEquals(0, controllable.getAtomic().timestamp);
      assertEquals(0, lwControllable.getAtomic().timestamp);

      SmartDashboard.putData("MyPWM", pwm);
      SmartDashboard.updateValues();
      LiveWindow.updateValues();

      // Should be published now
      TimestampedBoolean publishedValue = controllable.getAtomic();
      assertNotEquals(0, publishedValue.timestamp, 1e-9);
      assertFalse(publishedValue.value);

      assertEquals(0, lwControllable.getAtomic().timestamp);

      LiveWindow.setEnabled(lwEnabled);

      pwmValue.set(0.5);
      SmartDashboard.updateValues();
      LiveWindow.updateValues();

      // LiveWindow doesn't change `.controllable` in place!
      assertFalse(controllable.get());

      // Instead, LiveWindow publishes everything to the LiveWindow table
      TimestampedBoolean lwControllableTimestamped = lwControllable.getAtomic();
      if (lwEnabled) {
        assertNotEquals(0, lwControllableTimestamped.timestamp);
        assertTrue(lwControllableTimestamped.value);
      } else {
        assertEquals(0, lwControllableTimestamped.timestamp);
      }

      assertEquals(lwEnabled ? 0.5 : 0, pwm.get());
    }
  }
}
