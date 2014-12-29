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

static Resource *outputs = NULL;

void AnalogOutput::InitAnalogOutput(uint32_t channel) {
    m_table = NULL;

    Resource::CreateResourceObject(&outputs, kAnalogOutputs);

    char buf[64];

    if(!checkAnalogOutputChannel(channel))
    {
        snprintf(buf, 64, "analog input %d", channel);
        wpi_setWPIErrorWithContext(ChannelIndexOutOfRange, buf);
        return;
    }

    if(outputs->Allocate(channel, buf) == ~0ul)
    {
        CloneError(outputs);
        return;
    }

    m_channel = channel;

    void* port = getPort(m_channel);
    int32_t status = 0;
    m_port = initializeAnalogOutputPort(port, &status);
    wpi_setErrorWithContext(status, getHALErrorMessage(status));

    LiveWindow::GetInstance()->AddActuator("AnalogOutput", m_channel, this);
    HALReport(HALUsageReporting::kResourceType_AnalogChannel, m_channel, 0);
}

/**
 * Construct an analog output on the given channel.
 * All analog outputs are located on the MXP port.
 * @param The channel number on the roboRIO to represent.
 */
AnalogOutput::AnalogOutput(uint32_t channel) {
    InitAnalogOutput(channel);
}

/**
 * Destructor. Frees analog output resource
 */
AnalogOutput::~AnalogOutput() {
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
float AnalogOutput::GetVoltage() {
    int32_t status = 0;
    float voltage = getAnalogOutput(m_port, &status);

    wpi_setErrorWithContext(status, getHALErrorMessage(status));

    return voltage;
}

void AnalogOutput::UpdateTable() {
    if (m_table != NULL) {
        m_table->PutNumber("Value", GetVoltage());
    }
}

void AnalogOutput::StartLiveWindowMode() {
}

void AnalogOutput::StopLiveWindowMode() {
}

std::string AnalogOutput::GetSmartDashboardType() {
    return "Analog Output";
}

void AnalogOutput::InitTable(ITable *subTable) {
    m_table = subTable;
    UpdateTable();
}

ITable *AnalogOutput::GetTable() {
    return m_table;
}
