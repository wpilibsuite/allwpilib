
#include "simulation/SimFloatInput.h"
#include "interfaces/Potentiometer.h"
#include "SensorBase.h"
#include "LiveWindow/LiveWindowSendable.h" 

/**
 * A class for reading analog potentiometers.
 * 
 * @author Alex Henning
 */
class AnalogPotentiometer : public SensorBase, public Potentiometer, public LiveWindowSendable {
public:
    AnalogPotentiometer(int slot, int channel, double scale, double offset);
    AnalogPotentiometer(int channel, double scale, double offset);
    AnalogPotentiometer(int channel, double scale);
    AnalogPotentiometer(int channel);

    virtual double Get();
    virtual double PIDGet();
    
    /*
     * Live Window code, only does anything if live window is activated.
     */
    virtual std::string GetSmartDashboardType();
    virtual void InitTable(ITable *subtable);
    virtual void UpdateTable();
    virtual ITable* GetTable();

    /**
     * AnalogPotentiometers don't have to do anything special when entering the LiveWindow.
     */
    virtual void StartLiveWindowMode() {}
    
    /**
     * AnalogPotentiometers don't have to do anything special when exiting the LiveWindow.
     */
    virtual void StopLiveWindowMode() {}
  
private:
    int module, channel;
    SimFloatInput* impl;
    ITable* m_table;

    /**
     * Common initialization code called by all constructors.
     */
    void initPot(int slot, int channel, double scale, double offset);
};
