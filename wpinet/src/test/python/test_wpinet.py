import wpinet
import pytest


def test_existence():
    assert wpinet.UsbDevice is not None
    assert wpinet.UsbDeviceDetector is not None


def test_usb_device_detector_validates_port():
    def callback(_device):
        pass

    with pytest.raises(IndexError):
        wpinet.UsbDeviceDetector(-1, callback, callback)
    with pytest.raises(IndexError):
        wpinet.UsbDeviceDetector(4, callback, callback)
