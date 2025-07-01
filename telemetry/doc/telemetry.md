# Overview

A unified, extensible, data-oriented, output-only telemetry API for WPILib with heirarchical structure and flexible backend implementation.

# Motivation

Telemetry, logging, and visualization of data values are critical to debugging of real-time systems, in particular complex software-electromechanical systems such as robots, as it provides the ability to visualize the internals of system operation that are not externally observable.  To this end, WPILib provides a number of core libraries for both sending real-time data values to interactive GUI (dashboard) applications (NetworkTables) and logging data values to files for offline analysis (DataLog).

However, these core libraries are feature-rich, and to maximize user flexibility, have fairly complex APIs with large API surfaces.  Most users, particularly beginners, instead use simplified APIs that were originally designed for use with specific dashboards (SmartDashboard and Shuffleboard).  These existing classes have misleading names, as because all of them use NetworkTables under the hood, they are not limited to use for that specific dashboard--indeed it is quite common for users to use the SmartDashboard API with other dashboards.  These APIs are also dashboard-oriented, limited (don't support modern serialization methods such as struct and protobuf), and generally have a flat structure (don't support a heirarchy of values, at least not at the API level).

As users have increasingly used the NetworkTables and DataLog APIs, demand is also rising for an API that unifies these two APIs while supporting directing the telemetry values to different destinations based on need.  For example, sending some values to NetworkTables and others to DataLog.  While some of this demand is being driven by performance limitations of the RoboRIO, it's also useful to have other backend options such as a mock implementation for unit testing.

# References

- (NetworkTables)[../../ntcore/doc/networktables4.adoc]
- (DataLog)[../../datalog/doc/datalog.adoc]

# Design

## Frontend API

## Backend API

# Drawbacks

# Alternatives

# Trades

# Unresolved Questions
