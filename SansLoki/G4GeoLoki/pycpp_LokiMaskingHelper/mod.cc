#include "Core/Python.hh"
#include "G4GeoLoki/MaskingHelper.hh"

PYTHON_MODULE
{
  py::class_<MaskingHelper>("MaskingHelper", py::init<double>())
    .def(py::init<double, int>())
    .def("getPixelCentrePositionsForMasking",&MaskingHelper::getPixelCentrePositionsForMasking)
    .def("getTotalNumberOfPixels",&MaskingHelper::getTotalNumberOfPixels).staticmethod("getTotalNumberOfPixels")
    ;
}
