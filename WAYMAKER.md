# Summary

WayMaker is a tool to allow FRC teams to create, visualize, and edit paths on a field for their robots to follow during a match. WayMaker works in conjunction with the WPILib Trajectory features to separate creation of a path from generation of its corresponding trajectory on the robot. It is intended to replace the PathWeaver tool.

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
## Frames of Reference
 * Chassis Relative  - 
    * `x` is positive in the robot's forward direction and negative in the backward direction.
    * `y` is positive pointing left from the robot's perspective and negative pointing right.
    * `omega` is angular velocity with positive turning counterclockwise, as viewed from above.
    > Chassis Relative does not have a heading because the robot always faces forward in the +x chassis-relative direction.
 * Field Relative -
    * `x` is positive toward the opposing alliance wall, and negative towards the robot's own alliance wall. 
    * `y` is positive toward the right (long side) field boundary as viewed from the robot's alliance station.
    * `omega` is angular velocity with positive turning counterclockwise, as viewed from above. It is the same in chassis-relative and field-relative frames.
    * `heading` is the angle of the chassis-relative +x in the field frame. It is measured counterclockwise from the field-relative +x direction.
## Glossary
 * Path - a curve representing the course of the robot and defined by an ordered list of waypoints, through which the path must pass.
 * Trajectory - a list of states that the robot should target. The trajectory is generated from a path and is best described as a velocity profile along the path.
 * Waypoint - a point through which the path must pass. Waypoints have a heading, which may also constrain the path.
 * Heading - 
    * (of a point along the path) The desired direction of travel through the point, synonymous with the direction of the path tangent vector at the point.
    * (of a robot) The angle of the chassis-relative +x in the field frame, measured counterclockwise from the field-relative +x direction.
    > If a robot is driving a path with negative chassis x velocity, the robot heading may be opposite the path heading.
 * Forward - 
    * (of robot motion) When the direction of motion is in the direction of the chassis-relative +x axis.
    * (of a path) The direction that the robot should traverse the path.
 * Reverse - 
    * (of robot motion) When the direction of motion is in the direction of the chassis-relative -x axis.
    * (of a path) The direction opposite the robot's motion along the path.

 * Segment - A portion of a path between two consecutive waypoints.
 * Reversal - a maneuver in which the robot approaches a point with a chassis +x velocity, stops, and reverses away from the point with a chassis -x velocity, or vice versa.
 * Reversal Point - A waypoint where the robot should perform a reversal instead of passing through the waypoint.
 
# Diagrams




# Design

Waymaker has two components: a desktop application for editing the paths, and a generator added to WPILib to turn the path information into trajectories.

## Pipeline

Waymaker Desktop:
* Place waypoints on a to-scale top-down field image. 

* See a preview of the path defined by those points. 

* Edit options for the path, points, and segments between points to produce the desired path.

* Export the path information to a file that is deployed to the robot with the robot code.

Waymaker Generator (robot-side):

* The generator reads the deployed file and generates a Trajectory object for the robot to follow.

## Path

WayMaker represents paths as ordered lists of waypoints, accompanied by ordered lists of segments that represent the portions of the path between the waypoints. 

Each waypoint has the following properties. Depending on the path type of the segments adjacent to the point, some properties may not affect the shape of the curve. This will be further explained when the path types are detailed below. 

|**Name**|**Description**|**Type**|**Default**|
|---|---|---|---|
Index |Index 0 is the start point, and the robot follows the points in increasing order| Integer|
|X coordinate|In field-relative coordinates|Length|
|Y coordinate|In field-relative coordinates|Length|
|Heading|Path tangent at this point, in field-relative coordinates|Angle|towards positive field X.
|Tangent|Length of control vector|Length|Optimized for smooth curvature
|Holonomic Angle|Field-relative conversion of chassis-relative +x vector angle|Angle|Matches Heading

> Holonomic Angle is one of a few features that will likely be restricted behind a "Holonomic Mode" toggle. This toggle allows access to features only usable with holonomic drives. WayMaker's generator will need a sensible way to handle these features when generating for a differential drive. For example, Holonomic Angle can be simply ignored, since it will likely output as a separate motion profile anyway. 

Each segment has the following properties.

|**Name**|**Description**|**Type** | **Default**
|---|---|---|---|
|Index|The index of the segment’s start point|Integer
|Interpolation Type|A string representing the algorithm for interpolating the segment.|CubicHermite\|QuinticHermite\|Clothoid\|etc...|Same as both neighbor segments if matching, otherwise same as previous segment in the path.
|Velocity Sign| See below. If the robot should have negative (true) instead of positive (false) chassis x-velocity when traversing the segment.|Boolean|Same as both neighbor segments if matching, otherwise same as previous segment in the path.

### Segment Velocity Sign



![Reversal Points Figure](/waymakerDocs/media/reversal/reversal-point.svg)

*Figure 1: Reversal Point*

In Figure 1, the green color of the segments between 1 and 3 means that the robot moves along the path facing and driving forward. The large hollow triangle points along the chassis-relative +x axis. Similarly, the red color means the robot drives in reverse, with the "forward" direction pointing opposite the direction of travel. 

In a scenario such as point 3, which is where the path switches from positive to negative x-velocity, the robot will approach the point front-first, slow down to a stop, and reverse direction, driving away from the point back-first. 

If a waypoint is between two segments of the same drive direction, like points 2 and 4, the robot will pass through the point traveling in the direction of the heading, even though its front may be facing the opposite way. This maintains the meaning of heading: Heading always points along the path in the direction of travel.

>**NOTE**: This is different from the PathWeaver and current TrajectoryGenerator operation of reversal, in which the heading always refers to the chassis-relative +x direction. WayMaker generator will have to convert to that convention before generating trajectories.



Reversal’s representation in the UI is currently TBD. From the user's perspective, it might be setting a flag on the waypoint where drive direction should switch. 


## Framework

WayMaker is written in C++, using the ImGUI framework. This framework and language is chosen to match the ecosystem of current WPILib tooling, to minimize additional burden of maintenance. 

## JSON Path File Schema

"version" is a property determined by the layout of the schema, and not by any property of the path.

```json
{
  "name": string,
  "version": string,
  "units": string,
  "points": [
      {
          "x": double,
          "y": double,
          "heading": double,
          "tangent": double,
          "holonomicAngle": double
      },
      //... n points.
  ],
  "segments": [
      {
          "type": "CubicHermite" | "QuinticHermite" | "Clothoid",
          "velocitySign": bool
      },
      //... n-1 segments.
  ]
  
}
```
## PathWeaver Compatibility
REQUIREMENT: The WayMaker JSON schema needs to be able to represent paths created in PathWeaver.

>**TBD**: Should an exporter be added to PathWeaver, or should WayMaker import PathWeaver's existing format?

For a PathWeaver import from a .csv, the following transformations occur:

* `name` is created from the PathWeaver file name.
* `units` are assumed to be meters.
    >**TBD**: Whose responsibility is it to set PathWeaver to export paths in meters? 
* `x` remains the same value as in PathWeaver.
* `y` is adjusted by the width of the field to fit the WPILib coordinate system.
* `velocitySign` is true (reverse velocity) for all segments if it is true for all PathWeaver points, otherwise it is false for all segments. 
    >**TBD**: What to do with malformed data? (e.g., reverse not the same on all points in the csv?)
* `heading` and `tangent` are a coordinate conversion from PathWeaver's Tangent X and Tangent Y. If `velocitySign` is true, `heading` is then offset by 180 degrees to account for the difference in reverse behavior between PathWeaver and WayMaker. 
* `holonomicAngle` is set to the same as the adjusted `heading`. 
* `type` is "QuinticHermite" for all segments.

## Generator JSON Parsing



