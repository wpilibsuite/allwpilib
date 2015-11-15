/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "AnalogOutput.h"
#include "Resource.h"
#include "WPIErrors.h"
#include "LiveWindow/LiveWindow.h"
#include "HAL/Port.h"

#include <limits>
#include <sstream>

static std::unique_ptr<Resource> outputs;

/**
 * Construct an analog output on the given channel.
 * All analog outputs are located on the MXP port.
 * @param The channel number on the roboRIO to represent.
 */
AnalogOutput::AnalogOutput(uint32_t channel) {
  Resource::CreateResourceObject(outputs, kAnalogOutputs);

  std::stringstream buf;
  buf << "analog input " << channel;

  if (!checkAnalogOutputChannel(channel)) {
    wpi_setWPIErrorWithContext(ChannelIndexOutOfRange, buf.str());
    m_channel = std::numeric_limits<uint32_t>::max();
    m_port = nullptr;
    return;
  }

  if (outputs->Allocate(channel, buf.str()) ==
      std::numeric_limits<uint32_t>::max()) {
    CloneError(*outputs);
    m_channel = std::numeric_limits<uint32_t>::max();
    m_port = nullptr;
    return;
  }

  m_channel = channel;

  void* port = getPort(m_channel);
  int32_t status = 0;
  m_port = initializeAnalogOutputPort(port, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
  freePort(port);

  LiveWindow::GetInstance()->AddActuator("AnalogOutput", m_channel, this);
  HALReport(HALUsageReporting::kResourceType_AnalogOutput, m_channel);
}

/**
 * Destructor. Frees analog output resource
 */
AnalogOutput::~AnalogOutput() {
  freeAnalogOutputPort(m_port);
  outputs->Free(m_channel);
}

/**
 * Set the value of the analog output
 *
 * @param voltage The output value in Volts, from 0.0 to +5.0
 */
void AnalogOutput::SetVoltage(float voltage) {
  int32_t status = 0;
  setAnalogOutput(m_port, voltage, &status);

  wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Get the voltage of the analog output
 *
 * @return The value in Volts, from 0.0 to +5.0
 */
float AnalogOutput::GetVoltage() const {
  int32_t status = 0;
  float voltage = getAnalogOutput(m_port, &status);

  wpi_setErrorWithContext(status, getHALErrorMessage(status));

  return voltage;
}

void AnalogOutput::UpdateTable() {
  if (m_table != nullptr) {
    m_table->PutNumber("Value", GetVoltage());
  }
}

void AnalogOutput::StartLiveWindowMode() {}

void AnalogOutput::StopLiveWindowMode() {}

std::string AnalogOutput::GetSmartDashboardType() const {
  return "Analog Output";
}

void AnalogOutput::InitTable(std::shared_ptr<ITable> subTable) {
  m_table = subTable;
  UpdateTable();
}

std::shared_ptr<ITable> AnalogOutput::GetTable() const { return m_table; }
