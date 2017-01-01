/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Ultrasonic.h"

#include "Counter.h"
#include "DigitalInput.h"
#include "DigitalOutput.h"
#include "HAL/HAL.h"
#include "LiveWindow/LiveWindow.h"
#include "Timer.h"
#include "Utility.h"
#include "WPIErrors.h"

using namespace frc;

// Time (sec) for the ping trigger pulse.
constexpr double Ultrasonic::kPingTime;
// Priority that the ultrasonic round robin task runs.
const int Ultrasonic::kPriority;
// Max time (ms) between readings.
constexpr double Ultrasonic::kMaxUltrasonicTime;
constexpr double Ultrasonic::kSpeedOfSoundInchesPerSec;
// automatic round robin mode
std::atomic<bool> Ultrasonic::m_automaticEnabled{false};
std::set<Ultrasonic*> Ultrasonic::m_sensors;
std::thread Ultrasonic::m_thread;

/**
 * Background task that goes through the list of ultrasonic sensors and pings
 * each one in turn. The counter is configured to read the timing of the
 * returned echo pulse.
 *
 * DANGER WILL ROBINSON, DANGER WILL ROBINSON:
 * This code runs as a task and assumes that none of the ultrasonic sensors
 * will change while it's running. Make sure to disable automatic mode before
 * touching the list.
 */
void Ultrasonic::UltrasonicChecker() {
  while (m_automaticEnabled) {
    for (auto& sensor : m_sensors) {
      if (!m_automaticEnabled) break;

      if (sensor->IsEnabled()) {
        sensor->m_pingChannel->Pulse(kPingTime);  // do the ping
      }

      Wait(0.1);  // wait for ping to return
    }
  }
}

/**
 * Initialize the Ultrasonic Sensor.
 *
 * This is the common code that initializes the ultrasonic sensor given that
 * there are two digital I/O channels allocated. If the system was running in
 * automatic mode (round robin) when the new sensor is added, it is stopped,
 * the sensor is added, then automatic mode is restored.
 */
void Ultrasonic::Initialize() {
  bool originalMode = m_automaticEnabled;
  SetAutomaticMode(false);  // kill task when adding a new sensor
  // link this instance on the list
  m_sensors.insert(this);

  m_counter.SetMaxPeriod(1.0);
  m_counter.SetSemiPeriodMode(true);
  m_counter.Reset();
  m_enabled = true;  // make it available for round robin scheduling
  SetAutomaticMode(originalMode);

  static int instances = 0;
  instances++;
  HAL_Report(HALUsageReporting::kResourceType_Ultrasonic, instances);
  LiveWindow::GetInstance()->AddSensor("Ultrasonic",
                                       m_echoChannel->GetChannel(), this);
}

/**
 * Create an instance of the Ultrasonic Sensor.
 *
 * This is designed to support the Daventech SRF04 and Vex ultrasonic
 * sensors.
 *
 * @param pingChannel The digital output channel that sends the pulse to
 *                    initiate the sensor sending the ping.
 * @param echoChannel The digital input channel that receives the echo. The
 *                    length of time that the echo is high represents the
 *                    round trip time of the ping, and the distance.
 * @param units       The units returned in either kInches or kMilliMeters
 */
Ultrasonic::Ultrasonic(int pingChannel, int echoChannel, DistanceUnit units)
    : m_pingChannel(std::make_shared<DigitalOutput>(pingChannel)),
      m_echoChannel(std::make_shared<DigitalInput>(echoChannel)),
      m_counter(m_echoChannel) {
  m_units = units;
  Initialize();
}

/**
 * Create an instance of an Ultrasonic Sensor from a DigitalInput for the echo
 * channel and a DigitalOutput for the ping channel.
 *
 * @param pingChannel The digital output object that starts the sensor doing a
 *                    ping. Requires a 10uS pulse to start.
 * @param echoChannel The digital input object that times the return pulse to
 *                    determine the range.
 * @param units       The units returned in either kInches or kMilliMeters
 */
Ultrasonic::Ultrasonic(DigitalOutput* pingChannel, DigitalInput* echoChannel,
                       DistanceUnit units)
    : m_pingChannel(pingChannel, NullDeleter<DigitalOutput>()),
      m_echoChannel(echoChannel, NullDeleter<DigitalInput>()),
      m_counter(m_echoChannel) {
  if (pingChannel == nullptr || echoChannel == nullptr) {
    wpi_setWPIError(NullParameter);
    m_units = units;
    return;
  }
  m_units = units;
  Initialize();
}

/**
 * Create an instance of an Ultrasonic Sensor from a DigitalInput for the echo
 * channel and a DigitalOutput for the ping channel.
 *
 * @param pingChannel The digital output object that starts the sensor doing a
 *                    ping. Requires a 10uS pulse to start.
 * @param echoChannel The digital input object that times the return pulse to
 *                    determine the range.
 * @param units       The units returned in either kInches or kMilliMeters
 */
Ultrasonic::Ultrasonic(DigitalOutput& pingChannel, DigitalInput& echoChannel,
                       DistanceUnit units)
    : m_pingChannel(&pingChannel, NullDeleter<DigitalOutput>()),
      m_echoChannel(&echoChannel, NullDeleter<DigitalInput>()),
      m_counter(m_echoChannel) {
  m_units = units;
  Initialize();
}

/**
 * Create an instance of an Ultrasonic Sensor from a DigitalInput for the echo
 * channel and a DigitalOutput for the ping channel.
 *
 * @param pingChannel The digital output object that starts the sensor doing a
 *                    ping. Requires a 10uS pulse to start.
 * @param echoChannel The digital input object that times the return pulse to
 *                    determine the range.
 * @param units       The units returned in either kInches or kMilliMeters
 */
Ultrasonic::Ultrasonic(std::shared_ptr<DigitalOutput> pingChannel,
                       std::shared_ptr<DigitalInput> echoChannel,
                       DistanceUnit units)
    : m_pingChannel(pingChannel),
      m_echoChannel(echoChannel),
      m_counter(m_echoChannel) {
  m_units = units;
  Initialize();
}

/**
 * Destructor for the ultrasonic sensor.
 *
 * Delete the instance of the ultrasonic sensor by freeing the allocated digital
 * channels. If the system was in automatic mode (round robin), then it is
 * stopped, then started again after this sensor is removed (provided this
 * wasn't the last sensor).
 */
Ultrasonic::~Ultrasonic() {
  bool wasAutomaticMode = m_automaticEnabled;
  SetAutomaticMode(false);

  // No synchronization needed because the background task is stopped.
  m_sensors.erase(this);

  if (!m_sensors.empty() && wasAutomaticMode) {
    SetAutomaticMode(true);
  }
}

/**
 * Turn Automatic mode on/off.
 *
 * When in Automatic mode, all sensors will fire in round robin, waiting a set
 * time between each sensor.
 *
 * @param enabling Set to true if round robin scheduling should start for all
 *                 the ultrasonic sensors. This scheduling method assures that
 *                 the sensors are non-interfering because no two sensors fire
 *                 at the same time. If another scheduling algorithm is
 *                 prefered, it can be implemented by pinging the sensors
 *                 manually and waiting for the results to come back.
 */
void Ultrasonic::SetAutomaticMode(bool enabling) {
  if (enabling == m_automaticEnabled) return;  // ignore the case of no change

  m_automaticEnabled = enabling;

  if (enabling) {
    /* Clear all the counters so no data is valid. No synchronization is needed
     * because the background task is stopped.
     */
    for (auto& sensor : m_sensors) {
      sensor->m_counter.Reset();
    }

    m_thread = std::thread(&Ultrasonic::UltrasonicChecker);

    // TODO: Currently, lvuser does not have permissions to set task priorities.
    // Until that is the case, uncommenting this will break user code that calls
    // Ultrasonic::SetAutomicMode().
    // m_task.SetPriority(kPriority);
  } else {
    // Wait for background task to stop running
    m_thread.join();

    /* Clear all the counters (data now invalid) since automatic mode is
     * disabled. No synchronization is needed because the background task is
     * stopped.
     */
    for (auto& sensor : m_sensors) {
      sensor->m_counter.Reset();
    }
  }
}

/**
 * Single ping to ultrasonic sensor.
 *
 * Send out a single ping to the ultrasonic sensor. This only works if automatic
 * (round robin) mode is disabled. A single ping is sent out, and the counter
 * should count the semi-period when it comes in. The counter is reset to make
 * the current value invalid.
 */
void Ultrasonic::Ping() {
  wpi_assert(!m_automaticEnabled);
  m_counter.Reset();  // reset the counter to zero (invalid data now)
  m_pingChannel->Pulse(
      kPingTime);  // do the ping to start getting a single range
}

/**
 * Check if there is a valid range measurement.
 *
 * The ranges are accumulated in a counter that will increment on each edge of
 * the echo (return) signal. If the count is not at least 2, then the range has
 * not yet been measured, and is invalid.
 */
bool Ultrasonic::IsRangeValid() const { return m_counter.Get() > 1; }

/**
 * Get the range in inches from the ultrasonic sensor.
 *
 * @return double Range in inches of the target returned from the ultrasonic
 *         sensor. If there is no valid value yet, i.e. at least one
 *         measurement hasn't completed, then return 0.
 */
double Ultrasonic::GetRangeInches() const {
  if (IsRangeValid())
    return m_counter.GetPeriod() * kSpeedOfSoundInchesPerSec / 2.0;
  else
    return 0;
}

/**
 * Get the range in millimeters from the ultrasonic sensor.
 *
 * @return double Range in millimeters of the target returned by the ultrasonic
 *         sensor. If there is no valid value yet, i.e. at least one
 *         measurement hasn't completed, then return 0.
 */
double Ultrasonic::GetRangeMM() const { return GetRangeInches() * 25.4; }

/**
 * Get the range in the current DistanceUnit for the PIDSource base object.
 *
 * @return The range in DistanceUnit
 */
double Ultrasonic::PIDGet() {
  switch (m_units) {
    case Ultrasonic::kInches:
      return GetRangeInches();
    case Ultrasonic::kMilliMeters:
      return GetRangeMM();
    default:
      return 0.0;
  }
}

void Ultrasonic::SetPIDSourceType(PIDSourceType pidSource) {
  if (wpi_assert(pidSource == PIDSourceType::kDisplacement)) {
    m_pidSource = pidSource;
  }
}

/**
 * Set the current DistanceUnit that should be used for the PIDSource base
 * object.
 *
 * @param units The DistanceUnit that should be used.
 */
void Ultrasonic::SetDistanceUnits(DistanceUnit units) { m_units = units; }

/**
 * Get the current DistanceUnit that is used for the PIDSource base object.
 *
 * @return The type of DistanceUnit that is being used.
 */
Ultrasonic::DistanceUnit Ultrasonic::GetDistanceUnits() const {
  return m_units;
}

void Ultrasonic::UpdateTable() {
  if (m_table != nullptr) {
    m_table->PutNumber("Value", GetRangeInches());
  }
}

void Ultrasonic::StartLiveWindowMode() {}

void Ultrasonic::StopLiveWindowMode() {}

std::string Ultrasonic::GetSmartDashboardType() const { return "Ultrasonic"; }

void Ultrasonic::InitTable(std::shared_ptr<ITable> subTable) {
  m_table = subTable;
  UpdateTable();
}

std::shared_ptr<ITable> Ultrasonic::GetTable() const { return m_table; }
