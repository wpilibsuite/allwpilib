# Linux Bluetooth socket regressions

These standalone programs exercise the built Linux Bluetooth client and real
libuv without a Bluetooth adapter. Only Bluetooth socket operations are
redirected to local TCP sockets; selected connection errors are injected.

From the repository root, after configuring a shared-library CMake build with
`-DBUILD_SHARED_LIBS=ON -DCMAKE_EXPORT_COMPILE_COMMANDS=ON`:

```sh
cmake --build build-ninja --target wpinet
python3 wpinet/src/test/native/linux/run_bluetooth_regressions.py build-ninja
```

The runner uses the build's compiler/header paths and shared libraries. Set
`CXX` to override the compiler. It builds temporary executables and runs all
scenarios with diagnostics both disabled and enabled. No Bluetooth hardware,
BlueZ daemon, or elevated privileges are needed.

Coverage:

- POLLERR handling for L2CAP fallback, direct GATT, and established reset;
- failure reporting when SO_ERROR has already been cleared;
- ignoring callbacks from the replaced L2CAP poll handle;
- one delayed GATT retry after an immediate ENOMEM, including persistent error;
- no retries for a fatal permission error;
- cancellation and replacement while a retry timer is pending;
- silent default logging and unchanged outcomes with debug output enabled.

They run in separate processes because their socket symbol interposition must
not affect the other wpinet networking tests. The final TCP connection is
intentionally refused: these tests check error and retry lifecycle behavior,
not ATT discovery or Bluetooth radio reliability.
