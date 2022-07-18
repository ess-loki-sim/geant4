#include "G4GeoLoki/bcsPack.hh"
#include "Core/Units.hh"
#include <cmath>

double bcsPack::tubeGridParallelogramBase = 27.00 *Units::mm;
double bcsPack::tubeGridParallelogramSide = 28.40 *Units::mm;
double bcsPack::tubeGridParallelogramAngle = 13.45 *Units::degree; // 90-76.55=13.45

double bcsPack::tubeRotationAngle = (8.45-30) *Units::degree; //8.45 degree should be the final roataion after the pack is rotated, pack rotation must be subtracted // -30 degree needed because of different default positioning;

double bcsPack::packBoxWidth = 265.7 *Units::mm; //excluding the handle
double bcsPack::packBoxHeight = 55.20 *Units::mm;
double bcsPack::packBoxIdleLengthOnOneEnd = 111.50 *Units::mm;
double bcsPack::tubeCentreDistanceFromPackFront = 36.50 *Units::mm;

double bcsPack::B4CLengthOverStrawOnOneEnd = 12.0 *Units::mm;
double bcsPack::B4CDistanceFromLastTubeCentre = 23.0 *Units::mm;

double bcsPack::B4CMainPartHeight = 51.20 *Units::mm;
double bcsPack::B4CPanelThickness = 5.0 *Units::mm;
double bcsPack::B4CMiddlePartHeight = 5.00 *Units::mm;
double bcsPack::B4CMiddlePartThickness = 10.80 *Units::mm;
double bcsPack::B4CBottomPartHeight = 3.20 *Units::mm;
double bcsPack::B4CBottomPartThickness = 3.00 *Units::mm;


double bcsPack::getHorizontalTubeDistanceInPack() { 
  return tubeGridParallelogramBase; 
}

double bcsPack::getVerticalTubeDistanceInPack() {  //28.40*cos(13.45 degree)=27.62
  return tubeGridParallelogramSide * std::cos(tubeGridParallelogramAngle); 
}

double bcsPack::getTubeGridParallelogramAngle() {
  return tubeGridParallelogramAngle; 
}
double bcsPack::getTubeRotation() { //compensated for the rotation of the packs 
  return tubeRotationAngle - tubeGridParallelogramAngle; 
}

double bcsPack::getB4CMainPartHeight() { 
  return B4CMainPartHeight;
}
double bcsPack::getB4CMainPartHorizontalOffset() { 
  return getHorizontalTubeCentreOffsetInPack() + 3.0* tubeGridParallelogramBase + B4CDistanceFromLastTubeCentre + 0.5* B4CPanelThickness;
}
double bcsPack::getB4CMainPartVerticalOffset() { 
  return 0.5*B4CBottomPartHeight;
}

double bcsPack::getB4CMiddlePartThickness() { 
  return B4CMiddlePartThickness;
}
double bcsPack::getB4CMiddlePartHeight() { 
  return B4CMiddlePartHeight;
}
double bcsPack::getB4CMiddlePartHorizontalOffset() { 
  return getB4CMainPartHorizontalOffset() - 0.5* B4CPanelThickness - 0.5* B4CMiddlePartThickness;
}
double bcsPack::getB4CMiddlePartVerticalOffset() { 
  return getB4CMainPartVerticalOffset() - 0.5* B4CMainPartHeight + 0.5* B4CMiddlePartHeight;
}

double bcsPack::getB4CBottomPartThickness() { 
  return B4CBottomPartThickness;
}
double bcsPack::getB4CBottomPartHeight() { 
  return B4CBottomPartHeight;
}
double bcsPack::getB4CBottomPartHorizontalOffset() { 
  return getB4CMiddlePartHorizontalOffset() - 0.5* B4CMiddlePartThickness + 0.5* B4CBottomPartThickness;
}
double bcsPack::getB4CBottomPartVerticalOffset() { 
  return getB4CMiddlePartVerticalOffset() - 0.5* B4CMiddlePartHeight - 0.5* B4CBottomPartHeight;
}

double bcsPack::getTopRowOffsetInPack() { 
  return tubeGridParallelogramSide * std::sin(tubeGridParallelogramAngle); 
}

double bcsPack::getHorizontalTubeCentreOffsetInPack() { 
  return -0.5*packBoxWidth + tubeCentreDistanceFromPackFront;
}

double bcsPack::getVerticalTubeCentreOffsetInPack() {  //TODO
  return 0.5 * getVerticalTubeDistanceInPack(); 
}

double bcsPack::getTubeGridParallelogramSide() {
  return tubeGridParallelogramSide; 
}
double bcsPack::getTubeCentreDistanceFromPackFront() {
  return tubeCentreDistanceFromPackFront; 
}



double bcsPack::getPackBoxWidth() {
  return packBoxWidth;
}
double bcsPack::getPackBoxHeight() {
  return packBoxHeight;
}
double bcsPack::getPackBoxIdleLengthOnOneEnd() {
  return packBoxIdleLengthOnOneEnd;
}

double bcsPack::getB4CPanelThickness() { 
  return B4CPanelThickness; 
}
double bcsPack::getB4CLengthOverStrawOnOneEnd() { 
  return B4CLengthOverStrawOnOneEnd; 
}