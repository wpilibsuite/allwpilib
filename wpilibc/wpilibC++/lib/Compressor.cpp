/*
 * Compressor.cpp
 */

#include "Compressor.h"
#include "WPIErrors.h"

void Compressor::InitCompressor(uint8_t module) {
	m_table = 0;
	m_pcm_pointer = initializeCompressor(module);

	SetClosedLoopControl(true);
}

/**
 * Constructor
 *
 * Uses the default solenoid module number
 */
Compressor::Compressor() {
	InitCompressor(GetDefaultSolenoidModule());
}

/**
 * Constructor
 *
 * @param module The module number to use (1 or 2)
 */
Compressor::Compressor(uint8_t module) {
	InitCompressor(module);
}

Compressor::~Compressor() {

}

/**
 *  Starts the compressor and disables automatic closed-loop control
 */
void Compressor::Start() {
	SetClosedLoopControl(false);
	SetCompressor(true);
}

/**
 *  Stops the compressor and disables automatic closed-loop control
 */
void Compressor::Stop() {
	SetClosedLoopControl(false);
	SetCompressor(false);
}

void Compressor::SetCompressor(bool on) {
	int32_t status = 0;

	setCompressor(m_pcm_pointer, on, &status);

	if(status) {
		wpi_setWPIError(Timeout);
	}
}


/**
 * @return true if the compressor is on
 */
bool Compressor::Enabled() {
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
bool Compressor::GetPressureSwitchValue() {
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
float Compressor::GetCompressorCurrent() {
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
void Compressor::SetClosedLoopControl(bool on) {
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
bool Compressor::GetClosedLoopControl() {
	int32_t status = 0;
	bool value;

	value = getClosedLoopControl(m_pcm_pointer, &status);

	if(status) {
		wpi_setWPIError(Timeout);
	}

	return value;
}

void Compressor::UpdateTable() {
	if(m_table) {
		m_table->PutBoolean("Enabled", Enabled());
		m_table->PutBoolean("Pressure switch", GetPressureSwitchValue());
	}
}

void Compressor::StartLiveWindowMode() {
}

void Compressor::StopLiveWindowMode() {
}

std::string Compressor::GetSmartDashboardType() {
	return "Compressor";
}

void Compressor::InitTable(ITable *subTable) {
	m_table = subTable;
	UpdateTable();
}

ITable *Compressor::GetTable() {
	return m_table;
}

void Compressor::ValueChanged(ITable* source, const std::string& key, EntryValue value, bool isNew) {
	if(value.b) Start();
	else Stop();

}

