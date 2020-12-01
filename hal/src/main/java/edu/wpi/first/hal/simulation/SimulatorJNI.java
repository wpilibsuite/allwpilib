/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.hal.simulation;

import edu.wpi.first.hal.HAL;
import edu.wpi.first.hal.JNIWrapper;

public class SimulatorJNI extends JNIWrapper {
  public static native void setRuntimeType(int type);
  public static native void waitForProgramStart();
  public static native void setProgramStarted();
  public static native boolean getProgramStarted();
  public static native void restartTiming();
  public static native void pauseTiming();
  public static native void resumeTiming();
  public static native boolean isTimingPaused();
  public static native void stepTiming(long delta);
  public static native void stepTimingAsync(long delta);
  public static native void resetHandles();

  public static class SimPeriodicBeforeCallback implements AutoCloseable {
    private SimPeriodicBeforeCallback(Runnable r) {
      m_run = r;
    }

    @Override
    public void close() {
      synchronized (HAL.s_simPeriodicBefore) {
        HAL.s_simPeriodicBefore.remove(m_run);
      }
    }

    private Runnable m_run;
  }

  public static SimPeriodicBeforeCallback registerSimPeriodicBeforeCallback(Runnable r) {
    synchronized (HAL.s_simPeriodicBefore) {
      HAL.s_simPeriodicBefore.add(r);
    }
    return new SimPeriodicBeforeCallback(r);
  }

  public static class SimPeriodicAfterCallback implements AutoCloseable {
    private SimPeriodicAfterCallback(Runnable r) {
      m_run = r;
    }

    @Override
    public void close() {
      synchronized (HAL.s_simPeriodicAfter) {
        HAL.s_simPeriodicAfter.remove(m_run);
      }
    }

    private Runnable m_run;
  }

  public static SimPeriodicAfterCallback registerSimPeriodicAfterCallback(Runnable r) {
    synchronized (HAL.s_simPeriodicAfter) {
      HAL.s_simPeriodicAfter.add(r);
    }
    return new SimPeriodicAfterCallback(r);
  }
}
