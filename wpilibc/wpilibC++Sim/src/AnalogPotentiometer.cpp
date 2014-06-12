
#include "AnalogPotentiometer.h"

void AnalogPotentiometer::initPot(int slot, int channel, double scale, double offset) {
    char buffer[50];
    int n = sprintf(buffer, "analog/%d/%d", slot, channel);
    impl = new SimFloatInput(buffer);
    this->module = slot;
    this->channel = channel;
}

AnalogPotentiometer::AnalogPotentiometer(int slot, int channel, double scale, double offset) {
    initPot(slot, channel, scale, offset);
}

AnalogPotentiometer::AnalogPotentiometer(int channel, double scale, double offset) {
    initPot(1, channel, scale, offset);
}

AnalogPotentiometer::AnalogPotentiometer(int channel, double scale) {
    initPot(1, channel, scale, 0);
}

AnalogPotentiometer::AnalogPotentiometer(int channel) {
    initPot(1, channel, 1, 0);
}


double AnalogPotentiometer::Get() {
    return impl->Get();
}
    
double AnalogPotentiometer::PIDGet() {
    return Get();
}

std::string AnalogPotentiometer::GetSmartDashboardType() {
    return "Analog Input";
}

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
