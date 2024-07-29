# HAL WebSockets Client

This is an extension that provides a client version of a WebSockets API for transmitting robot hardware interface state over a network.  See the [Robot Hardware Interface WebSockets API specification](../halsim_ws_core/doc/hardware_ws_api.md) for more details on the protocol.

## Configuration

The WebSockets client has a number of configuration options available through environment variables.

``HALSIMWS_HOST``: The host to connect to.  Defaults to localhost.

``HALSIMWS_PORT``: The port number to connect to.  Defaults to 3300.

``HALSIMWS_URI``: The URI path to connect to.  Defaults to ``"/wpilibws"``.
