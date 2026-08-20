import commands2
import tunables
import wpilib


def test_networktables_lifecycle_matches_glass_run_cancel(
    scheduler: commands2.CommandScheduler, nt_instance
):
    tunables.TunableRegistry.reset()
    try:
        tunables.TunableRegistry.register_backend(
            "",
            wpilib.NetworkTablesTunableBackend(nt_instance, "/Tunables"),
        )

        counts = {
            "initialize": 0,
            "execute": 0,
            "interrupted_end": 0,
            "finished_end": 0,
        }

        def increment(name: str):
            counts[name] += 1

        def on_end(interrupted: bool):
            increment("interrupted_end" if interrupted else "finished_end")

        command = commands2.FunctionalCommand(
            lambda: increment("initialize"),
            lambda: increment("execute"),
            on_end,
            lambda: False,
        )
        command.set_name("Glass Label")
        tunables.publish("command", command)

        name = nt_instance.get_string_topic("/Tunables/command/name").subscribe("")
        assert (
            nt_instance.get_topic("/Tunables/command/name").get_property("mutable")
            is False
        )
        assert name.get() == "Glass Label"

        running = nt_instance.get_boolean_topic("/Tunables/command/running").get_entry(
            False
        )
        assert (
            nt_instance.get_topic("/Tunables/command/running").get_property("mutable")
            is True
        )
        assert running.get() is False
        assert not scheduler.is_scheduled(command)

        running.set(True)
        nt_instance.flush()
        tunables.TunableRegistry.update()

        assert scheduler.is_scheduled(command)
        assert running.get() is True
        assert counts == {
            "initialize": 1,
            "execute": 0,
            "interrupted_end": 0,
            "finished_end": 0,
        }

        scheduler.run()
        assert counts["execute"] == 1
        tunables.TunableRegistry.update()
        assert running.get() is True

        command.set_name("Updated Label")
        tunables.TunableRegistry.update()
        assert name.get() == "Updated Label"

        running.set(False)
        nt_instance.flush()
        tunables.TunableRegistry.update()

        assert not scheduler.is_scheduled(command)
        assert running.get() is False
        assert counts == {
            "initialize": 1,
            "execute": 1,
            "interrupted_end": 1,
            "finished_end": 0,
        }

        scheduler.run()
        assert counts["execute"] == 1
        tunables.TunableRegistry.update()
        assert running.get() is False
    finally:
        tunables.TunableRegistry.reset()
