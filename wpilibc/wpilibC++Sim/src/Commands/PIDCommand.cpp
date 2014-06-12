/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "Commands/PIDCommand.h"

#include "PIDController.h"
#include "float.h"

PIDCommand::PIDCommand(const char *name, double p, double i, double d, double f, double period) :
		Command(name)
{
	m_controller = new PIDController(p, i, d, this, this, period);
}

PIDCommand::PIDCommand(double p, double i, double d, double f, double period)
{
	m_controller = new PIDController(p, i, d, f, this, this, period);
}

PIDCommand::PIDCommand(const char *name, double p, double i, double d) :
	Command(name)
{
	m_controller = new PIDController(p, i, d, this, this);
}

PIDCommand::PIDCommand(const char *name, double p, double i, double d, double period) :
	Command(name)
{
	m_controller = new PIDController(p, i, d, this, this, period);
}

PIDCommand::PIDCommand(double p, double i, double d)
{
	m_controller = new PIDController(p, i, d, this, this);
}

PIDCommand::PIDCommand(double p, double i, double d, double period)
{
	m_controller = new PIDController(p, i, d, this, this, period);
}

PIDCommand::~PIDCommand()
{
	delete m_controller;
}

void PIDCommand::_Initialize()
{
	m_controller->Enable();
}

void PIDCommand::_End()
{
	m_controller->Disable();
}

void PIDCommand::_Interrupted()
{
	_End();
}

void PIDCommand::SetSetpointRelative(double deltaSetpoint)
{
	SetSetpoint(GetSetpoint() + deltaSetpoint);
}

void PIDCommand::PIDWrite(float output)
{
	UsePIDOutput(output);
}

double PIDCommand::PIDGet()
{
	return ReturnPIDInput();
}

PIDController *PIDCommand::GetPIDController()
{
	return m_controller;
}

void PIDCommand::SetSetpoint(double setpoint)
{
	m_controller->SetSetpoint(setpoint);
}

double PIDCommand::GetSetpoint()
{
	return m_controller->GetSetpoint();
}

double PIDCommand::GetPosition()
{
	return ReturnPIDInput();
}

std::string PIDCommand::GetSmartDashboardType(){
        return "PIDCommand";
}
void PIDCommand::InitTable(ITable* table){
	m_controller->InitTable(table);
	Command::InitTable(table);
}
