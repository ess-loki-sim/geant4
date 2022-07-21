#include "Core/Python.hh"
#include "G4GeoLoki/maskingHelper.hh"

PYTHON_MODULE
{
  py::class_<maskingHelper>("maskingHelper", py::init<double>())
    .def(py::init<double, int>())
    .def("calcPixelCentrePositionForMasking",&maskingHelper::calcPixelCentrePositionForMasking)
    .def("getPixelCentrePosition",&maskingHelper::getPixelCentrePosition)

    .def("getTotalNumberOfPixels",&maskingHelper::getTotalNumberOfPixels).staticmethod("getTotalNumberOfPixels")
    ;
}
