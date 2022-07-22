#ifndef G4GeoLoki_bcsPack_hh
#define G4GeoLoki_bcsPack_hh

#include <string>
#include <array>

class bcsPack {
public:
  static double getHorizontalTubeDistanceInPack();
  static double getVerticalTubeDistanceInPack();
  static double getTubeGridParallelogramAngle();

  static double getTopRowOffsetInPack();
  static double getHorizontalTubeCentreOffsetInPack();

  static double getTubeGridParallelogramSide();
  static double getTubeCentreDistanceFromPackFront();

  static double getPackBoxWidth();
  static double getPackBoxHeight();
  static double getPackBoxIdleLengthOnOneEnd();

  static double getTubeRotation();
  static double getHorizontalTubeOffset(const int inPackTubeId);
  static double getVerticalTubeOffset(const int inPackTubeId);

  /// B4C panel parts ///
  static double getB4CLengthOverStrawOnOneEnd();

  static double getB4CPartThickness(const int partId);
  static double getB4CPartHeight(const int partId);
  static double getB4CPartHorizontalOffset(const int partId);
  static double getB4CPartVerticalOffset(const int partId);

  /// Al panel parts ///
  static double getAlPartThickness(const int partId);
  static double getAlPartHeight(const int partId);
  static double getAlPartHorizontalOffset(const int partId);
  static double getAlPartVerticalOffset(const int partId);

private:
  static double tubeGridParallelogramBase;
  static double tubeGridParallelogramSide;
  static double tubeGridParallelogramAngle;

  static double tubeRotationAngle;

  static double packBoxWidth;
  static double packBoxHeight;
  static double packBoxIdleLengthOnOneEnd;
  static double tubeCentreDistanceFromPackFront; //1st lower (closest) tube 

  static double packHolderDistanceFromPackTop;
  static double packHolderDistanceFromPackFront;

  /// B4C panel parts ///
  static double B4CLengthOverStrawOnOneEnd;
  static double B4CDistanceFromLastTubeCentre;
  static double B4CPanelPartThickness[3];
  static double B4CPanelPartHeight[3];
};

#endif
