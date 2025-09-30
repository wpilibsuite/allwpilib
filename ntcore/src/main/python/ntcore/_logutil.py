import atexit
import logging
import threading

from . import _ntcore

import wpiutil.sync


class InstanceAlreadyStartedError(Exception):
    pass


class NtLogForwarder:
    """
    Forwards ntcore's logger to python's logging system
    """

    _instlock = threading.Lock()
    _instances = {}
    _instcfg = {}

    @classmethod
    def config_logging(
        cls,
        instance: _ntcore.NetworkTableInstance,
        minLevel: _ntcore.NetworkTableInstance.LogLevel,
        maxLevel: _ntcore.NetworkTableInstance.LogLevel,
        logName: str,
    ):
        handle = instance._getHandle()
        with cls._instlock:
            if handle in cls._instances:
                raise InstanceAlreadyStartedError(
                    "cannot configure logging after instance has been started"
                )

            cls._instcfg[handle] = (minLevel, maxLevel, logName)

    @classmethod
    def onInstanceStart(cls, instance: _ntcore.NetworkTableInstance):
        handle = instance._getHandle()
        with cls._instlock:
            if handle in cls._instances:
                return

            default_cfg = (
                _ntcore.NetworkTableInstance.LogLevel.kLogInfo,
                _ntcore.NetworkTableInstance.LogLevel.kLogCritical,
                "nt",
            )
            minLevel, maxLevel, logName = cls._instcfg.get(handle, default_cfg)

            cls._instances[handle] = cls(instance, logName, minLevel, maxLevel)

    @classmethod
    def onInstanceDestroy(cls, instance: _ntcore.NetworkTableInstance):
        handle = instance._getHandle()
        with cls._instlock:
            lfwd = cls._instances.pop(handle, None)
            if lfwd:
                lfwd.destroy()

    def __init__(
        self,
        instance: _ntcore.NetworkTableInstance,
        logName: str,
        minLevel: _ntcore.NetworkTableInstance.LogLevel,
        maxLevel: _ntcore.NetworkTableInstance.LogLevel,
    ):
        self.lock = threading.Lock()
        self.poller = _ntcore.NetworkTableListenerPoller(instance)
        ntLogger = self.poller.addLogger(minLevel, maxLevel)

        self.thread = threading.Thread(
            target=self._logging_thread,
            name=logName + "-log-thread",
            daemon=True,
            args=(self.poller, logName, ntLogger),
        )
        self.thread.start()

        atexit.register(self.destroy)

    def _logging_thread(
        self, poller: _ntcore.NetworkTableListenerPoller, logName: str, ntLogger: int
    ):
        logger = logging.getLogger(logName)

        _waitForObject = wpiutil.sync.waitForObject
        handle = poller.getHandle()

        while True:
            if not _waitForObject(handle):
                break

            messages = poller.readQueue()
            if not messages:
                continue

            for msg in messages:
                msg = msg.data
                if logger.isEnabledFor(msg.level):
                    lr = logger.makeRecord(
                        logName,
                        msg.level,
                        msg.filename,
                        msg.line,
                        "%s",
                        (msg.message,),
                        None,
                    )
                    logger.handle(lr)

    def destroy(self):
        with self.lock:
            if self.poller:
                self.poller.close()
                self.thread.join(timeout=1)
            self.poller = None


_config_logging = NtLogForwarder.config_logging
