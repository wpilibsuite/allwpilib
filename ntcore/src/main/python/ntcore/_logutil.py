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
        min_level: _ntcore.NetworkTableInstance.LogLevel,
        max_level: _ntcore.NetworkTableInstance.LogLevel,
        log_name: str,
    ):
        handle = instance._get_handle()
        with cls._instlock:
            if handle in cls._instances:
                raise InstanceAlreadyStartedError(
                    "cannot configure logging after instance has been started"
                )

            cls._instcfg[handle] = (min_level, max_level, log_name)

    @classmethod
    def on_instance_start(cls, instance: _ntcore.NetworkTableInstance):
        handle = instance._get_handle()
        with cls._instlock:
            if handle in cls._instances:
                return

            default_cfg = (
                _ntcore.NetworkTableInstance.LogLevel.INFO,
                _ntcore.NetworkTableInstance.LogLevel.CRITICAL,
                "nt",
            )
            min_level, max_level, log_name = cls._instcfg.get(handle, default_cfg)

            cls._instances[handle] = cls(instance, log_name, min_level, max_level)

    @classmethod
    def on_instance_destroy(cls, instance: _ntcore.NetworkTableInstance):
        handle = instance._get_handle()
        with cls._instlock:
            lfwd = cls._instances.pop(handle, None)
            if lfwd:
                lfwd.destroy()

    def __init__(
        self,
        instance: _ntcore.NetworkTableInstance,
        log_name: str,
        min_level: _ntcore.NetworkTableInstance.LogLevel,
        max_level: _ntcore.NetworkTableInstance.LogLevel,
    ):
        self.lock = threading.Lock()
        self.poller = _ntcore.NetworkTableListenerPoller(instance)
        nt_logger = self.poller.add_logger(min_level, max_level)

        self.thread = threading.Thread(
            target=self._logging_thread,
            name=log_name + "-log-thread",
            daemon=True,
            args=(self.poller, log_name, nt_logger),
        )
        self.thread.start()

        atexit.register(self.destroy)

    def _logging_thread(
        self, poller: _ntcore.NetworkTableListenerPoller, log_name: str, nt_logger: int
    ):
        logger = logging.getLogger(log_name)

        _wait_for_object = wpiutil.sync.wait_for_object
        handle = poller.get_handle()

        while True:
            if not _wait_for_object(handle):
                break

            messages = poller.read_queue()
            if not messages:
                continue

            for msg in messages:
                msg = msg.data
                if logger.isEnabledFor(msg.level):
                    lr = logger.makeRecord(
                        log_name,
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
