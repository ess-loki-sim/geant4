#ifndef G4GeoLoki_bcsPack_hh
#define G4GeoLoki_bcsPack_hh

#include <string>
#include <array>

class bcsPack {
public:
  static double getHorizontalTubeDistanceInPack();
  static double getVerticalTubeDistanceInPack();

  static double getTubeGridParallelogramAngle();
  static double getTubeRotation();

  static double getTopRowOffsetInPack();
  static double getHorizontalTubeCentreOffsetInPack();
  static double getVerticalTubeCentreOffsetInPack();

  static double getTubeGridParallelogramSide();
  static double getTubeCentreDistanceFromPackFront();

  static double getPackBoxWidth();
  static double getPackBoxHeight();
  static double getPackBoxIdleLengthOnOneEnd();

  static double getB4CPanelThickness();
  static double getB4CLengthOverStrawOnOneEnd();
  static double getB4CMainPartHeight();
  static double getB4CMainPartHorizontalOffset();
  static double getB4CMainPartVerticalOffset();

  static double getB4CMiddlePartThickness();  
  static double getB4CMiddlePartHeight();  
  static double getB4CMiddlePartHorizontalOffset();
  static double getB4CMiddlePartVerticalOffset();

  static double getB4CBottomPartThickness();  
  static double getB4CBottomPartHeight();  
  static double getB4CBottomPartHorizontalOffset();
  static double getB4CBottomPartVerticalOffset();

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

  static double B4CLengthOverStrawOnOneEnd;
  static double B4CDistanceFromLastTubeCentre;

  static double B4CMainPartHeight;
  static double B4CPanelThickness;
  static double B4CMiddlePartHeight;
  static double B4CMiddlePartThickness;
  static double B4CBottomPartHeight;
  static double B4CBottomPartThickness;

};

#endif
