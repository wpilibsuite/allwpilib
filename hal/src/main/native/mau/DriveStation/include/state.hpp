#pragma once

#include "MauDriveData.h"
#include "DriverStationInternal.h"
#include "library.hpp"
#include "shared.fwi.hpp"
#include <string>
#include <vector>

#define STATE_DISABLED 0
#define STATE_AUTO 1
#define STATE_TELEOP 2
#define STATE_TEST 3

namespace Toast {
	typedef Memory::RobotState RobotState;
    class State { 
        public:
            State(RobotState rs) {
                _rs = rs;
            }
            
            // Is this state controlled by an operator? This is true for TELEOP and TEST
            // States.
            API inline bool is_operator() {
				return _rs == RobotState::TELEOP || _rs == RobotState::TEST;
            }
            
            // Is the robot disabled? This is true for the DISABLED State.
            API inline  bool disabled() {
                return _rs == RobotState::DISABLED;
            }
            
            // Is the robot autonomous? This is true for the AUTONOMOUS State.
            API inline bool autonomous() {
                return _rs == RobotState::AUTO;
            }
            
            // Is the robot teleoperated? This is true for the TELEOP State
            API inline bool teleop() {
                return _rs == RobotState::TELEOP;
            }
            
            // Is the robot in test mode? This is true for the TEST State.
            API inline bool test() {
                return _rs == RobotState::TEST;
            }

			API inline RobotState robot_state() {
				return _rs;
            }
            
            // Convert the state to a string. This is one of either "Disabled",
            // "Autonomous", "Teleop" or "Test"
            API std::string to_string() {
                if (disabled()) return "Disabled";
                if (autonomous()) return "Autonomous";
                if (teleop()) return "Teleop";
                if (test()) return "Test";
		return 0;
            }
        private:
			RobotState _rs;
    };
    
    class IterativeBase {
        public:
            IterativeBase() { }
        
            API virtual void disabledInit() { };
            API virtual void disabledPeriodic() { };
            
            API virtual void autonomousInit() { };
            API virtual void autonomousPeriodic() { };
            
            API virtual void teleopInit() { };
            API virtual void teleopPeriodic() { };
            
            API virtual void testInit() { };
            API virtual void testPeriodic() { };
    };
    
    namespace States {
        namespace Internal {
            API void set_tick_timing(int ms);
            API int get_tick_timing();
            
            API void set_state(RobotState rs);
        }
        
        API State DISABLED();
        API State AUTO();
        API State TELEOP();
        API State TEST();
        
        API State from_robotstate(RobotState rs);
        
		API RobotState current_robotstate();
		API RobotState last_robotstate();

        API State current_state();
        API State last_state();
        
        API void start_tracker();
		API void _manual_trigger();
        
        // Register a tracker. Trackers are called when the robot transitions from 
        // one state to another. This accepts a function pointer of return type
        // 'void' and accepting two arguments of Toast::States::State, with the
        // first argument being the old state, and the second argument being the
        // new (current) state.
        API void register_tracker(void (*func)(State, State));
        
        // Register a ticker. Tickers are called periodically at the same frequency
        // as the IterativeRobot periodic functions. This accepts a function pointer
        // of return type 'void' and a single argument of type Toast::States::State
        // denoting the current state the robot is in.
        API void register_ticker(void (*func)(State));
        
        // Register an IterativeBase. IterativeBase is designed to mimic the layout
        // of IterativeRobot, and as such provides both Periodic and Init functions.
        API void register_iterative(IterativeBase *iterative);
    }
}