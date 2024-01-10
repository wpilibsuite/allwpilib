# Simulation Extensions
This is where WPILib's simulation extensions are housed. Simulation extensions are referred to by various names: simulation plugins, simulation modules, sim extensions, HALSIM extensions, but they all refer to the same thing, which are dynamically loaded libraries that can use HALSIM functions to register callbacks that update HAL data. The robot program loads simulation extensions by looking for the `HALSIM_EXTENSIONS` environment variable, which contains the paths to the libraries separated by colons on Linux/Mac, and semicolons on Windows.

# Writing a custom simulation extension
All simulation extensions contain a `int HALSIM_InitExtension(void)` function, which is the entry point. The function declaration should look like this:

```c++
extern "C" {
#if defined(WIN32) || defined(_WIN32)
__declspec(dllexport)
#endif
int HALSIM_InitExtension(void) {
}
}
```

The function is contained with an `extern "C"` block so it can be called by the robot program and has `__declspec(dllexport)` for Windows. From here, you can interface with the HAL to provide data.

## Extension registration
Extensions can register themselves by calling `HAL_RegisterExtension`. This will register the extension's name, and a pointer to some sort of data. A separate extension can listen for extension registration by calling `HAL_RegisterExtensionListener`. It takes a callback accepting the data initially passed into `HAL_RegisterExtension`. This can be used to detect if a specific extension was loaded, and take action if it was. Note that extensions must opt-in to registration; extensions that do not register will not trigger the registration listener.

## Using HALSIM functions
Several devices in the HAL have functions that allow you to feed data from an external source into the HAL. The full list can be found in the HAL subdirectory in `include/hal/simulation`. For example, the AccelerometerData header declares functions that update the X, Y, and Z axes with data. Some functions accept callbacks; the I2CData header declares functions which accept other functions with parameters that allow it to accept data. This allows the implementation of a simulation extension that interfaces with an I2C bus and connects it to the HAL, allowing the use of real I2C hardware in simulation. Note that these callbacks are called synchronously and in the same thread as the robot program; long delays in callbacks will block the main thread and can cause loop overruns.

## Building the extension
To build an extension for use in a robot project, you'll need to build with Gradle. The easiest way to get a working build.gradle file is to copy the build.gradle file from halsim_xrp. It doesn't have any tests, and it only depends on halsim_ws_core. The important line is `lib project: ':wpinet', library: 'wpinet', linkage: 'shared'`. This tells Gradle to link the extension with wpinet using shared libraries. Other libraries can be included in a similar way by referencing the Gradle subproject name. Note that you do not need to include the HAL since it is automatically included.

# Using a custom extension
After setting up a build.gradle file for a custom extension, follow the guides to build and publish your own local build of allwpilib. Once you've published a local build, follow the instructions in [DevelopmentBuilds.md](/DevelopmentBuilds.md) to use the locally published build in a robot project. Then, place this line your robot project's build.gradle file:
```groovy
wpi.sim.addDep("Custom Sim Extension", "edu.wpi.first.halsim", "pluginName")
```
where `Custom Sim Extension` is the name of the extension shown by VS Code and `pluginName` is the same as `pluginName` declared in the build.gradle file for the simulation extension.

# Built-in extensions
halsim_ds_socket: Allows the real Driver Station to control the robot program.

halsim_gui: Provides the simulation GUI.

halsim_ws_client: A websockets client that allows robot hardware interface state to be transmitted over websockets.

halsim_ws_core: A websockets library for use by other extensions. Not directly usable.

halsim_ws_server: A websockets server that allows robot hardware interface state to be transmitted over websockets.

halsim_xrp: A client that supports the XRP protocol, allowing the robot program to control and receive data from the XRP.
