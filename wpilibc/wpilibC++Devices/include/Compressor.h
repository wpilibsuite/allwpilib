/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef Compressor_H_
#define Compressor_H_

#include "HAL/HAL.hpp"
#include "SensorBase.h"
#include "tables/ITableListener.h"
#include "LiveWindow/LiveWindowSendable.h"

/**
 * PCM compressor
 */
class Compressor: public SensorBase, public LiveWindowSendable, public ITableListener {
public:
	explicit Compressor(uint8_t pcmID);
	Compressor();
	virtual ~Compressor();

	void Start();
	void Stop();
	bool Enabled();

	bool GetPressureSwitchValue();

	float GetCompressorCurrent();

	void SetClosedLoopControl(bool on);
	bool GetClosedLoopControl();

	bool GetCompressorCurrentTooHighFault();
	bool GetCompressorCurrentTooHighStickyFault();
	bool GetCompressorShortedStickyFault();
	bool GetCompressorShortedFault();
	bool GetCompressorNotConnectedStickyFault();
	bool GetCompressorNotConnectedFault();
	void ClearAllPCMStickyFaults();

	void UpdateTable();
	void StartLiveWindowMode();
	void StopLiveWindowMode();
	std::string GetSmartDashboardType();
	void InitTable(ITable *subTable);
	ITable *GetTable();
	void ValueChanged(ITable* source, const std::string& key, EntryValue value, bool isNew);

protected:
	void *m_pcm_pointer;

private:
	void InitCompressor(uint8_t module);
	void SetCompressor(bool on);

	ITable *m_table;
};

#endif /* Compressor_H_ */
