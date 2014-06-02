/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef __NETWORK_BUTTON_H__
#define __NETWORK_BUTTON_H__

#include "Buttons/Button.h"
#include <string>

class NetworkButton : public Button
{
public:
	NetworkButton(const char *tableName, const char *field);
	NetworkButton(ITable* table, const char *field);
	virtual ~NetworkButton() {}

	virtual bool Get();

private:
	ITable* m_netTable;
	std::string m_field;

};

#endif
