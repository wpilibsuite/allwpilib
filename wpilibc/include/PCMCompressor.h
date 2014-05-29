/*
 * PCMCompressor.h
 */

#ifndef PCMCOMPRESSOR_H_
#define PCMCOMPRESSOR_H_

#include "HAL/HAL.h"
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
class PCMCompressor: public SensorBase, public LiveWindowSendable, public ITableListener {
public:
	PCMCompressor(uint8_t module);
	PCMCompressor();
	~PCMCompressor();

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

#endif /* PCMCOMPRESSOR_H_ */
