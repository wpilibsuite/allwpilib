// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.networktables.DoublePublisher;
import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.NetworkTableInstance;
import java.lang.management.GarbageCollectorMXBean;
import java.lang.management.ManagementFactory;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.function.Consumer;
import java.util.function.Supplier;

/**
 * A Utility class for tracing code execution time. Will put info to NetworkTables under the
 * "Tracer" table.
 *
 * <p>Example inside {@code Robot.java}
 *
 * <pre><code>
 *
 * public void robotPeriodic() {
 *   CommandScheduler.getInstance().run(); // CommandScheduler is already traced
 *   Tracer.traceFunc("MyVendorDep", MyVendorDep::updateAll);
 * }
 * </code></pre>
 *
 * <p>Example inside a {@code Drive Subsystem}
 *
 * <pre><code>
 * // Subsystem periodics are automaticall traced
 * public void periodic() {
 *   for (var module : modules) {
 *     Tracer.traceFunc("Module" + module.getName(), module::update);
 *   }
 *   Tracer.traceFunc("Gyro", gyro::update);
 * }
 * </code></pre>
 */
public class Tracer {
  private static final class TraceStartData {
    private double m_startTime;
    private double m_startGCTotalTime;

    private void set(double startTime, double startGCTotalTime) {
      this.m_startTime = startTime;
      this.m_startGCTotalTime = startGCTotalTime;
    }
  }

  /**
   * All of the tracers persistent state in a single object to be stored in a {@link ThreadLocal}.
   */
  @SuppressWarnings("PMD.RedundantFieldInitializer")
  private static final class TracerState {
    private final NetworkTable m_rootTable;

    /**
     * The stack of traces, every startTrace will add to this stack and every endTrace will remove
     * from this stack.
     */
    private final ArrayList<String> m_traceStack = new ArrayList<>();

    /**
     * ideally we only need `traceStack` but in the interest of memory optimization and string
     * concatenation speed we store the history of the stack to reuse the stack names.
     */
    private final ArrayList<String> m_traceStackHistory = new ArrayList<>();

    /** The time of each trace, the key is the trace name, modified every endTrace. */
    private final HashMap<String, Double> m_traceTimes = new HashMap<>();

    /**
     * The start time of each trace and the gc time at the start of the trace, the key is the trace
     * name, modified every startTrace and endTrace.
     */
    private final HashMap<String, TraceStartData> m_traceStartTimes = new HashMap<>();

    /** the publishers for each trace, the key is the trace name, modified every endCycle. */
    private final HashMap<String, DoublePublisher> m_publishers = new HashMap<>();

    /*
     * If the cycle is poisened, it will warn the user
     * and not publish any data.
     */
    boolean m_cyclePoisoned = false;

    /** If the tracer is disabled, it will not publish any data or do any string manipulation. */
    boolean m_disabled = false;

    /**
     * If the tracer should be disabled next cycle and every cycle after that until this flag is set
     * to false. Disabling is done this way to prevent disabling/enabling.
     */
    boolean m_disableNextCycle = false;

    /**
     * Stack size is used to keep track of stack size even when disabled, calling `EndCycle` is
     * important when disabled or not to update the disabled state in a safe manner.
     */
    int m_stackSize = 0;

    // the garbage collector beans
    private final ArrayList<GarbageCollectorMXBean> m_gcs =
        new ArrayList<>(ManagementFactory.getGarbageCollectorMXBeans());
    private final DoublePublisher m_gcTimeEntry;
    private double m_gcTimeThisCycle = 0.0;

    private TracerState(String threadName) {
      if (singleThreadedMode.get()) {
        DriverStation.reportError(
            "[Tracer] Tracer is in single threaded mode, cannot start traces on multiple threads",
            true);
        this.m_disabled = true;
      }
      anyTracesStarted.set(true);
      if (threadName == null) {
        this.m_rootTable = NetworkTableInstance.getDefault().getTable("Tracer");
      } else {
        this.m_rootTable =
            NetworkTableInstance.getDefault().getTable("Tracer").getSubTable(threadName);
      }
      this.m_gcTimeEntry = m_rootTable.getDoubleTopic("GCTime").publish();
    }

    private String appendTraceStack(String trace) {
      m_stackSize++;
      if (m_disabled) {
        return "";
      }
      m_traceStack.add(trace);
      StringBuilder sb = new StringBuilder();
      for (int i = 0; i < m_traceStack.size(); i++) {
        sb.append(m_traceStack.get(i));
        if (i < m_traceStack.size() - 1) {
          sb.append('/');
        }
      }
      String str = sb.toString();
      m_traceStackHistory.add(str);
      return str;
    }

    private String popTraceStack() {
      m_stackSize = Math.max(0, m_stackSize - 1);
      if (m_disabled) {
        return "";
      }
      if (m_traceStack.isEmpty() || m_traceStackHistory.isEmpty() || m_cyclePoisoned) {
        m_cyclePoisoned = true;
        return "";
      }
      m_traceStack.remove(m_traceStack.size() - 1);
      return m_traceStackHistory.remove(m_traceStackHistory.size() - 1);
    }

    private double totalGCTime() {
      double gcTime = 0;
      for (GarbageCollectorMXBean gc : m_gcs) {
        gcTime += gc.getCollectionTime();
      }
      return gcTime;
    }

    private void endCycle() {
      if (m_disabled != m_disableNextCycle || m_cyclePoisoned) {
        // Gives publishers empty times,
        // reporting no data is better than bad data
        for (var publisher : m_publishers.entrySet()) {
          publisher.getValue().set(0.0);
        }
        return;
      } else {
        // update times for all already existing publishers
        for (var publisher : m_publishers.entrySet()) {
          Double time = m_traceTimes.remove(publisher.getKey());
          if (time == null) {
            time = 0.0;
          }
          publisher.getValue().set(time);
        }
        // create publishers for all new entries
        for (var traceTime : m_traceTimes.entrySet()) {
          DoublePublisher publisher = m_rootTable.getDoubleTopic(traceTime.getKey()).publish();
          publisher.set(traceTime.getValue());
          m_publishers.put(traceTime.getKey(), publisher);
        }
      }

      // log gc time
      if (!m_gcs.isEmpty()) {
        m_gcTimeEntry.set(m_gcTimeThisCycle);
      }
      m_gcTimeThisCycle = 0.0;

      // clean up state
      m_traceTimes.clear();
      m_traceStackHistory.clear();

      m_disabled = m_disableNextCycle;
    }
  }

  private static final AtomicBoolean singleThreadedMode = new AtomicBoolean(false);
  private static final AtomicBoolean anyTracesStarted = new AtomicBoolean(false);
  private static final ThreadLocal<TracerState> threadLocalState =
      ThreadLocal.withInitial(
          () -> {
            return new TracerState(Thread.currentThread().getName());
          });

  private static void startTraceInner(final String name, final TracerState state) {
    String stack = state.appendTraceStack(name);
    if (state.m_disabled) {
      return;
    }
    TraceStartData data = state.m_traceStartTimes.get(stack);
    if (data == null) {
      data = new TraceStartData();
      state.m_traceStartTimes.put(stack, data);
    }
    data.set(Timer.getFPGATimestamp() * 1_000.0, state.totalGCTime());
  }

  private static void endTraceInner(final TracerState state) {
    String stack = state.popTraceStack();
    if (!state.m_disabled) {
      if (stack.isEmpty()) {
        DriverStation.reportError(
            "[Tracer] Stack is empty,"
                + "this means that there are more endTrace calls than startTrace calls",
            true);
        return;
      }
      var startData = state.m_traceStartTimes.get(stack);
      double gcTimeSinceStart = state.totalGCTime() - startData.m_startGCTotalTime;
      state.m_gcTimeThisCycle += gcTimeSinceStart;
      state.m_traceTimes.put(
          stack, Timer.getFPGATimestamp() * 1_000.0 - startData.m_startTime - gcTimeSinceStart);
    }
    if (state.m_traceStack.isEmpty()) {
      state.endCycle();
    }
  }

  /**
   * Starts a trace, should be called at the beginning of a function that's not being called by user
   * code.
   *
   * @param name the name of the trace, should be unique to the function.
   */
  public static void startTrace(String name) {
    startTraceInner(name, threadLocalState.get());
  }

  /**
   * Ends a trace, should only be called at the end of a function that's not being called by user
   * code. If a {@link Tracer#startTrace(String)} is not paired with a {@link Tracer#endTrace()}
   * there could be dropped or incorrect data.
   */
  public static void endTrace() {
    endTraceInner(threadLocalState.get());
  }

  /**
   * Disables garbage collection logging for the current thread. This can help performance in some
   * cases.
   *
   * <p>This counts as starting a tracer on the current thread, this is important to consider with
   * {@link #enableSingleThreadedMode()} and should never be called before if you are using single
   * threaded mode.
   */
  public static void disableGcLoggingForCurrentThread() {
    TracerState state = threadLocalState.get();
    state.m_gcTimeEntry.close();
    state.m_gcs.clear();
  }

  /**
   * Enables single threaded mode for the Tracer. This will cause traces on different threads to
   * throw an exception. This will shorten the path of traced data in NetworkTables by not including
   * the thread name.
   *
   * <p><b>Warning:</b> This will throw an exception if called after any traces have been started.
   */
  public static void enableSingleThreadedMode() {
    if (anyTracesStarted.get()) {
      DriverStation.reportError(
          "[Tracer] Cannot enable single-threaded mode after traces have been started", true);
    } else {
      threadLocalState.set(new TracerState(null));
      singleThreadedMode.set(true);
    }
  }

  /**
   * Disables any tracing for the current thread. This will cause all {@link #startTrace(String)},
   * {@link #endTrace()} and {@link #traceFunc(String, Runnable)} to do nothing.
   */
  public static void disableTracingForCurrentThread() {
    threadLocalState.get().m_disableNextCycle = true;
  }

  /**
   * Enables any tracing for the current thread. This will cause all {@link #startTrace(String)},
   * {@link #endTrace()} and {@link #traceFunc(String, Runnable)} to work as normal.
   */
  public static void enableTracingForCurrentThread() {
    threadLocalState.get().m_disableNextCycle = false;
  }

  /**
   * Traces a function, should be used in place of {@link #startTrace(String)} and {@link
   * #endTrace()} for functions called by user code like {@code CommandScheduler.run()} and other
   * expensive functions.
   *
   * @param name the name of the trace, should be unique to the function.
   * @param runnable the function to trace.
   */
  public static void traceFunc(String name, Runnable runnable) {
    final TracerState state = threadLocalState.get();
    startTraceInner(name, state);
    try {
      runnable.run();
    } catch (Exception e) {
      endTraceInner(state);
      StackTraceElement[] stackTrace = e.getStackTrace();
      ArrayList<StackTraceElement> newStackTrace = new ArrayList<>();
      for (StackTraceElement element : stackTrace) {
        if (!element.getClassName().contains("edu.wpi.first.wpilibj.Tracer")) {
          newStackTrace.add(element);
        }
      }
      e.setStackTrace(newStackTrace.toArray(new StackTraceElement[0]));
      throw e;
    }
    endTraceInner(state);
  }

  /**
   * Traces a function, should be used in place of {@link #startTrace(String)} and {@link
   * #endTrace()} for functions called by user code.
   *
   * @param <R> the return type of the function.
   * @param name the name of the trace, should be unique to the function.
   * @param supplier the function to trace.
   * @return the return value of the function.
   */
  public static <R> R traceFunc(String name, Supplier<R> supplier) {
    final TracerState state = threadLocalState.get();
    startTraceInner(name, state);
    try {
      R ret = supplier.get();
      endTraceInner(state);
      return ret;
    } catch (Exception e) {
      endTraceInner(state);
      StackTraceElement[] stackTrace = e.getStackTrace();
      ArrayList<StackTraceElement> newStackTrace = new ArrayList<>();
      for (StackTraceElement element : stackTrace) {
        if (!element.getClassName().contains("edu.wpi.first.wpilibj.Tracer")) {
          newStackTrace.add(element);
        }
      }
      e.setStackTrace(newStackTrace.toArray(new StackTraceElement[0]));
      throw e;
    }
  }

  /** This function is only to be used in tests and is package private to prevent misuse. */
  static void resetForTest() {
    threadLocalState.remove();
    singleThreadedMode.set(false);
    anyTracesStarted.set(false);
  }

  // A REIMPLEMENTATION OF THE OLD TRACER TO NOT BREAK OLD CODE

  private static final long kMinPrintPeriod = 1000000; // microseconds

  private long m_lastEpochsPrintTime; // microseconds
  private long m_startTime; // microseconds

  private final HashMap<String, Long> m_epochs = new HashMap<>(); // microseconds

  /**
   * Constructs a {@code Tracer} compatible with the 2024 {@code Tracer}.
   *
   * @deprecated This constructor is only for compatibility with the 2024 {@code Tracer} and will be
   *     removed in 2025. Use the static methods in {@link Tracer} instead.
   */
  @Deprecated(since = "2025", forRemoval = true)
  public Tracer() {
    resetTimer();
  }

  /**
   * Clears all epochs.
   *
   * @deprecated This method is only for compatibility with the 2024 {@code Tracer} and will be
   *     removed in 2025. Use the static methods in {@link Tracer} instead.
   */
  @Deprecated(since = "2025", forRemoval = true)
  public void clearEpochs() {
    m_epochs.clear();
    resetTimer();
  }

  /**
   * Restarts the epoch timer.
   *
   * @deprecated This method is only for compatibility with the 2024 {@code Tracer} and will be
   *     removed in 2025. Use the static methods in {@link Tracer} instead.
   */
  @Deprecated(since = "2025", forRemoval = true)
  public final void resetTimer() {
    m_startTime = RobotController.getFPGATime();
  }

  /**
   * Adds time since last epoch to the list printed by printEpochs().
   *
   * <p>Epochs are a way to partition the time elapsed so that when overruns occur, one can
   * determine which parts of an operation consumed the most time.
   *
   * <p>This should be called immediately after execution has finished, with a call to this method
   * or {@link #resetTimer()} before execution.
   *
   * @param epochName The name to associate with the epoch.
   * @deprecated This method is only for compatibility with the 2024 {@code Tracer} and will be
   *     removed in 2025. Use the static methods in {@link Tracer} instead.
   */
  @Deprecated(since = "2025", forRemoval = true)
  public void addEpoch(String epochName) {
    long currentTime = RobotController.getFPGATime();
    m_epochs.put(epochName, currentTime - m_startTime);
    m_startTime = currentTime;
  }

  /**
   * Prints list of epochs added so far and their times to the DriverStation.
   *
   * @deprecated This method is only for compatibility with the 2024 {@code Tracer} and will be
   *     removed in 2025. Use the static methods in {@link Tracer} instead.
   */
  @Deprecated(since = "2025", forRemoval = true)
  public void printEpochs() {
    printEpochs(out -> DriverStation.reportWarning(out, false));
  }

  /**
   * Prints list of epochs added so far and their times to the entered String consumer.
   *
   * <p>This overload can be useful for logging to a file, etc.
   *
   * @param output the stream that the output is sent to
   * @deprecated This method is only for compatibility with the 2024 {@code Tracer} and will be
   *     removed in 2025. Use the static methods in {@link Tracer} instead.
   */
  @Deprecated(since = "2025", forRemoval = true)
  public void printEpochs(Consumer<String> output) {
    long now = RobotController.getFPGATime();
    if (now - m_lastEpochsPrintTime > kMinPrintPeriod) {
      StringBuilder sb = new StringBuilder();
      m_lastEpochsPrintTime = now;
      m_epochs.forEach(
          (key, value) -> sb.append(String.format("\t%s: %.6fs\n", key, value / 1.0e6)));
      if (sb.length() > 0) {
        output.accept(sb.toString());
      }
    }
  }
}
