# HAL WebSockets Server

This is an extension that provides a server version of a WebSockets API for transmitting robot hardware interface state over a network.  See the [Robot Hardware Interface WebSockets API specification](../halsim_ws_core/doc/hardware_ws_api.md) for more details on the protocol.

## Configuration

The WebSockets server has a number of configuration options available through environment variables.

``HALSIMWS_SYSROOT``: The local directory to serve non-websocket HTTP content from (e.g. HTML files) starting from `/`.  Defaults to the `sim` subdirectory of the current working directory.

``HALSIMWS_USERROOT``: The local directory to serve non-websocket HTTP content from (e.g. HTML files) starting from `/user/`.  Defaults to the `sim/user` subdirectory of the current working directory.

``HALSIMWS_PORT``: The port number to listen at.  Defaults to 3300.

``HALSIMWS_URI``: The URI path to use for WebSockets connections.  Defaults to ``"/wpilibws"``.
