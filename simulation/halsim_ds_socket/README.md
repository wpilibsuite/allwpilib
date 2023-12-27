# HAL DS Socket
This is an extension that allows the Driver Station to communicate with the robot program. Note that not everything has been reimplemented, since lots of DS data like battery voltage doesn't apply in simulation.

# Configuration

The only environment variable the extension supports is `DS_TIMEOUT_MS`, which is the amount of milliseconds it takes for a UDP packet to arrive from the DS before the robot program automatically disables. Default value is `100`, representing 100 milliseconds.
