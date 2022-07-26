#include "Core/Python.hh"
#include "G4GeoLoki/MaskingHelper.hh"

PYTHON_MODULE
{
  py::class_<MaskingHelper>("MaskingHelper", py::init<double>())
    .def(py::init<double, int>())
    .def("calcPixelCentrePositionForMasking",&MaskingHelper::calcPixelCentrePositionForMasking)
    .def("getPixelCentrePosition",&MaskingHelper::getPixelCentrePosition)

    .def("getTotalNumberOfPixels",&MaskingHelper::getTotalNumberOfPixels).staticmethod("getTotalNumberOfPixels")
    ;
}
