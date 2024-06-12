// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.estimator;

import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Nat;
import edu.wpi.first.math.Num;
import edu.wpi.first.math.numbers.N1;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.function.BiConsumer;

/**
 * This class incorporates time-delayed measurements into a Kalman filter's state estimate.
 *
 * @param <S> The number of states.
 * @param <I> The number of inputs.
 * @param <O> The number of outputs.
 */
public class KalmanFilterLatencyCompensator<S extends Num, I extends Num, O extends Num> {
  private static final int kMaxPastObserverStates = 300;

  private final List<Map.Entry<Double, ObserverSnapshot>> m_pastObserverSnapshots;

  /** Default constructor. */
  KalmanFilterLatencyCompensator() {
    m_pastObserverSnapshots = new ArrayList<>();
  }

  /** Clears the observer snapshot buffer. */
  public void reset() {
    m_pastObserverSnapshots.clear();
  }

  /**
   * Add past observer states to the observer snapshots list.
   *
   * @param observer The observer.
   * @param u The input at the timestamp.
   * @param localY The local output at the timestamp
   * @param timestampSeconds The timestamp of the state.
   */
  public void addObserverState(
      KalmanTypeFilter<S, I, O> observer,
      Matrix<I, N1> u,
      Matrix<O, N1> localY,
      double timestampSeconds) {
    m_pastObserverSnapshots.add(
        Map.entry(timestampSeconds, new ObserverSnapshot(observer, u, localY)));

    if (m_pastObserverSnapshots.size() > kMaxPastObserverStates) {
      m_pastObserverSnapshots.remove(0);
    }
  }

  /**
   * Add past global measurements (such as from vision)to the estimator.
   *
   * @param <R> The rows in the global measurement vector.
   * @param rows The rows in the global measurement vector.
   * @param observer The observer to apply the past global measurement.
   * @param nominalDtSeconds The nominal timestep.
   * @param y The measurement.
   * @param globalMeasurementCorrect The function take calls correct() on the observer.
   * @param timestampSeconds The timestamp of the measurement.
   */
  public <R extends Num> void applyPastGlobalMeasurement(
      Nat<R> rows,
      KalmanTypeFilter<S, I, O> observer,
      double nominalDtSeconds,
      Matrix<R, N1> y,
      BiConsumer<Matrix<I, N1>, Matrix<R, N1>> globalMeasurementCorrect,
      double timestampSeconds) {
    if (m_pastObserverSnapshots.isEmpty()) {
      // State map was empty, which means that we got a past measurement right at startup. The only
      // thing we can really do is ignore the measurement.
      return;
    }

    // Use a less verbose name for timestamp
    double timestamp = timestampSeconds;

    int maxIdx = m_pastObserverSnapshots.size() - 1;
    int low = 0;
    int high = maxIdx;

    // Perform a binary search to find the index of first snapshot whose
    // timestamp is greater than or equal to the global measurement timestamp
    while (low != high) {
      int mid = (low + high) / 2;
      if (m_pastObserverSnapshots.get(mid).getKey() < timestamp) {
        // This index and everything under it are less than the requested timestamp. Therefore, we
        // can discard them.
        low = mid + 1;
      } else {
        // t is at least as large as the element at this index. This means that anything after it
        // cannot be what we are looking for.
        high = mid;
      }
    }

    int indexOfClosestEntry;

    if (low == 0) {
      // If the global measurement is older than any snapshot, throw out the
      // measurement because there's no state estimate into which to incorporate
      // the measurement
      if (timestamp < m_pastObserverSnapshots.get(low).getKey()) {
        return;
      }

      // If the first snapshot has same timestamp as the global measurement, use
      // that snapshot
      indexOfClosestEntry = 0;
    } else if (low == maxIdx && m_pastObserverSnapshots.get(low).getKey() < timestamp) {
      // If all snapshots are older than the global measurement, use the newest
      // snapshot
      indexOfClosestEntry = maxIdx;
    } else {
      // Index of snapshot taken after the global measurement
      int nextIdx = low;

      // Index of snapshot taken before the global measurement. Since we already
      // handled the case where the index points to the first snapshot, this
      // computation is guaranteed to be non-negative.
      int prevIdx = nextIdx - 1;

      // Find the snapshot closest in time to global measurement
      double prevTimeDiff = Math.abs(timestamp - m_pastObserverSnapshots.get(prevIdx).getKey());
      double nextTimeDiff = Math.abs(timestamp - m_pastObserverSnapshots.get(nextIdx).getKey());
      indexOfClosestEntry = prevTimeDiff <= nextTimeDiff ? prevIdx : nextIdx;
    }

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
        globalMeasurementCorrect.accept(snapshot.inputs, y);
      }
      lastTimestamp = key;

      m_pastObserverSnapshots.set(
          i,
          Map.entry(
              key, new ObserverSnapshot(observer, snapshot.inputs, snapshot.localMeasurements)));
    }
  }

  /** This class contains all the information about our observer at a given time. */
  public final class ObserverSnapshot {
    /** The state estimate. */
    public final Matrix<S, N1> xHat;

    /** The error covariance. */
    public final Matrix<S, S> errorCovariances;

    /** The inputs. */
    public final Matrix<I, N1> inputs;

    /** The local measurements. */
    public final Matrix<O, N1> localMeasurements;

    private ObserverSnapshot(
        KalmanTypeFilter<S, I, O> observer, Matrix<I, N1> u, Matrix<O, N1> localY) {
      this.xHat = observer.getXhat();
      this.errorCovariances = observer.getP();

      inputs = u;
      localMeasurements = localY;
    }
  }
}
