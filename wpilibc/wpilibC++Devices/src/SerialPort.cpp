/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "SerialPort.h"

//#include "NetworkCommunication/UsageReporting.h"
#include "visa/visa.h"
#include <stdarg.h>

//static ViStatus _VI_FUNCH ioCompleteHandler (ViSession vi, ViEventType eventType, ViEvent event, ViAddr userHandle);

/**
 * Create an instance of a Serial Port class.
 *
 * @param baudRate The baud rate to configure the serial port.
 * @param dataBits The number of data bits per transfer.  Valid values are between 5 and 8 bits.
 * @param parity Select the type of parity checking to use.
 * @param stopBits The number of stop bits to use as defined by the enum StopBits.
 */
SerialPort::SerialPort(uint32_t baudRate, uint8_t dataBits, SerialPort::Parity parity, SerialPort::StopBits stopBits)
	: m_resourceManagerHandle (0)
	, m_portHandle (0)
	, m_consoleModeEnabled (false)
{
	ViStatus localStatus = VI_SUCCESS;
	localStatus = viOpenDefaultRM((ViSession*)&m_resourceManagerHandle);
	wpi_setError(localStatus);

	localStatus = viOpen(m_resourceManagerHandle, const_cast<char*>("ASRL1::INSTR"), VI_NULL, VI_NULL, (ViSession*)&m_portHandle);
	wpi_setError(localStatus);
	if (localStatus != 0)
	{
		m_consoleModeEnabled = true;
		return;
	}

	localStatus = viSetAttribute(m_portHandle, VI_ATTR_ASRL_BAUD, baudRate);
	wpi_setError(localStatus);
	localStatus = viSetAttribute(m_portHandle, VI_ATTR_ASRL_DATA_BITS, dataBits);
	wpi_setError(localStatus);
	localStatus = viSetAttribute(m_portHandle, VI_ATTR_ASRL_PARITY, parity);
	wpi_setError(localStatus);
	localStatus = viSetAttribute(m_portHandle, VI_ATTR_ASRL_STOP_BITS, stopBits);
	wpi_setError(localStatus);

	// Set the default timeout to 5 seconds.
	SetTimeout(5.0f);

	// Don't wait until the buffer is full to transmit.
	SetWriteBufferMode(kFlushOnAccess);

	EnableTermination();

	//viInstallHandler(m_portHandle, VI_EVENT_IO_COMPLETION, ioCompleteHandler, this);
	//viEnableEvent(m_portHandle, VI_EVENT_IO_COMPLETION, VI_HNDLR, VI_NULL);

	HALReport(HALUsageReporting::kResourceType_SerialPort, 0);
}

/**
 * Destructor.
 */
SerialPort::~SerialPort()
{
	if (!m_consoleModeEnabled)
	{
		//viUninstallHandler(m_portHandle, VI_EVENT_IO_COMPLETION, ioCompleteHandler, this);
		viClose(m_portHandle);
	}
	viClose(m_resourceManagerHandle);
}

/**
 * Set the type of flow control to enable on this port.
 *
 * By default, flow control is disabled.
 */
void SerialPort::SetFlowControl(SerialPort::FlowControl flowControl)
{
	if (!m_consoleModeEnabled)
	{
		ViStatus localStatus = viSetAttribute (m_portHandle, VI_ATTR_ASRL_FLOW_CNTRL, flowControl);
		wpi_setError(localStatus);
	}
}

/**
 * Enable termination and specify the termination character.
 *
 * Termination is currently only implemented for receive.
 * When the the terminator is recieved, the Read() or Scanf() will return
 *   fewer bytes than requested, stopping after the terminator.
 *
 * @param terminator The character to use for termination.
 */
void SerialPort::EnableTermination(char terminator)
{
	if (!m_consoleModeEnabled)
	{
		viSetAttribute(m_portHandle, VI_ATTR_TERMCHAR_EN, VI_TRUE);
		viSetAttribute(m_portHandle, VI_ATTR_TERMCHAR, terminator);
		viSetAttribute(m_portHandle, VI_ATTR_ASRL_END_IN, VI_ASRL_END_TERMCHAR);
	}
}

/**
 * Disable termination behavior.
 */
void SerialPort::DisableTermination()
{
	if (!m_consoleModeEnabled)
	{
		viSetAttribute(m_portHandle, VI_ATTR_TERMCHAR_EN, VI_FALSE);
		viSetAttribute(m_portHandle, VI_ATTR_ASRL_END_IN, VI_ASRL_END_NONE);
	}
}

/**
 * Get the number of bytes currently available to read from the serial port.
 *
 * @return The number of bytes available to read.
 */
int32_t SerialPort::GetBytesReceived()
{
	int32_t bytes = 0;
	if (!m_consoleModeEnabled)
	{
		ViStatus localStatus = viGetAttribute(m_portHandle, VI_ATTR_ASRL_AVAIL_NUM, &bytes);
		wpi_setError(localStatus);
	}
	return bytes;
}

/**
 * Output formatted text to the serial port.
 *
 * @bug All pointer-based parameters seem to return an error.
 *
 * @param writeFmt A string that defines the format of the output.
 */
void SerialPort::Printf(const char *writeFmt, ...)
{
	if (!m_consoleModeEnabled)
	{
		va_list args;
		va_start (args, writeFmt);
		ViStatus localStatus = viVPrintf(m_portHandle, (ViString)writeFmt, args);
		va_end (args);
		wpi_setError(localStatus);
	}
}

/**
 * Input formatted text from the serial port.
 *
 * @bug All pointer-based parameters seem to return an error.
 *
 * @param readFmt A string that defines the format of the input.
 */
void SerialPort::Scanf(const char *readFmt, ...)
{
	if (!m_consoleModeEnabled)
	{
		va_list args;
		va_start (args, readFmt);
		ViStatus localStatus = viVScanf(m_portHandle, (ViString)readFmt, args);
		va_end (args);
		wpi_setError(localStatus);
	}
}

/**
 * Read raw bytes out of the buffer.
 *
 * @param buffer Pointer to the buffer to store the bytes in.
 * @param count The maximum number of bytes to read.
 * @return The number of bytes actually read into the buffer.
 */
uint32_t SerialPort::Read(char *buffer, int32_t count)
{
	uint32_t retCount = 0;
	if (!m_consoleModeEnabled)
	{
		ViStatus localStatus = viBufRead(m_portHandle, (ViPBuf)buffer, count, (ViPUInt32)&retCount);
		switch (localStatus)
		{
		case VI_SUCCESS_TERM_CHAR:
		case VI_SUCCESS_MAX_CNT:
		case VI_ERROR_TMO: // Timeout
			break;
		default:
			wpi_setError(localStatus);
		}
	}
	return retCount;
}

/**
 * Write raw bytes to the buffer.
 *
 * @param buffer Pointer to the buffer to read the bytes from.
 * @param count The maximum number of bytes to write.
 * @return The number of bytes actually written into the port.
 */
uint32_t SerialPort::Write(const char *buffer, int32_t count)
{
	uint32_t retCount = 0;
	if (!m_consoleModeEnabled)
	{
		ViStatus localStatus = viBufWrite(m_portHandle, (ViPBuf)buffer, count, (ViPUInt32)&retCount);
		wpi_setError(localStatus);
	}
	return retCount;
}

/**
 * Configure the timeout of the serial port.
 *
 * This defines the timeout for transactions with the hardware.
 * It will affect reads and very large writes.
 *
 * @param timeout The number of seconds to to wait for I/O.
 */
void SerialPort::SetTimeout(float timeout)
{
	if (!m_consoleModeEnabled)
	{
		ViStatus localStatus = viSetAttribute(m_portHandle, VI_ATTR_TMO_VALUE, (uint32_t)(timeout * 1e3));
		wpi_setError(localStatus);
	}
}

/**
 * Specify the size of the input buffer.
 *
 * Specify the amount of data that can be stored before data
 * from the device is returned to Read or Scanf.  If you want
 * data that is recieved to be returned immediately, set this to 1.
 *
 * It the buffer is not filled before the read timeout expires, all
 * data that has been received so far will be returned.
 *
 * @param size The read buffer size.
 */
void SerialPort::SetReadBufferSize(uint32_t size)
{
	if (!m_consoleModeEnabled)
	{
		ViStatus localStatus = viSetBuf(m_portHandle, VI_READ_BUF, size);
		wpi_setError(localStatus);
	}
}

/**
 * Specify the size of the output buffer.
 *
 * Specify the amount of data that can be stored before being
 * transmitted to the device.
 *
 * @param size The write buffer size.
 */
void SerialPort::SetWriteBufferSize(uint32_t size)
{
	if (!m_consoleModeEnabled)
	{
		ViStatus localStatus = viSetBuf(m_portHandle, VI_WRITE_BUF, size);
		wpi_setError(localStatus);
	}
}

/**
 * Specify the flushing behavior of the output buffer.
 *
 * When set to kFlushOnAccess, data is synchronously written to the serial port
 *   after each call to either Printf() or Write().
 *
 * When set to kFlushWhenFull, data will only be written to the serial port when
 *   the buffer is full or when Flush() is called.
 *
 * @param mode The write buffer mode.
 */
void SerialPort::SetWriteBufferMode(SerialPort::WriteBufferMode mode)
{
	if (!m_consoleModeEnabled)
	{
		ViStatus localStatus = viSetAttribute(m_portHandle, VI_ATTR_WR_BUF_OPER_MODE, mode);
		wpi_setError(localStatus);
	}
}

/**
 * Force the output buffer to be written to the port.
 *
 * This is used when SetWriteBufferMode() is set to kFlushWhenFull to force a
 * flush before the buffer is full.
 */
void SerialPort::Flush()
{
	if (!m_consoleModeEnabled)
	{
		ViStatus localStatus = viFlush(m_portHandle, VI_WRITE_BUF);
		wpi_setError(localStatus);
	}
}

/**
 * Reset the serial port driver to a known state.
 *
 * Empty the transmit and receive buffers in the device and formatted I/O.
 */
void SerialPort::Reset()
{
	if (!m_consoleModeEnabled)
	{
		ViStatus localStatus = viClear(m_portHandle);
		wpi_setError(localStatus);
	}
}

//void SerialPort::_internalHandler(uint32_t port, uint32_t eventType, uint32_t event)
//{
//}

//ViStatus _VI_FUNCH ioCompleteHandler (ViSession vi, ViEventType eventType, ViEvent event, ViAddr userHandle)
//{
//	((SerialPort*) userHandle)->_internalHandler(vi, eventType, event);
//	return VI_SUCCESS;
//}
