import wpilib


def test_encoder_set_rate_window(wpilib_state):
    encoder = wpilib.Encoder(0, 1)

    encoder.set_rate_window(50)


def test_tachometer_set_rate_window(wpilib_state):
    tachometer = wpilib.Tachometer(0, wpilib.EdgeConfiguration.RISING_EDGE)

    tachometer.set_rate_window(50)


def test_tachometer_stopped_state_matches_rate(wpilib_state):
    tachometer = wpilib.Tachometer(0, wpilib.EdgeConfiguration.RISING_EDGE)

    assert tachometer.get_frequency() == 0.0
    assert tachometer.get_stopped()
