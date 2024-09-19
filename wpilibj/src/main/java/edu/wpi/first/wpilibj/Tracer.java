// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import java.lang.management.GarbageCollectorMXBean;
import java.lang.management.ManagementFactory;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.function.Supplier;
import java.util.concurrent.atomic.AtomicBoolean;

import edu.wpi.first.networktables.DoublePublisher;
import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.NetworkTableInstance;

/**
 * A Utility class for tracing code execution time.
 * Will put info to NetworkTables under the "Tracer" table.
 * 
 * <p> Example inside {@code Robot.java}
 * <pre><code>
 * @Override
 * public void robotPeriodic() {
 *   Tracer.traceFunc("CommandScheduler", scheduler::run);
 *   Tracer.traceFunc("MyVendorDep", MyVendorDep::updateAll);
 * }
 * </code></pre>
 * 
 * <p> Example inside a {@code Drive Subsystem}
 * <pre><code>
 * public void periodic() {
 *   Tracer.startTrace("DrivePeriodic");
 *   for (var module : modules) {
 *     Tracer.traceFunc("Module" + module.getName(), module::update);
 *   }
 *   Tracer.traceFunc("Gyro", gyro::update);
 *   Tracer.endTrace();
 * }
 * </code></pre>
 */
public class Tracer {
    private static final class TraceStartData {
        private double startTime = 0.0;
        private double startGCTotalTime = 0.0;

        private void set(double startTime, double startGCTotalTime) {
            this.startTime = startTime;
            this.startGCTotalTime = startGCTotalTime;
        }
    }

    /**
     * All of the tracers persistent state in a single object to be stored in a
     * {@link ThreadLocal}.
     */
    private static final class TracerState {
        private final NetworkTable rootTable;

        // the stack of traces, every startTrace will add to this stack
        // and every endTrace will remove from this stack
        private final ArrayList<String> traceStack = new ArrayList<>();
        // ideally we only need `traceStack` but in the interest of memory optimization
        // and string concatenation speed we store the history of the stack to reuse the stack names
        private final ArrayList<String> traceStackHistory = new ArrayList<>();
        // the time of each trace, the key is the trace name, modified every endTrace
        private final HashMap<String, Double> traceTimes = new HashMap<>();
        // the start time of each trace and the gc time at the start of the trace,
        // the key is the trace name, modified every startTrace and endTrace.
        private final HashMap<String, TraceStartData> traceStartTimes = new HashMap<>();
        // the publishers for each trace, the key is the trace name, modified every endCycle
        private final HashMap<String, DoublePublisher> publisherHeap = new HashMap<>();

        // the garbage collector beans
        private final ArrayList<GarbageCollectorMXBean> gcs = new ArrayList<>(ManagementFactory.getGarbageCollectorMXBeans());
        private final DoublePublisher gcTimeEntry;
        private double gcTimeThisCycle = 0.0;

        private TracerState(String threadName) {
            if (threadName == null) {
                this.rootTable = NetworkTableInstance.getDefault().getTable("Tracer");
            } else {
                this.rootTable = NetworkTableInstance.getDefault().getTable("Tracer").getSubTable(threadName);
            }
            this.gcTimeEntry = rootTable.getDoubleTopic("GCTime").publishEx(
                    "double",
                    "{ \"cached\": false}"
                );
        }

        private String appendTraceStack(String trace) {
            traceStack.add(trace);
            StringBuilder sb = new StringBuilder();
            for (int i = 0; i < traceStack.size(); i++) {
                sb.append(traceStack.get(i));
                if (i < traceStack.size() - 1) {
                    sb.append("/");
                }
            }
            String str = sb.toString();
            traceStackHistory.add(str);
            return str;
        }

        private String popTraceStack() {
            traceStack.remove(traceStack.size() - 1);
            return traceStackHistory.remove(traceStackHistory.size() - 1);
        }

        private double totalGCTime() {
            double gcTime = 0;
            for (GarbageCollectorMXBean gc : gcs) {
                gcTime += gc.getCollectionTime();
            }
            return gcTime;
        }

        private void endCycle() {
            // update times for all already existing publishers
            for (var publisher : publisherHeap.entrySet()) {
                Double time = traceTimes.remove(publisher.getKey());
                if (time == null) time = 0.0;
                publisher.getValue().set(time);
            }
            // create publishers for all new entries
            for (var traceTime : traceTimes.entrySet()) {
                DoublePublisher publisher = rootTable.getDoubleTopic(traceTime.getKey()).publishEx(
                    "double",
                    "{ \"cached\": false}"
                );
                publisher.set(traceTime.getValue());
                publisherHeap.put(traceTime.getKey(), publisher);
            }

            // log gc time
            if (gcs.size() > 0) gcTimeEntry.set(gcTimeThisCycle);
            gcTimeThisCycle = 0.0;

            // clean up state
            traceTimes.clear();
            traceStackHistory.clear();
        }
    }

    private static final AtomicBoolean singleThreadedMode = new AtomicBoolean(false);
    private static final ThreadLocal<TracerState> threadLocalState = ThreadLocal.withInitial(() -> {
        if (singleThreadedMode.get()) {
            throw new IllegalStateException("Single threaded mode is enabled, cannot create new TracerState");
        }
        return new TracerState(Thread.currentThread().getName());
    });

    private static void startTraceInner(final String name, final TracerState state) {
        String stack = state.appendTraceStack(name);
        TraceStartData data = state.traceStartTimes.get(stack);
        if (data == null) {
            data = new TraceStartData();
            state.traceStartTimes.put(stack, data);
        }
        data.set(Timer.getFPGATimestamp() * 1_000.0, state.totalGCTime());
    }

    private static void endTraceInner(final TracerState state) {
        try {
            String stack = state.popTraceStack();
            var startData = state.traceStartTimes.get(stack);
            double gcTimeSinceStart = state.totalGCTime() - startData.startGCTotalTime;
            state.gcTimeThisCycle += gcTimeSinceStart;
            state.traceTimes.put(
                stack,
                Timer.getFPGATimestamp() * 1_000.0
                - startData.startTime
                - gcTimeSinceStart
            );
            if (state.traceStack.size() == 0) {
                state.endCycle();
            }
        } catch (Exception e) {
            DriverStation.reportError("[Tracer] An end trace was called with no opening trace " + e, true);
            for (var trace : state.traceStack) {
                DriverStation.reportError("[Tracer] Open trace: " + trace, false);
            }
            state.endCycle();
        }
    }

    /**
     * Starts a trace,
     * should be called at the beginning of a function thats not being called by
     * user code.
     * Should be paired with {@link Tracer#endTrace()} at the end of the function.
     * 
     * Best used in periodic functions in Subsystems and Robot.java.
     * 
     * @param name the name of the trace, should be unique to the function.
     */
    public static void startTrace(String name) {
        startTraceInner(name, threadLocalState.get());
    }

    /**
     * Ends a trace, should only be called at the end of a function thats not being
     * called by user code.
     * If a {@link Tracer#startTrace(String)} is not paired with a
     * {@link Tracer#endTrace()} there could be a crash.
     */
    public static void endTrace() {
        endTraceInner(threadLocalState.get());
    }

    /**
     * Disables garbage collection logging for the current thread.
     * This can help performance in some cases.
     */
    public static void disableGcLoggingForCurrentThread() {
        TracerState state = threadLocalState.get();
        state.gcTimeEntry.close();
        state.gcs.clear();
    }

    /**
     * Enables single threaded mode for the Tracer.
     * This will cause traces on different threads to throw an exception.
     * This will shorten the path of traced data in NetworkTables by not including the thread name.
     * 
     * <p><b>Warning:</b> This will throw an exception if called after any traces have been started.
     */
    public static void enableSingleThreadedMode() {
        threadLocalState.set(new TracerState(null));
        singleThreadedMode.set(true);
    }

    /**
     * Traces a function, should be used in place of
     * {@link Tracer#startTrace(String)} and {@link Tracer#endTrace()}
     * for functions called by user code like {@code CommandScheduler.run()} and
     * other expensive functions.
     * 
     * @param name     the name of the trace, should be unique to the function.
     * @param runnable the function to trace.
     * 
     * @apiNote If you want to return a value then use
     *          {@link Tracer#traceFunc(String, Supplier)}.
     */
    public static void traceFunc(String name, Runnable runnable) {
        final TracerState state = threadLocalState.get();
        startTraceInner(name, state);
        runnable.run();
        endTraceInner(state);
    }

    /**
     * Traces a function, should be used in place of
     * {@link Tracer#startTrace(String)} and {@link Tracer#endTrace()}
     * for functions called by user code like {@code CommandScheduler.run()} and
     * other expensive functions.
     * 
     * @param name     the name of the trace, should be unique to the function.
     * @param supplier the function to trace.
     */
    public static <R> R traceFunc(String name, Supplier<R> supplier) {
        final TracerState state = threadLocalState.get();
        startTraceInner(name, state);
        R ret = supplier.get();
        endTraceInner(state);
        return ret;
    }
}

