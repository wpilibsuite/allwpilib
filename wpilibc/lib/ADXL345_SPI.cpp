/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "ADXL345_SPI.h"
#include "DigitalInput.h"
#include "DigitalOutput.h"
//#include "NetworkCommunication/UsageReporting.h"
#include "SPI.h"

const uint8_t ADXL345_SPI::kPowerCtlRegister;
const uint8_t ADXL345_SPI::kDataFormatRegister;
const uint8_t ADXL345_SPI::kDataRegister;
constexpr double ADXL345_SPI::kGsPerLSB;

/**
 * Constructor.
 * 
 * @param clk The GPIO the clock signal is wired to.
 * @param mosi The GPIO the MOSI (Master Out Slave In) signal is wired to.
 * @param miso The GPIO the MISO (Master In Slave Out) signal is wired to.
 * @param cs The GPIO the CS (Chip Select) signal is wired to.
 * @param range The range (+ or -) that the accelerometer will measure.
 */
ADXL345_SPI::ADXL345_SPI(DigitalOutput &clk, DigitalOutput &mosi, DigitalInput &miso,
	DigitalOutput &cs, DataFormat_Range range)
	: m_clk (NULL)
	, m_mosi (NULL)
	, m_miso (NULL)
	, m_cs (NULL)
	, m_spi (NULL)
{
	Init(&clk, &mosi, &miso, &cs, range);
}

/**
 * Constructor.
 * 
 * @param clk The GPIO the clock signal is wired to.
 * @param mosi The GPIO the MOSI (Master Out Slave In) signal is wired to.
 * @param miso The GPIO the MISO (Master In Slave Out) signal is wired to.
 * @param cs The GPIO the CS (Chip Select) signal is wired to.
 * @param range The range (+ or -) that the accelerometer will measure.
 */
ADXL345_SPI::ADXL345_SPI(DigitalOutput *clk, DigitalOutput *mosi, DigitalInput *miso,
	DigitalOutput *cs, DataFormat_Range range)
	: m_clk (NULL)
	, m_mosi (NULL)
	, m_miso (NULL)
	, m_cs (NULL)
	, m_spi (NULL)
{
	Init(clk, mosi, miso, cs, range);
}

/**
 * Constructor.
 * 
 * @param moduleNumber The digital module with the sensor attached.
 * @param clk The GPIO the clock signal is wired to.
 * @param mosi The GPIO the MOSI (Master Out Slave In) signal is wired to.
 * @param miso The GPIO the MISO (Master In Slave Out) signal is wired to.
 * @param cs The GPIO the CS (Chip Select) signal is wired to.
 * @param range The range (+ or -) that the accelerometer will measure.
 */
ADXL345_SPI::ADXL345_SPI(uint8_t moduleNumber, uint32_t clk, uint32_t mosi, uint32_t miso,
		uint32_t cs, ADXL345_SPI::DataFormat_Range range)
	: m_clk (NULL)
	, m_mosi (NULL)
	, m_miso (NULL)
	, m_cs (NULL)
	, m_spi (NULL)
{
	m_clk = new DigitalOutput(moduleNumber, clk);
	m_mosi = new DigitalOutput(moduleNumber, mosi);
	m_miso = new DigitalInput(moduleNumber, miso);
	m_cs = new DigitalOutput(moduleNumber, cs);
	Init(m_clk, m_mosi, m_miso, m_cs, range);
}

/**
 * Internal common init function.
 */
void ADXL345_SPI::Init(DigitalOutput *clk, DigitalOutput *mosi, DigitalInput *miso,
	DigitalOutput *cs, DataFormat_Range range)
{
	if (clk != NULL && mosi != NULL && miso != NULL && cs != NULL)
	{
		m_spi = new SPI(clk, mosi, miso);
		m_spi->SetMSBFirst();
		m_spi->SetSampleDataOnRising();
		m_spi->SetSlaveSelect(cs, kChipSelect, false);
		m_spi->SetClockActiveLow();
		// 8-bit address and 8-bit data
		m_spi->SetBitsPerWord(16);
		m_spi->ApplyConfig();
		m_spi->ClearReceivedData();

		// Turn on the measurements
		m_spi->Write((kPowerCtlRegister << 8) | kPowerCtl_Measure);
		m_spi->Read();
		// Specify the data format to read
		m_spi->Write((kDataFormatRegister << 8) | kDataFormat_FullRes | (uint8_t)(range & 0x03));
		m_spi->Read();

		// 8-bit address and 16-bit data
		m_spi->SetBitsPerWord(24);
		m_spi->ApplyConfig();

		HALReport(HALUsageReporting::kResourceType_ADXL345, HALUsageReporting::kADXL345_SPI);
	}
}

/**
 * Destructor.
 */
ADXL345_SPI::~ADXL345_SPI()
{
	delete m_spi;
	m_spi = NULL;
	delete m_cs;
	m_cs = NULL;
	delete m_miso;
	m_miso = NULL;
	delete m_mosi;
	m_mosi = NULL;
	delete m_clk;
	m_clk = NULL;
}

/**
 * Get the acceleration of one axis in Gs.
 * 
 * @param axis The axis to read from.
 * @return Acceleration of the ADXL345 in Gs.
 */
double ADXL345_SPI::GetAcceleration(ADXL345_SPI::Axes axis)
{
	int16_t rawAccel = 0;
	if(m_spi)
	{
		m_spi->Write(((kAddress_Read | kAddress_MultiByte | kDataRegister) + (uint8_t)axis) << 16);
		rawAccel = (uint16_t)m_spi->Read();

		// Sensor is little endian... swap bytes
		rawAccel = ((rawAccel >> 8) & 0xFF) | (rawAccel << 8);
	}
	return rawAccel * kGsPerLSB;
}

/**
 * Get the acceleration of all axes in Gs.
 * 
 * @return Acceleration measured on all axes of the ADXL345 in Gs.
 */
ADXL345_SPI::AllAxes ADXL345_SPI::GetAccelerations()
{
	AllAxes data = AllAxes();
	int16_t rawData[3];
	if (m_spi)
	{
		tFrameMode mode;
		bool activeLow;

		// Backup original settings.
		DigitalOutput *cs = m_spi->GetSlaveSelect(&mode, &activeLow);
		uint32_t bitsPerWord = m_spi->GetBitsPerWord();

		// Initialize the chip select to inactive.
		cs->Set(activeLow);

		// Control the chip select manually.
		m_spi->SetSlaveSelect(NULL);
		// 8-bit address
		m_spi->SetBitsPerWord(8);
		m_spi->ApplyConfig();

		// Assert chip select.
		cs->Set(!activeLow);

		// Select the data address.
		m_spi->Write(kAddress_Read | kAddress_MultiByte | kDataRegister);
		m_spi->Read();

		// 16-bits for each axis
		m_spi->SetBitsPerWord(16);
		m_spi->ApplyConfig();

		for (int32_t i=0; i<3; i++)
		{
			// SPI Interface can't read enough data in a single transaction to read all axes at once.
			rawData[i] = (uint16_t)m_spi->Read(true);
			// Sensor is little endian... swap bytes
			rawData[i] = ((rawData[i] >> 8) & 0xFF) | (rawData[i] << 8);
		}

		// Deassert chip select.
		cs->Set(activeLow);

		// Restore original settings.
		m_spi->SetSlaveSelect(cs, mode, activeLow);
		m_spi->SetBitsPerWord(bitsPerWord);
		m_spi->ApplyConfig();

		data.XAxis = rawData[0] * kGsPerLSB;
		data.YAxis = rawData[1] * kGsPerLSB;
		data.ZAxis = rawData[2] * kGsPerLSB;
	}
	return data;
}

