/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef __SENDABLE_CHOOSER_H__
#define __SENDABLE_CHOOSER_H__

#include "SmartDashboard/Sendable.h"
#include "tables/ITable.h"
#include <map>
#include <string>

/**
 * The {@link SendableChooser} class is a useful tool for presenting a selection of options
 * to the {@link SmartDashboard}.
 *
 * <p>For instance, you may wish to be able to select between multiple autonomous modes.
 * You can do this by putting every possible {@link Command} you want to run as an autonomous into
 * a {@link SendableChooser} and then put it into the {@link SmartDashboard} to have a list of options
 * appear on the laptop.  Once autonomous starts, simply ask the {@link SendableChooser} what the selected
 * value is.</p>
 *
 * @see SmartDashboard
 */
class SendableChooser : public Sendable
{
public:
	SendableChooser();
	virtual ~SendableChooser() {};

	void AddObject(const char *name, void *object);
	void AddDefault(const char *name, void *object);
	void *GetSelected();

	virtual void InitTable(ITable* subtable);
	virtual ITable* GetTable();
	virtual std::string GetSmartDashboardType();

private:
	std::string m_defaultChoice;
	std::map<std::string, void *> m_choices;
	ITable *m_table;
};

#endif
