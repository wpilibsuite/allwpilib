# Authors

Jeremiah Shue

6995 NOMAD, Autonomous Lead

GH: shueja-personal

CD: @Amicus1

Discord: @J S frc6995#8149


# Discussion

Chief Delphi: https://www.chiefdelphi.com/t/waymaker-a-new-waypoint-editor-for-path-planning/396363/39

GitHub: https://github.com/wpilibsuite/allwpilib/discussions/3611

FRC Discord: #programming-discussion

# Summary

WayMaker is a tool to allow FRC teams to create, visualize, and edit paths on a field for their robots to follow during a match. WayMaker works in conjunction with the WPILib Trajectory features to separate creation of a path from generation of its corresponding trajectory on the robot. It is intended to replace the PathWeaver tool.

# Motivation
## Why Replace PathWeaver?

The current path planning software, PathWeaver, was developed prior to trajectory generation robot-side being a part of WPILib. Therefore, PathWeaver includes tooling for generating the trajectory itself, and requires robot-specific information when starting a new path. 

PathWeaver is also written in JavaFX, while most WPILib tools developed since then have been in the C++ ImGUI framework. This means that PathWeaver is harder to maintain now that most developers are working in the ImGUI framework. 

With the advent of robot-side trajectory generation, many teams are using PathWeaver to draw the path, then copying or importing the pre-generation path data into their robot code. PathWeaver’s path data format uses a different coordinate convention than WPILib, which makes transferring the data somewhat error-prone.
## A Paradigm Shift

As mentioned, the way many teams are using PathWeaver exposes a shift in how paths are created for the robot. WPILib’s Trajectory API can generate trajectories robot-side, keeping the robot-specific information like trackwidth and velocity constraints with the robot code.

PathWeaver’s main drawback in this department is that it requires this information before it allows creation of paths, even just for visualization. The shape of the path does not depend on the robot following it, so a path planner that does not generate trajectories does not need this robot-specific information. 

With a path visualizer desktop application, there is an opportunity to break the path design away from the trajectory generation. Therefore it is most accurate to say that WayMaker completes the replacement of PathWeaver, a shift started with the Trajectory API.
## New Features

PathWeaver had one path type: Quintic Hermite. WPILib also supports Cubic Hermite, with the possibility to add more algorithms. WayMaker intends to support all curve interpolation algorithms that WPILib can natively generate. 

Furthermore, WayMaker adds a new feature called per-segment interpolation. In other words, different parts of the same path can use different interpolation algorithms. As a usage example, with the current options of Cubic/Quintic Hermite, this could allow a user to specify only some points on the path that affect the heading of the curve. 

WayMaker also allows the user to mark reversal points -  a maneuver in which the robot approaches a point, stops, and backs away from the point, continuing its drive facing the opposite direction. In PathWeaver, drive direction could only be set for the entire path as a whole, which made paths like the 2021 Bounce Path awkward to code.

# Definitions
 * Path - a curve representing the course of the robot and defined by an ordered list of waypoints, through which the path must pass.
 * Trajectory - a list of states that the robot should target. The trajectory is generated from a path and is best described as a velocity profile along the path.
 * Waypoint - a point through which the path must pass. Waypoints have a heading, which may also constrain the path.
 * Reversal - a maneuver in which the robot approaches a point, stops, and backs away from the point, continuing its drive facing the opposite direction. This is similar to one point of a three-point turn operation. WayMaker supports marking a waypoint as a reversal point.

# Design

Waymaker has two components: a desktop application for editing the paths, and a generator added to WPILib to turn the path information into trajectories.

## Pipeline

Waymaker Desktop:
Place waypoints on a to-scale top-down field image. 

See a preview of the path defined by those points. 

Edit options for the path, points, and segments between points to produce the desired path.

Export the path information to a file that is deployable to the robot.

Waymaker Generator (robot-side):

The generator reads the deployed file and generates a Trajectory object for the robot to follow.

## Path

WayMaker represents paths as ordered lists of waypoints, accompanied by ordered lists of segments that represent the portions of the path between the waypoints. 

Each waypoint has the following properties. Depending on the path type of the segments adjacent to the point, some properties may not affect the shape of the curve. This will be further explained when the path types are detailed below. 

|**Name**|**Description**|**Type**
|---|---|---|
Index |Index 0 is the start point, and the robot follows the points in increasing order | Integer
|X coordinate|In existing WPILib Pose2D coordinate system|Length
|Y coordinate|In existing WPILib Pose2D coordinate system|Length
|Heading|Direction of robot travel|Angle
|Holonomic Angle|Direction robot faces|Angle

(Note on the holonomic angle. This is one exception to the barrier between robot-specific information and WayMaker. Its implementation is TBD, but a robot pointing a different direction than it travels is only possible on holonomic drives. Therefore, WayMaker will have to somehow tell when the option should be available.)

Each segment has the following properties.

|**Name**|**Description**|**Type**
|---|---|---|
|Index|The index of the segment’s start point|Integer
|Interpolation Type|A string representing the algorithm for interpolating the segment.|CubicHermite\|QuinticHermite\|Clothoid\|etc...|
|Reversal|Whether the robot should have forward velocity (false) or reverse velocity (true) as defined by the encoders.|Boolean

(Note on the reversal flag. Reversal’s representation in the UI is currently TBD. One option is that reversal is done by setting a flag on the point at which the robot should reverse direction. Outside the UI, when the generator breaks up the path into strings of segments with the same direction and interpolation type, it makes sense to have a flag on the segment for direction. For storage and export of the path file, location of the reversal flag is on the segment. Therefore, the UI reversal will internally be flipping the reversal flag on segments, no matter how reversal is actually displayed to the user)
## Framework

WayMaker is written in C++, using the ImGUI framework. This framework and language is chosen to match the ecosystem of current WPILib tooling, to minimize additional burden of maintenance. 

## JSON Path File Schema

## Generator JSON Parsing



