/*
 * PCMCompressor.cpp
 */

#include "PCMCompressor.h"
#include "WPIErrors.h"

void PCMCompressor::InitCompressor(uint8_t module) {
	m_table = 0;
	m_pcm_pointer = initializeCompressor(module);

	SetClosedLoopControl(true);
}

/**
 * Constructor
 *
 * Uses the default solenoid module number
 */
PCMCompressor::PCMCompressor() {
	InitCompressor(GetDefaultSolenoidModule());
}

/**
 * Constructor
 *
 * @param module The module number to use (1 or 2)
 */
PCMCompressor::PCMCompressor(uint8_t module) {
	InitCompressor(module);
}

PCMCompressor::~PCMCompressor() {

}

/**
 *  Starts the compressor and disables automatic closed-loop control
 */
void PCMCompressor::Start() {
	SetClosedLoopControl(false);
	SetCompressor(true);
}

/**
 *  Stops the compressor and disables automatic closed-loop control
 */
void PCMCompressor::Stop() {
	SetClosedLoopControl(false);
	SetCompressor(false);
}

void PCMCompressor::SetCompressor(bool on) {
	int32_t status = 0;

	setCompressor(m_pcm_pointer, on, &status);

	if(status) {
		wpi_setWPIError(Timeout);
	}
}


/**
 * @return true if the compressor is on
 */
bool PCMCompressor::Enabled() {
	int32_t status = 0;
	bool value;

	value = getCompressor(m_pcm_pointer, &status);

	if(status) {
		wpi_setWPIError(Timeout);
	}

	return value;
}

/**
 * @return true if pressure is low
 */
bool PCMCompressor::GetPressureSwitchValue() {
	int32_t status = 0;
	bool value;

	value = getPressureSwitch(m_pcm_pointer, &status);

	if(status) {
		wpi_setWPIError(Timeout);
	}

	return value;
}


/**
 * @return The current through the compressor, in amps
 */
float PCMCompressor::GetCompressorCurrent() {
	int32_t status = 0;
	float value;

	value = getCompressorCurrent(m_pcm_pointer, &status);

	if(status) {
		wpi_setWPIError(Timeout);
	}

	return value;
}


/**
 * Enables or disables automatically turning the compressor on when the
 * pressure is low.
 */
void PCMCompressor::SetClosedLoopControl(bool on) {
	int32_t status = 0;

	setClosedLoopControl(m_pcm_pointer, on, &status);

	if(status) {
		wpi_setWPIError(Timeout);
	}
}

/**
 * Returns true if the compressor will automatically turn on when the
 * pressure is low.
 */
bool PCMCompressor::GetClosedLoopControl() {
	int32_t status = 0;
	bool value;

	value = getClosedLoopControl(m_pcm_pointer, &status);

	if(status) {
		wpi_setWPIError(Timeout);
	}

	return value;
}

void PCMCompressor::UpdateTable() {
	if(m_table) {
		m_table->PutBoolean("Enabled", Enabled());
		m_table->PutBoolean("Pressure switch", GetPressureSwitchValue());
	}
}

void PCMCompressor::StartLiveWindowMode() {
}

void PCMCompressor::StopLiveWindowMode() {
}

std::string PCMCompressor::GetSmartDashboardType() {
	return "PCMCompressor";
}

void PCMCompressor::InitTable(ITable *subTable) {
	m_table = subTable;
	UpdateTable();
}

ITable *PCMCompressor::GetTable() {
	return m_table;
}

void PCMCompressor::ValueChanged(ITable* source, const std::string& key, EntryValue value, bool isNew) {
	if(value.b) Start();
	else Stop();

}

