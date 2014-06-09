/*
 * Compressor.h
 */

#ifndef Compressor_H_
#define Compressor_H_

#include "HAL/HAL.hpp"
#include "SensorBase.h"
#include "tables/ITableListener.h"
#include "LiveWindow/LiveWindowSendable.h"

/**
 * CAN pneumatic control module compressor
 *
 *  Created on: May 28, 2014
 *      Author: Thomas Clark
 *
 */
class Compressor: public SensorBase, public LiveWindowSendable, public ITableListener {
public:
	Compressor(uint8_t module);
	Compressor();
	~Compressor();

	void Start();
	void Stop();
	bool Enabled();

	bool GetPressureSwitchValue();
	
	float GetCompressorCurrent();

	void SetClosedLoopControl(bool on);
	bool GetClosedLoopControl();

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
