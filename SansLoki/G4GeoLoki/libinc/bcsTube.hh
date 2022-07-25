#ifndef G4GeoLoki_bcsTube_hh
#define G4GeoLoki_bcsTube_hh

#include "G4Materials/NamedMaterialProvider.hh"

class bcsTube {
public:
    /// materials ///  
  static G4Material* tubeWallMaterial;
  static G4Material* tubeInnerGas;
  static G4Material* strawWallMaterial;
  static G4Material* converterMaterial;
  static G4Material* countingGas;
  
    /// straw ///
  static double getStrawOuterRadius();
  static double getStrawInnerRadius();
  static double getStrawWallThickness();

  static double getFrontTubeConverterThickness();
  static double getBackTubeConverterThickness();

    /// tube ///
  static double getTubeOuterRadius();
  static double getTubeInnerRadius();

  static double getStrawPositionX(const int strawId);
  static double getStrawPositionY(const int strawId);

private:
  static double strawOuterRadius;
  static double strawWallThickness;

  static double frontTubeConverterThickness;
  static double backTubeConverterThickness;

  static double tubeOuterRadius;
  static double tubeWallThickness;

  static double strawPositionsInTube[7][2];
  static double strawStrawDistance;
};

#endif
