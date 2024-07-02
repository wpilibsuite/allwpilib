// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.livewindow;

import edu.wpi.first.networktables.BooleanPublisher;
import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.NetworkTableInstance;
import edu.wpi.first.networktables.StringPublisher;
import edu.wpi.first.networktables.StringTopic;
import edu.wpi.first.util.sendable.Sendable;
import edu.wpi.first.util.sendable.SendableRegistry;
import edu.wpi.first.wpilibj.smartdashboard.SendableBuilderImpl;

/**
 * The LiveWindow class is the public interface for putting sensors and actuators on the LiveWindow.
 */
public final class LiveWindow {
  private static final class Component implements AutoCloseable {
    @Override
    public void close() {
      if (m_namePub != null) {
        m_namePub.close();
        m_namePub = null;
      }
      if (m_typePub != null) {
        m_typePub.close();
        m_typePub = null;
      }
    }

    boolean m_firstTime = true;
    boolean m_telemetryEnabled;
    StringPublisher m_namePub;
    StringPublisher m_typePub;
  }

  private static final String kSmartDashboardType = "LW Subsystem";

  private static final int dataHandle = SendableRegistry.getDataHandle();
  private static final NetworkTable liveWindowTable =
      NetworkTableInstance.getDefault().getTable("LiveWindow");
  private static final NetworkTable statusTable = liveWindowTable.getSubTable(".status");
  private static final BooleanPublisher enabledPub =
      statusTable.getBooleanTopic("LW Enabled").publish();
  private static boolean startLiveWindow;
  private static boolean liveWindowEnabled;
  private static boolean telemetryEnabled;

  private static Runnable enabledListener;
  private static Runnable disabledListener;

  static {
    SendableRegistry.setLiveWindowBuilderFactory(SendableBuilderImpl::new);
    enabledPub.set(false);
  }

  private static Component getOrAdd(Sendable sendable) {
    Component data = (Component) SendableRegistry.getData(sendable, dataHandle);
    if (data == null) {
      data = new Component();
      SendableRegistry.setData(sendable, dataHandle, data);
    }
    return data;
  }

  private LiveWindow() {
    throw new UnsupportedOperationException("This is a utility class!");
  }

  /**
   * Sets function to be called when LiveWindow is enabled.
   *
   * @param runnable function (or null for none)
   */
  public static synchronized void setEnabledListener(Runnable runnable) {
    enabledListener = runnable;
  }

  /**
   * Sets function to be called when LiveWindow is disabled.
   *
   * @param runnable function (or null for none)
   */
  public static synchronized void setDisabledListener(Runnable runnable) {
    disabledListener = runnable;
  }

  /**
   * Returns true if LiveWindow is enabled.
   *
   * @return True if LiveWindow is enabled.
   */
  public static synchronized boolean isEnabled() {
    return liveWindowEnabled;
  }

  /**
   * Set the enabled state of LiveWindow.
   *
   * <p>If it's being enabled, turn off the scheduler and remove all the commands from the queue and
   * enable all the components registered for LiveWindow. If it's being disabled, stop all the
   * registered components and re-enable the scheduler.
   *
   * <p>TODO: add code to disable PID loops when enabling LiveWindow. The commands should re-enable
   * the PID loops themselves when they get rescheduled. This prevents arms from starting to move
   * around, etc. after a period of adjusting them in LiveWindow mode.
   *
   * @param enabled True to enable LiveWindow.
   */
  public static synchronized void setEnabled(boolean enabled) {
    if (liveWindowEnabled != enabled) {
      startLiveWindow = enabled;
      liveWindowEnabled = enabled;
      updateValues(); // Force table generation now to make sure everything is defined
      if (enabled) {
        System.out.println("Starting live window mode.");
        if (enabledListener != null) {
          enabledListener.run();
        }
      } else {
        System.out.println("stopping live window mode.");
        SendableRegistry.foreachLiveWindow(
            dataHandle, cbdata -> ((SendableBuilderImpl) cbdata.builder).stopLiveWindowMode());
        if (disabledListener != null) {
          disabledListener.run();
        }
      }
      enabledPub.set(enabled);
    }
  }

  /**
   * Enable telemetry for a single component.
   *
   * @param sendable component
   */
  public static synchronized void enableTelemetry(Sendable sendable) {
    // Re-enable global setting in case disableAllTelemetry() was called.
    telemetryEnabled = true;
    getOrAdd(sendable).m_telemetryEnabled = true;
  }

  /**
   * Disable telemetry for a single component.
   *
   * @param sendable component
   */
  public static synchronized void disableTelemetry(Sendable sendable) {
    getOrAdd(sendable).m_telemetryEnabled = false;
  }

  /** Disable ALL telemetry. */
  public static synchronized void disableAllTelemetry() {
    telemetryEnabled = false;
    SendableRegistry.foreachLiveWindow(
        dataHandle,
        cbdata -> {
          if (cbdata.data == null) {
            cbdata.data = new Component();
          }
          ((Component) cbdata.data).m_telemetryEnabled = false;
        });
  }

  /** Enable ALL telemetry. */
  public static synchronized void enableAllTelemetry() {
    telemetryEnabled = true;
    SendableRegistry.foreachLiveWindow(
        dataHandle,
        cbdata -> {
          if (cbdata.data == null) {
            cbdata.data = new Component();
          }
          ((Component) cbdata.data).m_telemetryEnabled = true;
        });
  }

  /**
   * Tell all the sensors to update (send) their values.
   *
   * <p>Actuators are handled through callbacks on their value changing from the SmartDashboard
   * widgets.
   */
  public static synchronized void updateValues() {
    // Only do this if either LiveWindow mode or telemetry is enabled.
    if (!liveWindowEnabled && !telemetryEnabled) {
      return;
    }

    SendableRegistry.foreachLiveWindow(
        dataHandle,
        cbdata -> {
          if (cbdata.sendable == null || cbdata.parent != null) {
            return;
          }

          if (cbdata.data == null) {
            cbdata.data = new Component();
          }

          Component component = (Component) cbdata.data;

          if (!liveWindowEnabled && !component.m_telemetryEnabled) {
            return;
          }

          if (component.m_firstTime) {
            // By holding off creating the NetworkTable entries, it allows the
            // components to be redefined. This allows default sensor and actuator
            // values to be created that are replaced with the custom names from
            // users calling setName.
            if (cbdata.name.isEmpty()) {
              return;
            }
            NetworkTable ssTable = liveWindowTable.getSubTable(cbdata.subsystem);
            NetworkTable table;
            // Treat name==subsystem as top level of subsystem
            if (cbdata.name.equals(cbdata.subsystem)) {
              table = ssTable;
            } else {
              table = ssTable.getSubTable(cbdata.name);
            }
            component.m_namePub = new StringTopic(table.getTopic(".name")).publish();
            component.m_namePub.set(cbdata.name);
            ((SendableBuilderImpl) cbdata.builder).setTable(table);
            cbdata.sendable.initSendable(cbdata.builder);
            component.m_typePub =
                new StringTopic(ssTable.getTopic(".type"))
                    .publishEx(
                        StringTopic.kTypeString,
                        "{\"SmartDashboard\":\"" + kSmartDashboardType + "\"}");
            component.m_typePub.set(kSmartDashboardType);

            component.m_firstTime = false;
          }

          if (startLiveWindow) {
            ((SendableBuilderImpl) cbdata.builder).startLiveWindowMode();
          }
          cbdata.builder.update();
        });

    startLiveWindow = false;
  }
}
