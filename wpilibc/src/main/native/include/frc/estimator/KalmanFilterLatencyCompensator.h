/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <array>
#include <functional>
#include <utility>
#include <vector>

#include <Eigen/Core>
#include <units/time.h>

namespace frc {

template <int States, int Inputs, int Outputs, typename KalmanFilterType>
class KalmanFilterLatencyCompensator {
 public:
  struct ObserverSnapshot {
    Eigen::Matrix<double, States, 1> xHat;
    Eigen::Matrix<double, States, States> errorCovariances;
    Eigen::Matrix<double, Inputs, 1> inputs;
    Eigen::Matrix<double, Outputs, 1> localMeasurements;

    ObserverSnapshot(const KalmanFilterType& observer,
                     const Eigen::Matrix<double, Inputs, 1>& u,
                     const Eigen::Matrix<double, Outputs, 1>& localY)
        : xHat(observer.Xhat()),
          errorCovariances(observer.P()),
          inputs(u),
          localMeasurements(localY) {}
  };

  void AddObserverState(const KalmanFilterType& observer,
                        Eigen::Matrix<double, Inputs, 1> u,
                        Eigen::Matrix<double, Outputs, 1> localY,
                        units::second_t timestamp) {
    // Add the new state into the vector.
    m_pastObserverSnapshots.emplace_back(timestamp,
                                         ObserverSnapshot{observer, u, localY});

    // Remove the oldest snapshot if the vector exceeds our maximum size.
    if (m_pastObserverSnapshots.size() > kMaxPastObserverStates) {
      m_pastObserverSnapshots.erase(m_pastObserverSnapshots.begin());
    }
  }

  template <int Rows>
  void ApplyPastMeasurement(
      KalmanFilterType* observer, units::second_t nominalDt,
      Eigen::Matrix<double, Rows, 1> y,
      std::function<void(const Eigen::Matrix<double, Inputs, 1>& u, const Eigen::Matrix<double, Rows, 1>& y)>
          globalMeasurementCorrect,
      units::second_t timestamp) {
    if (m_pastObserverSnapshots.size() == 0) {
      // State map was empty, which means that we got a measurement right at
      // startup. The only thing we can do is ignore the measurement.
      return;
    }

    // We will perform a binary search to find the index of the element in the
    // vector that has a timestamp that is equal to or greater than the vision
    // measurement timestamp.

    // This index starts at one because we use the previous state later on, and
    // we always want to have a "previous state".
    int low = 1;
    int high = m_pastObserverSnapshots.size() - 1;

    while (low != high) {
      int mid = (low + high) / 2.0;
      if (m_pastObserverSnapshots[mid].first < timestamp) {
        // This index and everything under it are less than the requested
        // timestamp. Therefore, we can discard them.
        low = mid + 1;
      } else {
        // t is at least as large as the element at this index. This means that
        // anything after it cannot be what we are looking for.
        high = mid;
      }
    }

    // We are simply assigning this index to a new variable to avoid confusion
    // with variable names.
    int index = low;

    // High and Low should be the same. The sampled timestamp is greater than or
    // equal to the vision pose timestamp. We will now find the entry which is
    // closest in time to the requested timestamp.

    size_t indexOfClosestEntry =
        units::math::abs(timestamp - m_pastObserverSnapshots[index - 1].first) <
                units::math::abs(timestamp -
                                 m_pastObserverSnapshots[index].first)
            ? index - 1
            : index;

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
        observer->SetP(snapshot.errorCovariances);
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
