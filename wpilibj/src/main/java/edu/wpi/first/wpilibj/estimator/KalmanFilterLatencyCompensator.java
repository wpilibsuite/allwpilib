package edu.wpi.first.wpilibj.estimator;

import java.util.Map;
import java.util.TreeMap;

import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.Num;
import edu.wpi.first.wpiutil.math.numbers.N1;

class KalmanFilterLatencyCompensator<S extends Num, I extends Num, O extends Num> {
  private static final int k_maxPastObserverStates = 300;

  private final TreeMap<Double, ObserverState> m_pastObserverStates;

  KalmanFilterLatencyCompensator() {
    m_pastObserverStates = new TreeMap<>();
  }

  @SuppressWarnings("ParameterName")
  public void addObserverState(
          KalmanTypeFilter<S, I, O> observer, Matrix<I, N1> u, double timestampSeconds
  ) {
    m_pastObserverStates.put(timestampSeconds, new ObserverState(observer, u));

    if (m_pastObserverStates.size() > k_maxPastObserverStates) {
      m_pastObserverStates.remove(m_pastObserverStates.firstKey());
    }
  }

  @SuppressWarnings("ParameterName")
  public void applyPastMeasurement(
          KalmanTypeFilter<S, I, O> observer,
          double nominalDtSeconds,
          Matrix<O, N1> y,
          double timestampSeconds
  ) {
    var low = m_pastObserverStates.floorEntry(timestampSeconds);
    var high = m_pastObserverStates.ceilingEntry(timestampSeconds);

    // Find the entry closest in time to timestampSeconds
    Map.Entry<Double, ObserverState> closestEntry = null;
    if (low != null && high != null) {
      closestEntry =
              Math.abs(timestampSeconds - low.getKey()) < Math.abs(timestampSeconds - high.getKey())
                      ? low : high;
    } else {
      closestEntry = low != null ? low : high;
    }
    if (closestEntry == null) {
      // State map was empty, which means that we got a past measurement right at startup
      // The only thing we can really do is ignore the measurement
      return;
    }

    var newSnapshots = new TreeMap<Double, ObserverState>();
    var snapshotsToUse = m_pastObserverStates.tailMap(closestEntry.getKey(), true);

    double lastTimestamp = snapshotsToUse.firstEntry() != null
            ? snapshotsToUse.firstEntry().getKey() - nominalDtSeconds : 0;
    for (var entry : snapshotsToUse.entrySet()) {
      var st = entry.getValue();
      if (y != null) {
        observer.setP(st.errorCovariances);
        observer.setXhat(st.xHat);
        // Note that we correct the observer with inputs closest in time to the measurement
        // This makes the assumption that the dt is small enough that the difference between the
        // measurement time and the time that the inputs were captured at is very small
        observer.correct(st.inputs, y);
      }
      observer.predict(st.inputs, entry.getKey() - lastTimestamp);
      lastTimestamp = entry.getKey();

      newSnapshots.put(entry.getKey(), new ObserverState(observer, st.inputs));

      y = null;
    }

    // Replace observer snapshots that haven't been corrected by a measurement for ones that have
    snapshotsToUse.clear();
    m_pastObserverStates.putAll(newSnapshots);
  }

  /**
   * This class contains all the information about our observer at a given time.
   */
  @SuppressWarnings("MemberName")
  public class ObserverState {
    public final Matrix<S, N1> xHat;
    public final Matrix<S, S> errorCovariances;
    public final Matrix<I, N1> inputs;

    @SuppressWarnings("ParameterName")
    private ObserverState(KalmanTypeFilter<S, I, O> observer, Matrix<I, N1> u) {
      this.xHat = observer.getXhat();
      this.errorCovariances = observer.getP();

      inputs = u;
    }
  }
}
