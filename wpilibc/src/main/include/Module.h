/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef MODULE_H_
#define MODULE_H_

#include "SensorBase.h"
#include "NetworkCommunication/LoadOut.h"

#define kMaxModules	(nLoadOut::kModuleType_Solenoid * kMaxModuleNumber + (kMaxModuleNumber - 1))

class Module: public SensorBase
{
public:
	nLoadOut::tModuleType GetType() {return m_moduleType;}
	uint8_t GetNumber() {return m_moduleNumber;}
	static Module *GetModule(nLoadOut::tModuleType type, uint8_t number);

protected:
	explicit Module(nLoadOut::tModuleType type, uint8_t number);
	virtual ~Module();

	nLoadOut::tModuleType m_moduleType; ///< The type of module represented.
	uint8_t m_moduleNumber; ///< The module index within the module type.

private:
	static uint8_t ToIndex(nLoadOut::tModuleType type, uint8_t number);
	static Module* m_modules[kMaxModules];
};

#endif
