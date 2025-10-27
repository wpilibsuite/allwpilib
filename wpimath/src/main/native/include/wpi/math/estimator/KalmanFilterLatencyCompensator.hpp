// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <algorithm>
#include <array>
#include <functional>
#include <utility>
#include <vector>

#include "frc/EigenCore.h"
#include "units/math.h"
#include "units/time.h"

namespace frc {

/**
 * This class incorporates time-delayed measurements into a Kalman filter's
 * state estimate.
 *
 * @tparam States The number of states.
 * @tparam Inputs The number of inputs.
 * @tparam Outputs The number of outputs.
 */
template <int States, int Inputs, int Outputs, typename KalmanFilterType>
class KalmanFilterLatencyCompensator {
 public:
  /**
   * This class contains all the information about our observer at a given time.
   */
  struct ObserverSnapshot {
    /// The state estimate.
    Vectord<States> xHat;
    /// The square root error covariance.
    Matrixd<States, States> squareRootErrorCovariances;
    /// The inputs.
    Vectord<Inputs> inputs;
    /// The local measurements.
    Vectord<Outputs> localMeasurements;

    ObserverSnapshot(const KalmanFilterType& observer, const Vectord<Inputs>& u,
                     const Vectord<Outputs>& localY)
        : xHat(observer.Xhat()),
          squareRootErrorCovariances(observer.S()),
          inputs(u),
          localMeasurements(localY) {}
  };

  /**
   * Clears the observer snapshot buffer.
   */
  void Reset() { m_pastObserverSnapshots.clear(); }

  /**
   * Add past observer states to the observer snapshots list.
   *
   * @param observer  The observer.
   * @param u         The input at the timestamp.
   * @param localY    The local output at the timestamp
   * @param timestamp The timestamp of the state.
   */
  void AddObserverState(const KalmanFilterType& observer, Vectord<Inputs> u,
                        Vectord<Outputs> localY, units::second_t timestamp) {
    // Add the new state into the vector.
    m_pastObserverSnapshots.emplace_back(timestamp,
                                         ObserverSnapshot{observer, u, localY});

    // Remove the oldest snapshot if the vector exceeds our maximum size.
    if (m_pastObserverSnapshots.size() > kMaxPastObserverStates) {
      m_pastObserverSnapshots.erase(m_pastObserverSnapshots.begin());
    }
  }

  /**
   * Add past global measurements (such as from vision)to the estimator.
   *
   * @param observer                 The observer to apply the past global
   *                                 measurement.
   * @param nominalDt                The nominal timestep.
   * @param y                        The measurement.
   * @param globalMeasurementCorrect The function take calls correct() on the
   *                                 observer.
   * @param timestamp                The timestamp of the measurement.
   */
  template <int Rows>
  void ApplyPastGlobalMeasurement(
      KalmanFilterType* observer, units::second_t nominalDt, Vectord<Rows> y,
      std::function<void(const Vectord<Inputs>& u, const Vectord<Rows>& y)>
          globalMeasurementCorrect,
      units::second_t timestamp) {
    if (m_pastObserverSnapshots.size() == 0) {
      // State map was empty, which means that we got a measurement right at
      // startup. The only thing we can do is ignore the measurement.
      return;
    }

    // Perform a binary search to find the index of first snapshot whose
    // timestamp is greater than or equal to the global measurement timestamp
    auto it = std::lower_bound(
        m_pastObserverSnapshots.cbegin(), m_pastObserverSnapshots.cend(),
        timestamp,
        [](const auto& entry, const auto& ts) { return entry.first < ts; });

    size_t indexOfClosestEntry;

    if (it == m_pastObserverSnapshots.cbegin()) {
      // If the global measurement is older than any snapshot, throw out the
      // measurement because there's no state estimate into which to incorporate
      // the measurement
      if (timestamp < it->first) {
        return;
      }

      // If the first snapshot has same timestamp as the global measurement, use
      // that snapshot
      indexOfClosestEntry = 0;
    } else if (it == m_pastObserverSnapshots.cend()) {
      // If all snapshots are older than the global measurement, use the newest
      // snapshot
      indexOfClosestEntry = m_pastObserverSnapshots.size() - 1;
    } else {
      // Index of snapshot taken after the global measurement
      int nextIdx = std::distance(m_pastObserverSnapshots.cbegin(), it);

      // Index of snapshot taken before the global measurement. Since we already
      // handled the case where the index points to the first snapshot, this
      // computation is guaranteed to be nonnegative.
      int prevIdx = nextIdx - 1;

      // Find the snapshot closest in time to global measurement
      units::second_t prevTimeDiff =
          units::math::abs(timestamp - m_pastObserverSnapshots[prevIdx].first);
      units::second_t nextTimeDiff =
          units::math::abs(timestamp - m_pastObserverSnapshots[nextIdx].first);
      indexOfClosestEntry = prevTimeDiff < nextTimeDiff ? prevIdx : nextIdx;
    }

    units::second_t lastTimestamp =
        m_pastObserverSnapshots[indexOfClosestEntry].first - nominalDt;

    // We will now go back in time to the state of the system at the time when
    // the measurement was captured. We will reset the observer to that state,
    // and apply correction based on the measurement. Then, we will go back
    // through all observer states until the present and apply past inputs to
    // get the present estimated state.
    for (size_t i = indexOfClosestEntry; i < m_pastObserverSnapshots.size();
         ++i) {
      auto& [key, snapshot] = m_pastObserverSnapshots[i];

      if (i == indexOfClosestEntry) {
        observer->SetS(snapshot.squareRootErrorCovariances);
        observer->SetXhat(snapshot.xHat);
      }

      observer->Predict(snapshot.inputs, key - lastTimestamp);
      observer->Correct(snapshot.inputs, snapshot.localMeasurements);

      if (i == indexOfClosestEntry) {
        // Note that the measurement is at a timestep close but probably not
        // exactly equal to the timestep for which we called predict. This makes
        // the assumption that the dt is small enough that the difference
        // between the measurement time and the time that the inputs were
        // captured at is very small.
        globalMeasurementCorrect(snapshot.inputs, y);
      }

      lastTimestamp = key;
      snapshot = ObserverSnapshot{*observer, snapshot.inputs,
                                  snapshot.localMeasurements};
    }
  }

 private:
  static constexpr size_t kMaxPastObserverStates = 300;
  std::vector<std::pair<units::second_t, ObserverSnapshot>>
      m_pastObserverSnapshots;
};
}  // namespace frc
