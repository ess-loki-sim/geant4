#ifndef G4GeoLoki_calibMasks_hh
#define G4GeoLoki_calibMasks_hh

#include <iostream>
#include <vector>
#include <map>
#include <numeric>
#include "G4Materials/NamedMaterialProvider.hh"

class calibMasks {
public:
  class calibMasksBase {
    public:
      const std::string name;
      const double thickness;
      const double height;
      const double leftTubeEndDistance;
      const double elevationFromMaskFront = 0.0; 
      const std::vector<double> pattern;

      calibMasksBase(std::string name, double thickness, double height, double leftTubeEndDistance, std::vector<double> pattern): name(name), thickness(thickness), height(height), leftTubeEndDistance(leftTubeEndDistance), pattern(pattern){
      }
      calibMasksBase(std::string name, double thickness, double height, double leftTubeEndDistance, double elevationFromMaskFront, std::vector<double> pattern): name(name), thickness(thickness), height(height), leftTubeEndDistance(leftTubeEndDistance), elevationFromMaskFront(elevationFromMaskFront), pattern(pattern){}

      double getWidth() const;
      double getNumberOfMaskParts() const;
  };
  static calibMasksBase getCalibMask(std::string name);
  static G4Material* maskMaterial;
  static G4Material* maskBoxMaterial;

private:
  const static std::map<std::string, calibMasks::calibMasksBase> masks;
};

#endif
