/*
* Copyright (C) Cross The Road Electronics.  All rights reserved.
* License information can be found in CTRE_LICENSE.txt
* For support and suggestions contact support@ctr-electronics.com or file
* an issue tracker at https://github.com/CrossTheRoadElec/Phoenix-Releases
*/

package edu.wpi.first.wpilibj.motorcontrol;

import java.util.ArrayList;
import java.util.List;

import edu.wpi.first.hal.HAL;
import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.wpilibj.DriverStation;
import edu.wpi.first.wpilibj.PWM;
import edu.wpi.first.wpilibj.motorcontrol.PWMMotorController;
import edu.wpi.first.wpilibj.Timer;

/**
 * PWM class for Talon FXS.
 * This class has extensions that allow basic configuration over PWM when the
 * robot controller
 * is enabled.
 */
public class PWMTalonFXS extends PWMMotorController {

    /**
     * Supported motor arrangements.
     */
    public enum MotorArrangement {
        /**
         * CTR Electronics Minion motor connected over JST
         */
        Minion_JST,
        /**
         * Third-party NEO motor connected over JST
         */
        NEO_JST,
        /**
         * Third-party NEO 550 motor connected over JST
         */
        NEO550_JST,
        /**
         * Third-party NEO Vortex motor connected over JST
         */
        VORTEX_JST,
        /**
         * Brushed DC motor connected to terminals A and B
         */
        Brushed_DC
    };

    /**
     * Time tracking between calls.
     */
    private Timer _timer = new Timer();

    /**
     * list of configs pulses to send
     */
    private List<Integer> _configs = new ArrayList<>();

   /**
   * Constructor for a Talon FXS connected via PWM.
   *
   * @param channel The PWM channel that the Talon FX is attached to. 0-9 are
   *                on-board, 10-19 are on the MXP port
   */
    public PWMTalonFXS(final int channel) {
        super("PWMTalonFXS", channel);

        m_pwm.setBoundsMicroseconds(2004, 1520, 1500, 1480, 997);
        m_pwm.setPeriodMultiplier(PWM.PeriodMultiplier.k1X);
        m_pwm.setSpeed(0.0);
        m_pwm.setZeroLatch();

        HAL.report(tResourceType.kResourceType_CTRE_future12, getChannel() + 1);
    }
    /**
     * Sets the mode of operation when output is neutral or disabled.
     * 
     * This request will not block. Instead it caches the request to apply during enabled robot operation.
     * 
     * Calling frequently or every loop will prevent control PWM updates from reaching the device.
     * This should be avoided.
     * 
     * @param bIsBrake      True for brake, false for ooast.
     * 
     * @return true if request has been buffered successfully.
     */
    public boolean setNeutralMode(boolean bIsBrake) {

        /* sanity check the collection */
        if (_configs.size() > 10) {
            /* something is likely wrong in the application */
            return false;
        }

        /* push the config  */
        _configs.add(bIsBrake ? 4000 : 3500);

        /* apply update now if possible */
        if (DriverStation.isEnabled()) {
            set(0);
        }

        return true;
    }
    /**
     * Sets the motor arrangement, including which motor to drive.
     * 
     * This request will not block. Instead it caches the request to apply during enabled robot operation.
     * 
     * Calling this once on bootup is sufficient.  
     * 
     * Calling frequently or every loop will prevent control PWM updates from reaching the device.
     * This should be avoided.
     * 
     * @param motorArrangement      The desired motor arrangement
     * 
     * @return true if request has been buffered successfully.
     */
    public boolean setMotorArrangement(MotorArrangement motorArrangement) {
        /* sanity check the collection */
        if (_configs.size() > 10) {
            /* something is likely wrong in the application */
            return false;
        }

        /* push the config */
        int microseconds = 0;

        switch (motorArrangement) {
            case Minion_JST:
                microseconds = 3000;
                break;
            case NEO_JST:
                microseconds = 3100;
                break;
            case NEO550_JST:
                microseconds = 3200;
                break;
            case VORTEX_JST:
                microseconds = 3300;
                break;
            case Brushed_DC:
                microseconds = 3700;
                break;

            default:
                return false;
        }

        _configs.add(microseconds);

        /* apply update now if possible */
        if (DriverStation.isEnabled()) {
            set(0);
        }

        return true;
    }

    private boolean IsTmrExpired() {
        return _timer.get() > 0.1;
    }

    @Override
    public void set(double speed) {

        /* timer is running means we are configuring */
        if (!_timer.isRunning()) {

            /* timer not running, we are not configurating */
            if (!DriverStation.isEnabled() || _configs.isEmpty()) {

                /* turn off timer */
                _timer.stop();

                /* do what the base class normally does */
                super.set(speed);
            } else {
                /* start timer */
                _timer.restart();

                /* send PWM */
                m_pwm.setPulseTimeMicroseconds(_configs.get(0));
            }

        } else {
            /* timer running, we are applying a config */
            if (!DriverStation.isEnabled()) {

                /* turn off timer, abandon the pulse */
                _timer.start();

                /* do what the base class normally does */
                super.set(speed);

            } else if (!IsTmrExpired()) {
                /* still waiting on config pulses to finish */
            } else {
                /* tmr period expired */

                /* remove the config we've applied */
                _configs.remove(0);

                /* next steps */
                if (_configs.isEmpty()) {
                    /* turn off timer */
                    _timer.stop();

                    /* do what the base class normally does */
                    super.set(speed);

                } else {

                    /* start timer */
                    _timer.start();

                    /* send PWM */
                    m_pwm.setPulseTimeMicroseconds(_configs.get(0));
                }
            }
        }
    }
}