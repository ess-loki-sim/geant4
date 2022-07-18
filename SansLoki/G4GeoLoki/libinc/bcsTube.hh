#ifndef G4GeoLoki_bcsTube_hh
#define G4GeoLoki_bcsTube_hh

#include <string>
#include <array>

class bcsTube {
public:
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
