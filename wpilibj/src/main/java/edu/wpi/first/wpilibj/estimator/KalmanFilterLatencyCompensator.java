package edu.wpi.first.wpilibj.estimator;

import java.util.Map;
import java.util.TreeMap;

import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.Num;
import edu.wpi.first.wpiutil.math.numbers.N1;

class KalmanFilterLatencyCompensator<S extends Num, I extends Num, O extends Num> {
  private static final int k_maxPastObserverStates = 300;

  private final TreeMap<Double, ObserverSnapshot> m_pastObserverSnapshots;

  KalmanFilterLatencyCompensator() {
    m_pastObserverSnapshots = new TreeMap<>();
  }

  @SuppressWarnings("ParameterName")
  public void addObserverState(
          KalmanTypeFilter<S, I, O> observer, Matrix<I, N1> u, double timestampSeconds
  ) {
    m_pastObserverSnapshots.put(timestampSeconds, new ObserverSnapshot(observer, u));

    if (m_pastObserverSnapshots.size() > k_maxPastObserverStates) {
      m_pastObserverSnapshots.remove(m_pastObserverSnapshots.firstKey());
    }
  }

  @SuppressWarnings("ParameterName")
  public void applyPastMeasurement(
          KalmanTypeFilter<S, I, O> observer,
          double nominalDtSeconds,
          Matrix<O, N1> y,
          double timestampSeconds
  ) {
    var low = m_pastObserverSnapshots.floorEntry(timestampSeconds);
    var high = m_pastObserverSnapshots.ceilingEntry(timestampSeconds);

    // Find the entry closest in time to timestampSeconds
    Map.Entry<Double, ObserverSnapshot> closestEntry = null;
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

    var newSnapshots = new TreeMap<Double, ObserverSnapshot>();
    var snapshotsToUse = m_pastObserverSnapshots.tailMap(closestEntry.getKey(), true);

    double lastTimestamp = snapshotsToUse.firstEntry() != null
            ? snapshotsToUse.firstEntry().getKey() - nominalDtSeconds : 0;
    for (var entry : snapshotsToUse.entrySet()) {
      var st = entry.getValue();

      observer.predict(st.inputs, entry.getKey() - lastTimestamp);
      lastTimestamp = entry.getKey();
      if (y != null) {
        observer.setP(st.errorCovariances);
        observer.setXhat(st.xHat);
        // Note that we correct the observer with inputs closest in time to the measurement
        // This makes the assumption that the dt is small enough that the difference between the
        // measurement time and the time that the inputs were captured at is very small
        observer.correct(st.inputs, y);
      }

      newSnapshots.put(entry.getKey(), new ObserverSnapshot(observer, st.inputs));

      y = null;
    }

    // Replace observer snapshots that haven't been corrected by "y" for ones that have
    // This is a 1:1 replacement of uncorrected to corrected snapshots
    snapshotsToUse.clear();
    m_pastObserverSnapshots.putAll(newSnapshots);
  }

  /**
   * This class contains all the information about our observer at a given time.
   */
  @SuppressWarnings("MemberName")
  public class ObserverSnapshot {
    public final Matrix<S, N1> xHat;
    public final Matrix<S, S> errorCovariances;
    public final Matrix<I, N1> inputs;

    @SuppressWarnings("ParameterName")
    private ObserverSnapshot(KalmanTypeFilter<S, I, O> observer, Matrix<I, N1> u) {
      this.xHat = observer.getXhat();
      this.errorCovariances = observer.getP();

      inputs = u;
    }
  }
}
