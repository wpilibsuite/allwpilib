package edu.wpi.first.wpilibj;

import static org.junit.Assert.assertEquals;

import edu.wpi.first.wpilibj.test.AbstractComsSetup;
import java.util.logging.Logger;
import org.junit.Test;

public class NotifierTest extends AbstractComsSetup {

  private static final Logger logger = Logger.getLogger(NotifierTest.class.getName());

  private static class LoopTimer implements Runnable {

    private long m_lastLoopTime;
    private long m_loopCount;
    private double m_average;

    @Override
    public void run() {
      long currentTime = RobotController.getFPGATime();
      long loopTime = currentTime - m_lastLoopTime;
      m_lastLoopTime = currentTime;
      if (m_lastLoopTime != 0) {
        m_average = ((m_average * m_loopCount) + loopTime) / ++m_loopCount;
      }
    }

    public double getAverageLoopTime() {
      return m_average;
    }
  }

  @Override
  protected Logger getClassLogger() {
    return logger;
  }

  @Test
  public void loopTest() throws InterruptedException {
    LoopTimer loopTimer = new LoopTimer();
    Notifier notifier = new Notifier(loopTimer);

    notifier.startPeriodic(1.0 / 200.0);
    Thread.sleep(5000);

    double time = loopTimer.getAverageLoopTime();
    assertEquals("Notifier out of time! Expected 5000; Got: " + time, 5000, time, 200);
  }

}
