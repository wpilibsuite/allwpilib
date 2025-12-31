import wpilib.simulation


def test_wpilib_simulation():
    pass


def test_addressableled_sim():
    led = wpilib.AddressableLED(0)
    sim = wpilib.simulation.AddressableLEDSim(led)
    assert not sim.getStart()
    led.setStart(1)
    assert sim.getStart()

    data = [
        wpilib.AddressableLED.LEDData(1, 2, 3),
        wpilib.AddressableLED.LEDData(4, 5, 6),
    ]

    led.setLength(len(data))
    led.setData(data)
    assert sim.getData() == data

    led.setStart(0)
    assert not sim.getStart()
