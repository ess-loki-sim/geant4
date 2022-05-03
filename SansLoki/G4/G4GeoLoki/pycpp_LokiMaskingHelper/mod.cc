#include "Core/Python.hh"
#include "G4GeoLoki/bcsBanks.hh"

PYTHON_MODULE
{
  py::class_<bcsBanks>("bcsBanks", py::init<double>())
    .def("calcPixelCentrePositionForMasking",&bcsBanks::calcPixelCentrePositionForMasking)
    .def("getPixelCentrePosition",&bcsBanks::getPixelCentrePosition)

    .def("getTotalNumberOfPixels",&bcsBanks::getTotalNumberOfPixels).staticmethod("getTotalNumberOfPixels")
    ;
}
