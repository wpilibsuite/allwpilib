import dataclasses
import logging
import multiprocessing
import multiprocessing.connection
import os
import pathlib
import signal
import sys
import time
import typing as T

import pytest

import robotpy.main
import wpilib


from .pytest_plugin import RobotTestingPlugin


def _enable_faulthandler():
    #
    # In the event of a segfault, faulthandler will dump the currently
    # active stack so you can figure out what went wrong.
    #
    # Additionally, on non-Windows platforms we register a SIGUSR2
    # handler -- if you send the robot process a SIGUSR2, then
    # faulthandler will dump all of your current stacks. This can
    # be really useful for figuring out things like deadlocks.
    #

    import logging

    logger = logging.getLogger("faulthandler")

    try:
        # These should work on all platforms
        import faulthandler

        faulthandler.enable()
    except Exception as e:
        logger.warning("Could not enable faulthandler: %s", e)
        return

    try:
        faulthandler.register(signal.SIGUSR2)
        logger.info("registered SIGUSR2 for PID %s", os.getpid())
    except Exception:
        return


class WorkerPlugin:
    """
    This pytest plugin runs in the isolated process that runs a test that uses the
    robot fixture.

    Heavily borrowed from pytest-xdist WorkerInteractor
    """

    def __init__(self, channel: multiprocessing.connection.Connection):
        self.channel = channel

    def sendevent(self, name: str, **kwargs: object):
        self.channel.send((name, kwargs))

    @pytest.hookimpl(wrapper=True)
    def pytest_sessionstart(self, session: pytest.Session):
        self.config = session.config
        return (yield)

    @pytest.hookimpl
    def pytest_internalerror(self, excrepr: object):
        formatted_error = str(excrepr)
        for line in formatted_error.split("\n"):
            print("IERROR>", line, file=sys.stderr)
        self.sendevent("internal_error", formatted_error=formatted_error)

    @pytest.hookimpl
    def pytest_runtest_logstart(
        self,
        nodeid: str,
        location: tuple[str, int | None, str],
    ):
        self.sendevent("logstart", nodeid=nodeid, location=location)

    @pytest.hookimpl
    def pytest_runtest_logfinish(
        self,
        nodeid: str,
        location: tuple[str, int | None, str],
    ):
        self.sendevent("logfinish", nodeid=nodeid, location=location)

    @pytest.hookimpl
    def pytest_runtest_logreport(self, report: pytest.TestReport):
        data = self.config.hook.pytest_report_to_serializable(
            config=self.config, report=report
        )
        self.sendevent("testreport", data=data)


def _run_test(
    item_nodeid, config_args, robot_class, robot_file, verbose, pipe, root_path
):
    """This function runs in a subprocess"""
    logging.root.addHandler(logging.NullHandler())
    logging.root.setLevel(logging.DEBUG if verbose else logging.INFO)

    _enable_faulthandler()

    # This is used by getDeployDirectory, so make sure it gets fixed
    robotpy.main.robot_py_path = robot_file

    os.chdir(root_path)

    # keep the plugins around because it has a reference to the robot
    # and we don't want it to die and deadlock
    plugin = RobotTestingPlugin(robot_class, robot_file, True)
    worker_plugin = WorkerPlugin(pipe)

    ec = pytest.main(
        [item_nodeid, "--no-header", "-p", "no:terminalreporter", *config_args],
        plugins=[plugin, worker_plugin],
    )

    # ensure output is printed out
    sys.stdout.flush()

    # Don't let the process die, let the parent kill us to avoid
    # python interpreter badness
    worker_plugin.sendevent("finished", exit_code=ec)
    pipe.close()

    # ensure that the gc doesn't collect the plugin..
    while plugin:
        time.sleep(100)


@dataclasses.dataclass
class IsolatedTestJob:
    item: pytest.Function
    conn: multiprocessing.connection.Connection
    process: multiprocessing.Process
    start_time: float
    exit_code: int | None = None

    finished: bool = False

    # set when the worker indicates it has finished
    worker_completed: bool = False

    def set_exit_code(self, ec: int):
        if self.exit_code is None:
            self.exit_code = ec


class IsolatedTestsPlugin:
    """
    This pytest plugin runs any test that uses the 'robot' fixture in an
    isolated subprocess
    """

    def __init__(
        self,
        robot_class: T.Type[wpilib.RobotBase],
        robot_file: pathlib.Path,
        builtin_tests: bool,
        verbose: bool,
        parallelism: int,
    ):
        self._robot_class = robot_class
        self._robot_file = robot_file
        self._builtin_tests = builtin_tests
        self._verbose = verbose

        if parallelism < 1:
            try:
                parallelism = multiprocessing.cpu_count() - 1
            except NotImplementedError:
                parallelism = 1

        self._parallelism = max(1, parallelism)
        self._shouldstop = False

    @pytest.hookimpl(wrapper=True)
    def pytest_sessionstart(self, session: pytest.Session):
        self._config = session.config
        self._maxfail: int = self._config.getvalue("maxfail")
        self._countfailures = 0
        self._shouldstop = False

        multiprocessing.set_start_method("spawn")

        return (yield)

    @pytest.hookimpl
    def pytest_runtestloop(self, session: pytest.Session) -> bool:
        if (
            session.testsfailed
            and not session.config.option.continue_on_collection_errors
        ):
            raise session.Interrupted(
                f"{session.testsfailed} error{'s' if session.testsfailed != 1 else ''} during collection"
            )

        if session.config.option.collectonly:
            return True

        running: list[IsolatedTestJob] = []
        deferred: list[pytest.Function] = []
        try:
            # Start any tests that use the robot fixture first. Tests that don't
            # use the robot fixture will be ran later
            for item in session.items:
                assert isinstance(item, pytest.Function)
                if "robot" not in item.fixturenames:
                    deferred.append(item)
                    continue

                while len(running) >= self._parallelism:
                    self._wait_for_jobs(running, session)

                running.append(self._start_isolated_test(item))
                self._maybe_raise(session)

            # Run the in-process tests now while the robot tests are finishing
            for idx, item in enumerate(deferred):
                nextitem = deferred[idx + 1] if idx + 1 < len(deferred) else None
                session.config.hook.pytest_runtest_protocol(
                    item=item, nextitem=nextitem
                )
                self._maybe_raise(session)

            while running:
                self._wait_for_jobs(running, session)
        finally:
            for job in running:
                self._cleanup_job(job)

        return True

    def _start_isolated_test(self, item: pytest.Function) -> IsolatedTestJob:

        config_args = self._config.invocation_params.args
        if self._builtin_tests:
            nodeid = f"{config_args[0]}::{item.name}"
            config_args = config_args[1:]
        else:
            nodeid = item.nodeid

        pconn, cconn = multiprocessing.Pipe()
        process = multiprocessing.Process(
            target=_run_test,
            args=(
                nodeid,
                config_args,
                self._robot_class,
                self._robot_file,
                self._verbose,
                cconn,
                self._config.rootpath,
            ),
        )
        process.start()
        cconn.close()

        return IsolatedTestJob(
            item=item,
            conn=pconn,
            process=process,
            start_time=time.time(),
        )

    def _wait_for_jobs(self, running: list[IsolatedTestJob], session: pytest.Session):
        if not running:
            return

        ready = multiprocessing.connection.wait([job.conn for job in running])

        for conn in ready:
            job = next(job for job in running if job.conn == conn)
            self._process_job_messages(job, session)
            if job.finished:
                running.remove(job)
                self._finalize_job(job, session)

    def _process_job_messages(self, job: IsolatedTestJob, session: pytest.Session):
        while not job.finished:
            try:
                if not job.conn.poll():
                    break
                callname, kwargs = job.conn.recv()
            except (IOError, EOFError) as e:
                job.finished = True
                break

            method = "worker_" + callname
            call = getattr(self, method)
            call(job, **kwargs)
            self._maybe_raise(session)

        if not job.process.is_alive():
            job.finished = True

    def _finalize_job(self, job: IsolatedTestJob, session: pytest.Session):
        self._cleanup_job(job)

        if job.worker_completed:
            return

        stop = time.time()
        duration = stop - job.start_time

        ec = job.exit_code
        longrepr = None
        if ec is None:
            longrepr = "subprocess failed for unknown reason"
        else:
            if ec < 0:
                try:
                    signal_name = signal.strsignal(-ec)
                    longrepr = f"subprocess exited due to signal {-ec}: {signal_name}"
                except ValueError:
                    pass

            if longrepr is None:
                longrepr = f"subprocess exited with exit code {ec}"

        report = pytest.TestReport(
            nodeid=job.item.nodeid,
            location=job.item.location,
            keywords=job.item.keywords,
            outcome="failed",
            longrepr=longrepr,
            when="call",
            duration=duration,
            start=job.start_time,
            stop=stop,
        )

        self._config.hook.pytest_runtest_logstart(
            nodeid=job.item.nodeid, location=job.item.location
        )
        self._config.hook.pytest_runtest_logreport(report=report)
        self._config.hook.pytest_runtest_logfinish(
            nodeid=job.item.nodeid, location=job.item.location
        )

        self._maybe_raise(session)

    def _cleanup_job(self, job: IsolatedTestJob):
        try:
            job.conn.close()
        except Exception:
            pass

        if job.process.is_alive():
            job.process.kill()

        try:
            job.process.join(timeout=1)
        except TimeoutError:
            pass

        ec = job.process.exitcode
        if ec is not None:
            job.set_exit_code(ec)

        job.process.close()

    def _maybe_raise(self, session: pytest.Session):
        if self._shouldstop:
            raise session.Interrupted(self._shouldstop)
        if session.shouldfail:
            raise session.Failed(session.shouldfail)
        if session.shouldstop:
            raise session.Interrupted(session.shouldstop)

    #
    # Worker dispatch functions (copied from pytest-xdist)
    #

    def worker_logstart(
        self,
        job: IsolatedTestJob,
        nodeid: str,
        location: tuple[str, int | None, str],
    ):
        """Emitted when a node calls the pytest_runtest_logstart hook."""
        if self._config.option.verbose > 0:
            return
        self._config.hook.pytest_runtest_logstart(nodeid=nodeid, location=location)

    def worker_logfinish(
        self,
        job: IsolatedTestJob,
        nodeid: str,
        location: tuple[str, int | None, str],
    ):
        """Emitted when a node calls the pytest_runtest_logfinish hook."""
        if self._config.option.verbose > 0:
            return
        self._config.hook.pytest_runtest_logfinish(nodeid=nodeid, location=location)

    def worker_testreport(self, job: IsolatedTestJob, data: object):
        """Emitted when a node calls the pytest_runtest_logreport hook."""

        report = self._config.hook.pytest_report_from_serializable(
            config=self._config, data=data
        )
        self._config.hook.pytest_runtest_logreport(report=report)
        self._handlefailures(report)

    def worker_internal_error(self, job: IsolatedTestJob, formatted_error: str):
        """Emitted when a node calls the pytest_internalerror hook."""
        for line in formatted_error.split("\n"):
            print("IERROR>", line, file=sys.stderr)

        job.finished = True
        if not self._shouldstop:
            self._shouldstop = "internal error in worker"

    def worker_finished(self, job: IsolatedTestJob, exit_code: object | None = None):
        """Emitted when a node finishes running."""
        if exit_code is not None:
            job.exit_code = int(exit_code)

        job.worker_completed = True
        job.finished = True

    def _handlefailures(self, rep: pytest.TestReport):
        if rep.failed:
            self._countfailures += 1
            if (
                self._maxfail
                and self._countfailures >= self._maxfail
                and not self._shouldstop
            ):
                self._shouldstop = f"stopping after {self._countfailures} failures"

    #
    # These fixtures match the ones in RobotTestingPlugin but these have no effect
    #

    @pytest.fixture(scope="function")
    def robot(self):
        pass

    @pytest.fixture(scope="function")
    def control(self, reraise, robot):
        pass

    @pytest.fixture()
    def robot_file(self) -> pathlib.Path:
        """The absolute filename your robot code is started from"""
        return self._robot_file

    @pytest.fixture()
    def robot_path(self) -> pathlib.Path:
        """The absolute directory that your robot code is located at"""
        return self._robot_file.parent
