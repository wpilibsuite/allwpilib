/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "HAL/HAL.hpp"
#include "SensorBase.h"
#include "LiveWindow/LiveWindowSendable.h"

/**
 * MXP analog output class.
 */
class AnalogOutput : public SensorBase, public LiveWindowSendable
{
public:
    explicit AnalogOutput(uint32_t channel);
    virtual ~AnalogOutput();

    void SetVoltage(float voltage);
    float GetVoltage();

    void UpdateTable();
    void StartLiveWindowMode();
    void StopLiveWindowMode();
    std::string GetSmartDashboardType();
    void InitTable(ITable *subTable);
    ITable *GetTable();

protected:
    void InitAnalogOutput(uint32_t channel);
    uint32_t m_channel;
    void* m_port;

    ITable *m_table;
};
