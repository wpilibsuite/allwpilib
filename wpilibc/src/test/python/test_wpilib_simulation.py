import wpilib.simulation


def test_wpilib_simulation():
    pass


def test_addressableled_sim():
    led = wpilib.AddressableLED(0)
    sim = wpilib.simulation.AddressableLEDSim(led)
    assert not sim.get_start()
    led.set_start(1)
    assert sim.get_start()

    data = [
        wpilib.AddressableLED.LEDData(1, 2, 3),
        wpilib.AddressableLED.LEDData(4, 5, 6),
    ]

    led.set_length(len(data))
    led.set_data(data)
    assert sim.get_data() == data

    led.set_start(0)
    assert not sim.get_start()
