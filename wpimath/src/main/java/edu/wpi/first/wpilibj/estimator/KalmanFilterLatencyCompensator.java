/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.estimator;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.function.BiConsumer;

import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.Nat;
import edu.wpi.first.wpiutil.math.Num;
import edu.wpi.first.wpiutil.math.numbers.N1;

public class KalmanFilterLatencyCompensator<S extends Num, I extends Num, O extends Num> {
  private static final int kMaxPastObserverStates = 300;

  private final List<Map.Entry<Double, ObserverSnapshot>> m_pastObserverSnapshots;

  KalmanFilterLatencyCompensator() {
    m_pastObserverSnapshots = new ArrayList<>();
  }

  /**
   * Add past observer states to the observer snapshots list.
   *
   * @param observer         The observer.
   * @param u                The input at the timestamp.
   * @param localY           The local output at the timestamp
   * @param timestampSeconds The timesnap of the state.
   */
  @SuppressWarnings("ParameterName")
  public void addObserverState(
          KalmanTypeFilter<S, I, O> observer, Matrix<I, N1> u, Matrix<O, N1> localY,
          double timestampSeconds
  ) {
    m_pastObserverSnapshots.add(Map.entry(
            timestampSeconds, new ObserverSnapshot(observer, u, localY)
    ));

    if (m_pastObserverSnapshots.size() > kMaxPastObserverStates) {
      m_pastObserverSnapshots.remove(0);
    }
  }

  /**
   * Add past global measurements (such as from vision)to the estimator.
   *
   * @param <R>                               The rows in the global measurement vector.
   * @param rows                              The rows in the global measurement vector.
   * @param observer                          The observer to apply the past global measurement.
   * @param nominalDtSeconds                  The nominal timestep.
   * @param globalMeasurement                 The measurement.
   * @param globalMeasurementCorrect          The function take calls correct() on the observer.
   * @param globalMeasurementTimestampSeconds The timestamp of the measurement.
   */
  @SuppressWarnings({"ParameterName", "PMD.AvoidInstantiatingObjectsInLoops"})
  public <R extends Num> void applyPastGlobalMeasurement(
          Nat<R> rows,
          KalmanTypeFilter<S, I, O> observer,
          double nominalDtSeconds,
          Matrix<R, N1> globalMeasurement,
          BiConsumer<Matrix<I, N1>, Matrix<R, N1>> globalMeasurementCorrect,
          double globalMeasurementTimestampSeconds
  ) {
    if (m_pastObserverSnapshots.isEmpty()) {
      // State map was empty, which means that we got a past measurement right at startup. The only
      // thing we can really do is ignore the measurement.
      return;
    }

    // This index starts at one because we use the previous state later on, and we always want to
    // have a "previous state".
    int maxIdx = m_pastObserverSnapshots.size() - 1;
    int low = 1;
    int high = Math.max(maxIdx, 1);

    while (low != high) {
      int mid = (low + high) / 2;
      if (m_pastObserverSnapshots.get(mid).getKey() < globalMeasurementTimestampSeconds) {
        // This index and everything under it are less than the requested timestamp. Therefore, we
        // can discard them.
        low = mid + 1;
      } else {
        // t is at least as large as the element at this index. This means that anything after it
        // cannot be what we are looking for.
        high = mid;
      }
    }

    // We are simply assigning this index to a new variable to avoid confusion
    // with variable names.
    int index = low;
    double timestamp = globalMeasurementTimestampSeconds;
    int indexOfClosestEntry =
        Math.abs(timestamp - m_pastObserverSnapshots.get(index - 1).getKey())
            <= Math.abs(timestamp - m_pastObserverSnapshots.get(Math.min(index, maxIdx)).getKey())
            ? index - 1
            : index;

    double lastTimestamp =
            m_pastObserverSnapshots.get(indexOfClosestEntry).getKey() - nominalDtSeconds;

    // We will now go back in time to the state of the system at the time when
    // the measurement was captured. We will reset the observer to that state,
    // and apply correction based on the measurement. Then, we will go back
    // through all observer states until the present and apply past inputs to
    // get the present estimated state.
    for (int i = indexOfClosestEntry; i < m_pastObserverSnapshots.size(); i++) {
      var key = m_pastObserverSnapshots.get(i).getKey();
      var snapshot = m_pastObserverSnapshots.get(i).getValue();

      if (i == indexOfClosestEntry) {
        observer.setP(snapshot.errorCovariances);
        observer.setXhat(snapshot.xHat);
      }

      observer.predict(snapshot.inputs, key - lastTimestamp);
      observer.correct(snapshot.inputs, snapshot.localMeasurements);

      if (i == indexOfClosestEntry) {
        // Note that the measurement is at a timestep close but probably not exactly equal to the
        // timestep for which we called predict.
        // This makes the assumption that the dt is small enough that the difference between the
        // measurement time and the time that the inputs were captured at is very small.
        globalMeasurementCorrect.accept(snapshot.inputs, globalMeasurement);
      }
      lastTimestamp = key;

      m_pastObserverSnapshots.set(i, Map.entry(key,
              new ObserverSnapshot(observer, snapshot.inputs, snapshot.localMeasurements)));
    }
  }

  /**
   * This class contains all the information about our observer at a given time.
   */
  @SuppressWarnings("MemberName")
  public class ObserverSnapshot {
    public final Matrix<S, N1> xHat;
    public final Matrix<S, S> errorCovariances;
    public final Matrix<I, N1> inputs;
    public final Matrix<O, N1> localMeasurements;

    @SuppressWarnings("ParameterName")
    private ObserverSnapshot(
            KalmanTypeFilter<S, I, O> observer, Matrix<I, N1> u, Matrix<O, N1> localY
    ) {
      this.xHat = observer.getXhat();
      this.errorCovariances = observer.getP();

      inputs = u;
      localMeasurements = localY;
    }
  }
}
