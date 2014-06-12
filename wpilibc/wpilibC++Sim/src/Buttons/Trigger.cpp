/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "Buttons/Button.h"

#include "Buttons/HeldButtonScheduler.h"
#include "Buttons/PressedButtonScheduler.h"
#include "Buttons/ReleasedButtonScheduler.h"
#include "Buttons/ToggleButtonScheduler.h"
#include "Buttons/CancelButtonScheduler.h"

Trigger::Trigger() {
	m_table = NULL;
}

bool Trigger::Grab()
{
	if (Get())
		return true;
	else if (m_table != NULL)
	{
		//if (m_table->isConnected())//TODO is connected on button?
			return m_table->GetBoolean("pressed");
		/*else
			return false;*/
	}
	else
		return false;
}

void Trigger::WhenActive(Command *command)
{
	PressedButtonScheduler *pbs = new PressedButtonScheduler(Grab(), this, command);
	pbs->Start();
}

void Trigger::WhileActive(Command *command)
{
	HeldButtonScheduler *hbs = new HeldButtonScheduler(Grab(), this, command);
	hbs->Start();
}

void Trigger::WhenInactive(Command *command)
{
	ReleasedButtonScheduler *rbs = new ReleasedButtonScheduler(Grab(), this, command);
	rbs->Start();
}

void Trigger::CancelWhenActive(Command *command) {
	CancelButtonScheduler *cbs = new CancelButtonScheduler(Grab(), this, command);
	cbs->Start();
}

void Trigger::ToggleWhenActive(Command *command) {
	ToggleButtonScheduler *tbs = new ToggleButtonScheduler(Grab(), this, command);
	tbs->Start();
}

std::string Trigger::GetSmartDashboardType(){
	return "Button";
}

void Trigger::InitTable(ITable* table){
	m_table = table;
	if(m_table!=NULL){
		m_table->PutBoolean("pressed", Get());
	}
}

ITable* Trigger::GetTable(){
	return m_table;
}
