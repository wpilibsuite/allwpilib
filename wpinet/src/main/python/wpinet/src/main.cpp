
#include <memory>
#include <utility>

#include <pybind11/functional.h>
#include <pybind11/pybind11.h>

#include "semiwrap_init.wpinet._wpinet.hpp"
#include "wpi/net/UsbDeviceDetector.hpp"

namespace py = pybind11;

namespace {

class PyUsbDeviceDetector {
 public:
  PyUsbDeviceDetector(int port, wpi::net::UsbDeviceDetector::Callback connected,
                      wpi::net::UsbDeviceDetector::Callback disconnected)
      : m_detector{std::make_unique<wpi::net::UsbDeviceDetector>(
            port, std::move(connected), std::move(disconnected))} {}

  ~PyUsbDeviceDetector() {
    {
      py::gil_scoped_release release;
      m_detector->Stop();
    }
    m_detector.reset();
  }

  bool Start() {
    py::gil_scoped_release release;
    return m_detector->Start();
  }

  void Stop() {
    py::gil_scoped_release release;
    m_detector->Stop();
  }

  bool IsRunning() const { return m_detector->IsRunning(); }

  bool HasImplementation() const { return m_detector->HasImplementation(); }

 private:
  std::unique_ptr<wpi::net::UsbDeviceDetector> m_detector;
};

}  // namespace

SEMIWRAP_PYBIND11_MODULE(m) {
  initWrapper(m);

  py::class_<wpi::net::UsbDevice>(m, "UsbDevice")
      .def_readonly("port", &wpi::net::UsbDevice::port)
      .def_readonly("syspath", &wpi::net::UsbDevice::syspath)
      .def_readonly("usb_syspath", &wpi::net::UsbDevice::usbSyspath)
      .def_readonly("subsystem", &wpi::net::UsbDevice::subsystem)
      .def_readonly("device_type", &wpi::net::UsbDevice::deviceType)
      .def_readonly("device_node", &wpi::net::UsbDevice::deviceNode)
      .def_readonly("vendor_id", &wpi::net::UsbDevice::vendorId)
      .def_readonly("product_id", &wpi::net::UsbDevice::productId)
      .def_readonly("manufacturer", &wpi::net::UsbDevice::manufacturer)
      .def_readonly("product", &wpi::net::UsbDevice::product)
      .def_readonly("serial_number", &wpi::net::UsbDevice::serialNumber);

  py::class_<PyUsbDeviceDetector>(m, "UsbDeviceDetector")
      .def(py::init<int, wpi::net::UsbDeviceDetector::Callback,
                    wpi::net::UsbDeviceDetector::Callback>(),
           py::arg("port"), py::arg("connected"), py::arg("disconnected"))
      .def("start", &PyUsbDeviceDetector::Start)
      .def("stop", &PyUsbDeviceDetector::Stop)
      .def("is_running", &PyUsbDeviceDetector::IsRunning)
      .def("has_implementation", &PyUsbDeviceDetector::HasImplementation)
      .def(
          "__enter__",
          [](PyUsbDeviceDetector& self) -> PyUsbDeviceDetector& {
            return self;
          },
          py::return_value_policy::reference_internal)
      .def("__exit__",
           [](PyUsbDeviceDetector& self, const py::object&, const py::object&,
              const py::object&) { self.Stop(); });
}
