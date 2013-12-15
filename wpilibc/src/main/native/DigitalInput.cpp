/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "DigitalInput.h"
#include "DigitalModule.h"
#include "NetworkCommunication/UsageReporting.h"
#include "Resource.h"
#include "WPIErrors.h"

// TODO: This is not a good place for this...
Resource *interruptsResource = NULL;

/**
 * Create an instance of a DigitalInput.
 * Creates a digital input given a slot and channel. Common creation routine
 * for all constructors.
 */
void DigitalInput::InitDigitalInput(uint8_t moduleNumber, uint32_t channel)
{
	m_table = NULL;
	char buf[64];
	Resource::CreateResourceObject(&interruptsResource, interrupt_kNumSystems);
	if (!CheckDigitalModule(moduleNumber))
	{
		snprintf(buf, 64, "Digital Module %d", moduleNumber);
		wpi_setWPIErrorWithContext(ModuleIndexOutOfRange, buf);
		return;
	}
	if (!CheckDigitalChannel(channel))
	{
		snprintf(buf, 64, "Digital Channel %d", channel);
		wpi_setWPIErrorWithContext(ChannelIndexOutOfRange, buf);
		return;
	}
	m_channel = channel;
	m_module = DigitalModule::GetInstance(moduleNumber);
	m_module->AllocateDIO(channel, true);

	nUsageReporting::report(nUsageReporting::kResourceType_DigitalInput, channel, moduleNumber - 1);
}

/**
 * Create an instance of a Digital Input class.
 * Creates a digital input given a channel and uses the default module.
 *
 * @param channel The digital channel (1..14).
 */
DigitalInput::DigitalInput(uint32_t channel)
{
	InitDigitalInput(GetDefaultDigitalModule(), channel);
}

/**
 * Create an instance of a Digital Input class.
 * Creates a digital input given an channel and module.
 *
 * @param moduleNumber The digital module (1 or 2).
 * @param channel The digital channel (1..14).
 */
DigitalInput::DigitalInput(uint8_t moduleNumber, uint32_t channel)
{
	InitDigitalInput(moduleNumber, channel);
}

/**
 * Free resources associated with the Digital Input class.
 */
DigitalInput::~DigitalInput()
{
	if (StatusIsFatal()) return;
	if (m_interrupt != NULL)
	{
		int32_t status = 0;
		cleanInterrupts(m_interrupt, &status);
		wpi_setErrorWithContext(status, getHALErrorMessage(status));
		m_interrupt = NULL;
		interruptsResource->Free(m_interruptIndex);
	}
	m_module->FreeDIO(m_channel);
}

/*
 * Get the value from a digital input channel.
 * Retrieve the value of a single digital input channel from the FPGA.
 */
uint32_t DigitalInput::Get()
{
	if (StatusIsFatal()) return 0;
	return m_module->GetDIO(m_channel);
}

/**
 * @return The GPIO channel number that this object represents.
 */
uint32_t DigitalInput::GetChannel()
{
	return m_channel;
}

/**
 * @return The value to be written to the channel field of a routing mux.
 */
uint32_t DigitalInput::GetChannelForRouting()
{
	return DigitalModule::RemapDigitalChannel(GetChannel() - 1);
}

/**
 * @return The value to be written to the module field of a routing mux.
 */
uint32_t DigitalInput::GetModuleForRouting()
{
	if (StatusIsFatal()) return 0;
	return m_module->GetNumber() - 1;
}

/**
 * @return The value to be written to the analog trigger field of a routing mux.
 */
bool DigitalInput::GetAnalogTriggerForRouting()
{
	return false;
}

/**
 * Request interrupts asynchronously on this digital input.
 * @param handler The address of the interrupt handler function of type tInterruptHandler that
 * will be called whenever there is an interrupt on the digitial input port.
 * Request interrupts in synchronus mode where the user program interrupt handler will be
 * called when an interrupt occurs.
 * The default is interrupt on rising edges only.
 */
void DigitalInput::RequestInterrupts(InterruptHandlerFunction handler, void *param)
{
	if (StatusIsFatal()) return;
	uint32_t index = interruptsResource->Allocate("Async Interrupt");
	if (index == ~0ul)
	{
		CloneError(interruptsResource);
		return;
	}
	m_interruptIndex = index;

	 // Creates a manager too
	AllocateInterrupts(false);

	int32_t status = 0;
	requestInterrupts(m_interrupt, GetModuleForRouting(), GetChannelForRouting(),
					  GetAnalogTriggerForRouting(), &status);
	SetUpSourceEdge(true, false);
	attachInterruptHandler(m_interrupt, handler, param, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Request interrupts synchronously on this digital input.
 * Request interrupts in synchronus mode where the user program will have to explicitly
 * wait for the interrupt to occur.
 * The default is interrupt on rising edges only.
 */
void DigitalInput::RequestInterrupts()
{
	if (StatusIsFatal()) return;
	uint32_t index = interruptsResource->Allocate("Sync Interrupt");
	if (index == ~0ul)
	{
		CloneError(interruptsResource);
		return;
	}
	m_interruptIndex = index;

	AllocateInterrupts(true);

	int32_t status = 0;
	requestInterrupts(m_interrupt, GetModuleForRouting(), GetChannelForRouting(),
					  GetAnalogTriggerForRouting(), &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
	SetUpSourceEdge(true, false);
}

void DigitalInput::SetUpSourceEdge(bool risingEdge, bool fallingEdge)
{
	if (StatusIsFatal()) return;
	if (m_interrupt == NULL)
	{
		wpi_setWPIErrorWithContext(NullParameter, "You must call RequestInterrupts before SetUpSourceEdge");
		return;
	}
	if (m_interrupt != NULL)
	{
		int32_t status = 0;
		setInterruptUpSourceEdge(m_interrupt, risingEdge, fallingEdge, &status);
		wpi_setErrorWithContext(status, getHALErrorMessage(status));
	}
}

void DigitalInput::UpdateTable() {
	if (m_table != NULL) {
		m_table->PutBoolean("Value", Get());
	}
}

void DigitalInput::StartLiveWindowMode() {
	
}

void DigitalInput::StopLiveWindowMode() {
	
}

std::string DigitalInput::GetSmartDashboardType() {
	return "DigitalInput";
}

void DigitalInput::InitTable(ITable *subTable) {
	m_table = subTable;
	UpdateTable();
}

ITable * DigitalInput::GetTable() {
	return m_table;
}
