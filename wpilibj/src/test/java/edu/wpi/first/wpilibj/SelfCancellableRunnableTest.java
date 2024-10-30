// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotNull;

import edu.wpi.first.wpilibj.simulation.DriverStationSim;
import edu.wpi.first.wpilibj.simulation.SimHooks;
import java.util.concurrent.atomic.AtomicInteger;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.parallel.ResourceLock;

/**
 * Validates {@link SelfCancellableRunnable}
 *
 * <p>TODO(emintz): move TimedRobot.MockRobot into its own file.
 */
class SelfCancellableRunnableTest {

  static class MockRobot extends TimedRobot {
    static final double kPeriod = 0.02;

    public final AtomicInteger m_robotInitCount = new AtomicInteger(0);
    public final AtomicInteger m_simulationInitCount = new AtomicInteger(0);
    public final AtomicInteger m_disabledInitCount = new AtomicInteger(0);
    public final AtomicInteger m_autonomousInitCount = new AtomicInteger(0);
    public final AtomicInteger m_teleopInitCount = new AtomicInteger(0);
    public final AtomicInteger m_testInitCount = new AtomicInteger(0);

    public final AtomicInteger m_robotPeriodicCount = new AtomicInteger(0);
    public final AtomicInteger m_simulationPeriodicCount = new AtomicInteger(0);
    public final AtomicInteger m_disabledPeriodicCount = new AtomicInteger(0);
    public final AtomicInteger m_autonomousPeriodicCount = new AtomicInteger(0);
    public final AtomicInteger m_teleopPeriodicCount = new AtomicInteger(0);
    public final AtomicInteger m_testPeriodicCount = new AtomicInteger(0);

    public final AtomicInteger m_disabledExitCount = new AtomicInteger(0);
    public final AtomicInteger m_autonomousExitCount = new AtomicInteger(0);
    public final AtomicInteger m_teleopExitCount = new AtomicInteger(0);
    public final AtomicInteger m_testExitCount = new AtomicInteger(0);

    MockRobot() {
      super(kPeriod);

      m_robotInitCount.addAndGet(1);
    }

    @Override
    public void simulationInit() {
      m_simulationInitCount.addAndGet(1);
    }

    @Override
    public void disabledInit() {
      m_disabledInitCount.addAndGet(1);
    }

    @Override
    public void autonomousInit() {
      m_autonomousInitCount.addAndGet(1);
    }

    @Override
    public void teleopInit() {
      m_teleopInitCount.addAndGet(1);
    }

    @Override
    public void testInit() {
      m_testInitCount.addAndGet(1);
    }

    @Override
    public void robotPeriodic() {
      m_robotPeriodicCount.addAndGet(1);
    }

    @Override
    public void simulationPeriodic() {
      m_simulationPeriodicCount.addAndGet(1);
    }

    @Override
    public void disabledPeriodic() {
      m_disabledPeriodicCount.addAndGet(1);
    }

    @Override
    public void autonomousPeriodic() {
      m_autonomousPeriodicCount.addAndGet(1);
    }

    @Override
    public void teleopPeriodic() {
      m_teleopPeriodicCount.addAndGet(1);
    }

    @Override
    public void testPeriodic() {
      m_testPeriodicCount.addAndGet(1);
    }

    @Override
    public void disabledExit() {
      m_disabledExitCount.addAndGet(1);
    }

    @Override
    public void autonomousExit() {
      m_autonomousExitCount.addAndGet(1);
    }

    @Override
    public void teleopExit() {
      m_teleopExitCount.addAndGet(1);
    }

    @Override
    public void testExit() {
      m_testExitCount.addAndGet(1);
    }

    public static double period() {
      return kPeriod;
    }
  }

  private static class TestSelfCancellingRunnable
      extends SelfCancellableRunnable {

    private AtomicInteger m_invocationCount;
    private final int m_cancelAfter;

    private TestSelfCancellingRunnable(int cancelAfter) {
      m_cancelAfter = cancelAfter;
      m_invocationCount = new AtomicInteger(0);
    }

    @Override
    public void run() {
      if (m_invocationCount.get() < m_cancelAfter) {
        m_invocationCount.addAndGet(1);
      } else {
        cancel();
      }
    }

    private int invocationCount() {
      return m_invocationCount.get();
    }
  }

  private MockRobot m_robot;
  private Thread m_robotThread;

  @BeforeEach
  public void setup() {
    SimHooks.pauseTiming();
    DriverStationSim.resetData();
    m_robot = new MockRobot();
  }

  @AfterEach
  public void cleanup() {
    m_robot.endCompetition();
    try {
      m_robotThread.interrupt();
      m_robotThread.join();
    } catch (InterruptedException ex) {
      Thread.currentThread().interrupt();
    } finally {
      m_robot.close();
      SimHooks.resumeTiming();
    }
  }

  private void startSimulation() {
    m_robotThread = new Thread(m_robot::startCompetition);
    m_robotThread.start();
    DriverStationSim.setEnabled(false);
    DriverStationSim.notifyNewData();
    SimHooks.stepTiming(0.0); // Wait for Notifiers
  }

  @Test
  @ResourceLock("timing")
  void schedulePeriodicPeriodSecondsNoDelayOneIteration() {
    double timeStep = MockRobot.period() / 2.0;

    TestSelfCancellingRunnable callback =
        new TestSelfCancellingRunnable(1);
    assertNotNull(callback.schedulePeriodic(m_robot, timeStep));

    startSimulation();

    assertEquals(0, m_robot.m_disabledInitCount.get());
    assertEquals(0, m_robot.m_disabledPeriodicCount.get());
    assertEquals(0, callback.invocationCount());

    SimHooks.stepTiming(timeStep);

    assertEquals(0, m_robot.m_disabledInitCount.get());
    assertEquals(0, m_robot.m_disabledPeriodicCount.get());
    assertEquals(1,  callback.invocationCount());

    SimHooks.stepTiming(timeStep);

    assertEquals(1, m_robot.m_disabledInitCount.get());
    assertEquals(1, m_robot.m_disabledPeriodicCount.get());
    assertEquals(1,  callback.invocationCount());
  }

  @Test
  @ResourceLock("timing")
  void schedulePeriodicPeriodSecondsNoDelayTwoIterations() {
    double timeStep = MockRobot.period() / 2.0;

    TestSelfCancellingRunnable callback =
        new TestSelfCancellingRunnable(2);
    callback.schedulePeriodic(m_robot, timeStep);

    startSimulation();

    assertEquals(0, m_robot.m_disabledInitCount.get());
    assertEquals(0, m_robot.m_disabledPeriodicCount.get());
    assertEquals(0, callback.invocationCount());

    SimHooks.stepTiming(timeStep);

    assertEquals(0, m_robot.m_disabledInitCount.get());
    assertEquals(0, m_robot.m_disabledPeriodicCount.get());
    assertEquals(1, callback.invocationCount());

    SimHooks.stepTiming(timeStep);

    assertEquals(1, m_robot.m_disabledInitCount.get());
    assertEquals(1, m_robot.m_disabledPeriodicCount.get());
    assertEquals(2, callback.invocationCount());

    SimHooks.stepTiming(timeStep);

    assertEquals(1, m_robot.m_disabledInitCount.get());
    assertEquals(1, m_robot.m_disabledPeriodicCount.get());
    assertEquals(2, callback.invocationCount());

    SimHooks.stepTiming(timeStep);

    assertEquals(1, m_robot.m_disabledInitCount.get());
    assertEquals(2, m_robot.m_disabledPeriodicCount.get());
    assertEquals(2, callback.invocationCount());
  }

  @Test
  @ResourceLock("timing")
  void testSchedulePeriodicSecondsWithOffsetTwoIterations() {
    double periodSeconds = MockRobot.period() / 2.0;
    double offsetSeconds = MockRobot.period() / 4.0;
    double threeEightsSecond = MockRobot.period() * 3.0 / 8.0;
    double oneQuarterSecond = MockRobot.period() / 4.0;

    TestSelfCancellingRunnable callback =
        new TestSelfCancellingRunnable(2);
    callback.schedulePeriodic(m_robot, periodSeconds, offsetSeconds);

    // Expirations in this test (ms)
    //
    // Let p be period in ms.
    //
    // Robot | Callback
    // ================
    //     p |    0.75p
    //    2p |    1.25p

    startSimulation();


    assertEquals(0, m_robot.m_disabledInitCount.get());
    assertEquals(0, m_robot.m_disabledPeriodicCount.get());
    assertEquals(0, callback.invocationCount());

    SimHooks.stepTiming(threeEightsSecond);

    assertEquals(0, m_robot.m_disabledInitCount.get());
    assertEquals(0, m_robot.m_disabledPeriodicCount.get());
    assertEquals(0, callback.invocationCount());

    SimHooks.stepTiming(threeEightsSecond);

    assertEquals(0, m_robot.m_disabledInitCount.get());
    assertEquals(0, m_robot.m_disabledPeriodicCount.get());
    assertEquals(1, callback.invocationCount());

    SimHooks.stepTiming(oneQuarterSecond);

    assertEquals(1, m_robot.m_disabledInitCount.get());
    assertEquals(1, m_robot.m_disabledPeriodicCount.get());
    assertEquals(1, callback.invocationCount());

    SimHooks.stepTiming(oneQuarterSecond);

    assertEquals(1, m_robot.m_disabledInitCount.get());
    assertEquals(1, m_robot.m_disabledPeriodicCount.get());
    assertEquals(2, callback.invocationCount());

    SimHooks.stepTiming(oneQuarterSecond);

    assertEquals(1, m_robot.m_disabledInitCount.get());
    assertEquals(1, m_robot.m_disabledPeriodicCount.get());
    assertEquals(2, callback.invocationCount());

    SimHooks.stepTiming(oneQuarterSecond);

    assertEquals(1, m_robot.m_disabledInitCount.get());
    assertEquals(1, m_robot.m_disabledPeriodicCount.get());
    assertEquals(2, callback.invocationCount());
  }

  @Test
  void testSchedulePeriodic1() {
  }
}
