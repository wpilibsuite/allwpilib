#include "LiveWindow/LiveWindow.h"
#include "networktables/NetworkTable.h"
#include <algorithm>
#include <sstream>

/**
 * Get an instance of the LiveWindow main class
 * This is a singleton to guarantee that there is only a single instance regardless of
 * how many times GetInstance is called.
 */
LiveWindow * LiveWindow::GetInstance()
{
	static LiveWindow* instance = new LiveWindow();

	return instance;
}

/**
 * LiveWindow constructor.
 * Allocate the necessary tables.
 */
LiveWindow::LiveWindow()
{
	m_enabled = false;
	m_liveWindowTable = NetworkTable::GetTable("LiveWindow");
	m_statusTable = m_liveWindowTable->GetSubTable("~STATUS~");
	m_scheduler = Scheduler::GetInstance();
}

/**
 * Change the enabled status of LiveWindow
 * If it changes to enabled, start livewindow running otherwise stop it
 */
void LiveWindow::SetEnabled(bool enabled)
{
	if (m_enabled == enabled)
		return;
	if (enabled)
	{
		if (m_firstTime)
		{
			InitializeLiveWindowComponents();
			m_firstTime = false;
		}
		m_scheduler->SetEnabled(false);
		m_scheduler->RemoveAll();
		for (std::map<LiveWindowSendable *, LiveWindowComponent>::iterator it =
				m_components.begin(); it != m_components.end(); ++it)
		{
			it->first->StartLiveWindowMode();
		}
	}
	else
	{
		for (std::map<LiveWindowSendable *, LiveWindowComponent>::iterator it =
				m_components.begin(); it != m_components.end(); ++it)
		{
			it->first->StopLiveWindowMode();
		}
		m_scheduler->SetEnabled(true);
	}
	m_enabled = enabled;
	m_statusTable->PutBoolean("LW Enabled", m_enabled);
}

LiveWindow::~LiveWindow()
{
}

/**
 * Add a Sensor associated with the subsystem and with call it by the given name.
 * @param subsystem The subsystem this component is part of.
 * @param name The name of this component.
 * @param component A LiveWindowSendable component that represents a sensor.
 */
void LiveWindow::AddSensor(const char *subsystem, const char *name,
		LiveWindowSendable *component)
{
	m_components[component].subsystem = subsystem;
	m_components[component].name = name;
	m_components[component].isSensor = true;
}

/**
 * Add an Actuator associated with the subsystem and with call it by the given name.
 * @param subsystem The subsystem this component is part of.
 * @param name The name of this component.
 * @param component A LiveWindowSendable component that represents a actuator.
 */
void LiveWindow::AddActuator(const char *subsystem, const char *name,
		LiveWindowSendable *component)
{
	m_components[component].subsystem = subsystem;
	m_components[component].name = name;
	m_components[component].isSensor = false;
}

/**
 * INTERNAL
 */
void LiveWindow::AddSensor(std::string type, int channel, LiveWindowSendable *component)
{
	std::ostringstream oss;
	oss << type << "[" << channel << "]";
	std::string types(oss.str());
	char* cc = new char[types.size() + 1];
	types.copy(cc, types.size());
	cc[types.size()]='\0';
	AddSensor("Ungrouped", cc, component);
	if (std::find(m_sensors.begin(), m_sensors.end(), component) == m_sensors.end())
		m_sensors.push_back(component);
}

/**
 * INTERNAL
 */
void LiveWindow::AddActuator(std::string type, int channel, LiveWindowSendable *component)
{
	std::ostringstream oss;
	oss << type << "[" << channel << "]";
	std::string types(oss.str());
	char* cc = new char[types.size() + 1];
	types.copy(cc, types.size());
	cc[types.size()]='\0';
	AddActuator("Ungrouped", cc, component);
}

/**
 * INTERNAL
 */
void LiveWindow::AddActuator(std::string type, int module, int channel, LiveWindowSendable *component)
{
    std::ostringstream oss;
    oss << type << "[" << module << "," << channel << "]";
    std::string types(oss.str());
    char* cc = new char[types.size() + 1];
    types.copy(cc, types.size());
    cc[types.size()]='\0';
    AddActuator("Ungrouped", cc, component);
}

/**
 * Tell all the sensors to update (send) their values
 * Actuators are handled through callbacks on their value changing from the
 * SmartDashboard widgets.
 */
void LiveWindow::UpdateValues()
{
	for (unsigned int i = 0; i < m_sensors.size(); i++)
	{
		m_sensors[i]->UpdateTable();
	}
}

/**
 * This method is called periodically to cause the sensors to send new values
 * to the SmartDashboard.
 */
void LiveWindow::Run()
{
	if (m_enabled)
	{
		UpdateValues();
	}
}

/**
 * Initialize all the LiveWindow elements the first time we enter LiveWindow mode.
 * By holding off creating the NetworkTable entries, it allows them to be redefined
 * before the first time in LiveWindow mode. This allows default sensor and actuator
 * values to be created that are replaced with the custom names from users calling
 * addActuator and addSensor.
 */
void LiveWindow::InitializeLiveWindowComponents()
{
	for (std::map<LiveWindowSendable *, LiveWindowComponent>::iterator it =
			m_components.begin(); it != m_components.end(); ++it)
	{
		LiveWindowSendable *component = it->first;
		LiveWindowComponent c = it->second;
		std::string subsystem = c.subsystem;
		std::string name = c.name;
		m_liveWindowTable->GetSubTable(subsystem)->PutString("~TYPE~",
				"LW Subsystem");
		ITable *table = m_liveWindowTable->GetSubTable(subsystem)->GetSubTable(
				name);
		table->PutString("~TYPE~", component->GetSmartDashboardType());
		table->PutString("Name", name);
		table->PutString("Subsystem", subsystem);
		component->InitTable(table);
		if (c.isSensor)
		{
			m_sensors.push_back(component);
		}
	}
}
