
#include "AnalogPotentiometer.h"
#include "ControllerPower.h"

/**
 * Common initialization code called by all constructors.
 */
void AnalogPotentiometer::initPot(AnalogInput *input, double fullRange, double offset) {
    m_fullRange = fullRange;
    m_offset = offset;
    m_analog_input = input;
}

/**
 * Construct an Analog Potentiometer object from a channel number.
 * @param channel The channel number on the roboRIO to represent. 0-3 are on-board 4-7 are on the MXP port.
 * @param fullRange The angular value (in desired units) representing the full 0-5V range of the input.
 * @param offset The angular value (in desired units) representing the angular output at 0V.
 */
AnalogPotentiometer::AnalogPotentiometer(int channel, double fullRange, double offset) {
    m_init_analog_input = true;
    initPot(new AnalogInput(channel), fullRange, offset);
}

/**
 * Construct an Analog Potentiometer object from an existing Analog Input pointer.
 * @param channel The existing Analog Input pointer 
 * @param fullRange The angular value (in desired units) representing the full 0-5V range of the input.
 * @param offset The angular value (in desired units) representing the angular output at 0V.
 */
AnalogPotentiometer::AnalogPotentiometer(AnalogInput *input, double fullRange, double offset) {
    m_init_analog_input = false;
    initPot(input, fullRange, offset);
}

/**
 * Construct an Analog Potentiometer object from an existing Analog Input reference.
 * @param channel The existing Analog Input reference 
 * @param fullRange The angular value (in desired units) representing the full 0-5V range of the input.
 * @param offset The angular value (in desired units) representing the angular output at 0V.
 */
AnalogPotentiometer::AnalogPotentiometer(AnalogInput &input, double fullRange, double offset) {
    m_init_analog_input = false;
    initPot(&input, fullRange, offset);
}

/**
 * Destructor. Releases the Analog Input resource if it was allocated by this object
 */
AnalogPotentiometer::~AnalogPotentiometer() {
  if(m_init_analog_input){
    delete m_analog_input;
    m_init_analog_input = false;
  }
}

/**
 * Get the current reading of the potentiometer.
 *
 * @return The current position of the potentiometer (in the units used for fullRaneg and offset).
 */
double AnalogPotentiometer::Get() {
    return (m_analog_input->GetVoltage() / ControllerPower::GetVoltage5V()) * m_fullRange + m_offset;
}

/**
 * Implement the PIDSource interface.
 *
 * @return The current reading.
 */
double AnalogPotentiometer::PIDGet() {
    return Get();
}


/**
 * @return the Smart Dashboard Type
 */
std::string AnalogPotentiometer::GetSmartDashboardType() {
    return "Analog Input";
}

/**
 * Live Window code, only does anything if live window is activated.
 */
void AnalogPotentiometer::InitTable(ITable *subtable) {
    m_table = subtable;
    UpdateTable();
}

void AnalogPotentiometer::UpdateTable() {
    if (m_table != NULL) {
        m_table->PutNumber("Value", Get());
    }
}

ITable* AnalogPotentiometer::GetTable() {
    return m_table;
}
