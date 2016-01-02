/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.simulation.ds;

import java.util.ArrayList;
import java.util.List;

import edu.wpi.first.wpilibj.simulation.ds.ISimJoystick;
import edu.wpi.first.wpilibj.simulation.ds.SimJoystick;

import net.java.games.input.Controller;
import net.java.games.input.ControllerEnvironment;

public class JoystickProvider {
	List<ISimJoystick> joysticks;
	
	public JoystickProvider() {
		scanForJoysticks();
	}
	
	public List<ISimJoystick> scanForJoysticks() {
		List<ISimJoystick> foundControllers = new ArrayList<>();
		Controller[] controllers = ControllerEnvironment.getDefaultEnvironment().getControllers();

        for(int i = 0; i < controllers.length; i++){
            Controller controller = controllers[i];
            if (controller.getType() == Controller.Type.STICK
            		|| controller.getType() == Controller.Type.GAMEPAD 
                    || controller.getType() == Controller.Type.WHEEL
                    || controller.getType() == Controller.Type.FINGERSTICK) {
                foundControllers.add(new SimJoystick(controller));
            }
        }
        
        joysticks = foundControllers;
        return foundControllers;
	}
	
	public List<ISimJoystick> getJoysticks() {
        return joysticks;
	}
	
	public void setJoysticks(List<ISimJoystick> joysticks) {
		this.joysticks = joysticks;
	}
}
