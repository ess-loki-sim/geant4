#include "G4GeoLoki/bcsBanks.hh"
#include "Core/Units.hh"
#include <cmath>
#include <iostream>
#include <array>

/// straw ///
double bcsBanks::strawOuterRadius = 3.75 *Units::mm;
double bcsBanks::strawWallThickness = 0.0254 *Units::mm; //0.001 inch

double bcsBanks::frontTubeConverterThickness = 0.65 *Units::um;
double bcsBanks::backTubeConverterThickness = 1.00 *Units::um;

/// tube ///
double bcsBanks::tubeOuterRadius = 12.7 *Units::mm; // 1 inch diameter
double bcsBanks::tubeWallThickness = 0.889 *Units::mm; //0.035 inch  //0.94 
double bcsBanks::strawStrawDistance = 7.75 *Units::mm; // 

//std::string bcsBanks::tubeWallMaterial = "NCrystal:cfg=Al_sg225.ncmat";

double bcsBanks::strawPositionsInTube[7][2] = { //TODO reorder to change volume numbering
    { -0.5*strawStrawDistance * tan(M_PI/3.), -0.5*strawStrawDistance }, //20
    { 0, -strawStrawDistance }, //0
    { 0.5*strawStrawDistance * tan(M_PI/3.), -0.5*strawStrawDistance }, //10
    { 0, 0},  //30
    { -0.5*strawStrawDistance * tan(M_PI/3.), 0.5*strawStrawDistance }, //50
    { 0, strawStrawDistance}, //60
    { 0.5*strawStrawDistance * tan(M_PI/3.), 0.5*strawStrawDistance }, //40
  };

/// tube grid  ///
double bcsBanks::tubeGridParallelogramBase = 27.00 *Units::mm;
double bcsBanks::tubeGridParallelogramSide = 28.40 *Units::mm;
double bcsBanks::tubeGridParallelogramAngle = 13.45 *Units::degree; // 90-76.55=13.45

double bcsBanks::tubeRotationAngle = (8.45-30) *Units::degree; //8.45 degree should be the final roataion after the pack is rotated, pack rotation must be subtracted // -30 degree needed because of different default positioning;


double bcsBanks::packBoxWidth = 265.7 *Units::mm; //excluding the handle
double bcsBanks::packBoxHeight = 55.20 *Units::mm;
double bcsBanks::packBoxIdleLengthOnOneEnd = 111.50 *Units::mm;
double bcsBanks::tubeCentreDistanceFromPackFront = 36.50 *Units::mm;

double bcsBanks::packHolderDistanceFromPackTop = 7.6 *Units::mm;
double bcsBanks::packHolderDistanceFromPackFront = 7.5 *Units::mm;

double bcsBanks::B4CLengthOverStrawOnOneEnd = 12.0 *Units::mm;
double bcsBanks::B4CDistanceFromLastTubeCentre = 23.0 *Units::mm;

double bcsBanks::B4CMainPartHeight = 51.20 *Units::mm;
double bcsBanks::B4CPanelThickness = 5.0 *Units::mm;
double bcsBanks::B4CMiddlePartHeight = 5.00 *Units::mm;
double bcsBanks::B4CMiddlePartThickness = 10.80 *Units::mm;
double bcsBanks::B4CBottomPartHeight = 3.20 *Units::mm;
double bcsBanks::B4CBottomPartThickness = 3.00 *Units::mm;


/// bank ///
double bcsBanks::strawLengthInBank[9] = { // all in mm
    1000.0, // 0 - rear
    1000.0,  // 1 - mid top
    500.0, // 2 - mid left
    1000.0,  // 3 - mid bottom
    500.0, // 4 - mid right
    1200.0, // 5 - front top
    1200.0, // 6 - front left
    1200.0, // 7 - front bottom
    1200.0, // 8 - front right
};

int bcsBanks::numberOfPixels[9] = { // number of pixels along the straws
    //16384, // 0 - rear (used for CalibSlitSourceGen) 
    256, // 0 - rear
    256,  // 1 - mid top
    256, // 2 - mid left
    256,  // 3 - mid bottom
    256, // 4 - mid right
    256, // 5 - front top
    256, // 6 - front left
    256, // 7 - front bottom
    256, // 8 - front right
};

int bcsBanks::numberOfPacksInBank[9] = { // all in mm
    28, // 0 - rear
    8,  // 1 - mid top
    6, // 2 - mid left
    8,  // 3 - mid bottom
    6, // 4 - mid right
    14, // 5 - front top
    16, // 6 - front left
    10, // 7 - front bottom
    16, // 8 - front right
};

double bcsBanks::bankPositionAngle[9] = {
    0, // 0 - rear
    9.7,  // 1 - mid top
    6.5, // 2 - mid left
    9.7,  // 3 - mid bottom
    6.5, // 4 - mid right
    32.20, // 5 - front top 
    25.6, // 6 - front left
    27.5, // 7 - front bottom
    25.6, // 8 - front right
};

double bcsBanks::bankTiltAngle[9] = {
    90.0, // 0 - rear
    94.0,  // 1 - mid top
    92.0, // 2 - mid left
    94.0,  // 3 - mid bottom
    92.0, // 4 - mid right
    104.7, // 5 - front top 
    105.0, // 6 - front left
    100.0, // 7 - front bottom //180-80=100
    105.0, // 8 - front right
};

double bcsBanks::calcBankRotation(const int bankId){ //27.5+ (80-90)
  return (90 - (bankTiltAngle[bankId] - bankPositionAngle[bankId])) *Units::degree;
}

double bcsBanks::bankRotation[9][3] = { // all in mm
    {0.0,      0.5*M_PI, calcBankRotation(0)}, // 0 - rear
    {M_PI,     0.5*M_PI, calcBankRotation(1)},  // 1 - mid top
    {1.5*M_PI, 0.5*M_PI, calcBankRotation(2)}, // 2 - mid left
    {0.0,      0.5*M_PI, calcBankRotation(3)},  // 3 - mid bottom
    {0.5*M_PI, 0.5*M_PI, calcBankRotation(4)}, // 4 - mid right
    {M_PI,     0.5*M_PI, calcBankRotation(5)}, // 5 - front top
    {1.5*M_PI, 0.5*M_PI, calcBankRotation(6)}, // 6 - front left
    {0.0,      0.5*M_PI, calcBankRotation(7)}, // 7 - front bottom
    {0.5*M_PI, 0.5*M_PI, calcBankRotation(8)}, // 8 - front right
};

double bcsBanks::bankDistance[9] = {
    0.0, // 0 - rear //TODO ssd
    2950.0,  // 1 - mid top
    3350.0, // 2 - mid left
    2950.0,  // 3 - mid bottom
    3350.0, // 4 - mid right
    1364.68, // 5 - front top
    1750.0, // 6 - front left
    1340.0, // 7 - front bottom
    1750.0, // 8 - front right
};

double bcsBanks::calcBankPositionZ(const int bankId) {
  double intendedPosition = bankDistance[bankId] * std::cos(bankPositionAngle[bankId]*Units::degree);
  double bankCentreOffsetZ = detectorSystemCentreOffsetInBank(bankId, 2) * std::cos(calcBankRotation(bankId)) - detectorSystemCentreOffsetInBank(bankId, 1) * std::sin(calcBankRotation(bankId));
  return intendedPosition + bankCentreOffsetZ;
}
double bcsBanks::calcBankPositionXY(const int bankId) {
  double intendedPosition = bankDistance[bankId] * std::sin(bankPositionAngle[bankId]*Units::degree);
  double bankCentreOffsetXY = detectorSystemCentreOffsetInBank(bankId, 2) * std::sin(calcBankRotation(bankId)) + detectorSystemCentreOffsetInBank(bankId, 1) * std::cos(calcBankRotation(bankId));

  return intendedPosition + bankCentreOffsetXY;
}

double bcsBanks::bankPosition[9][3] = {
    {0, 0, 0}, // 0 - rear !calculated in getBankPosition function!
    {0, calcBankPositionXY(1), calcBankPositionZ(1)},  // 1 - mid top
    {calcBankPositionXY(2), 0, calcBankPositionZ(2)}, // 2 - mid left
    {0, -calcBankPositionXY(3), calcBankPositionZ(3)},  // 3 - mid bottom
    {-calcBankPositionXY(4), 0, calcBankPositionZ(4)}, // 4 - mid right
    {0, calcBankPositionXY(5), calcBankPositionZ(5)}, // 5 - front top
    {calcBankPositionXY(6), 0, calcBankPositionZ(6)}, // 6 - front left
    {0, -calcBankPositionXY(7), calcBankPositionZ(7)}, // 7 - front bottom
    {-calcBankPositionXY(8), 0, calcBankPositionZ(8)}, // 8 - front right
};
double bcsBanks::bankPositionOffset[9][3] = {
    {0.0, 0.0, 0.0}, // 0 - rear 
    {0.0, 0.0, 0.0},  // 1 - mid top
    {0.0, 0.0, 0.0}, // 2 - mid left
    {0.0, 0.0, 0.0},  // 3 - mid bottom
    {0.0, 0.0, 0.0}, // 4 - mid right
    {100.50, 0.0, 0.0}, // 5 - front top
    {0.0, -14.57, 0.0}, // 6 - front left
    {-100.50, 0.0, 0.0}, // 7 - front bottom
    {0.0, 51.43, 0.0}, // 8 - front right
};
double bcsBanks::bankSize[9][3] = { // x (width), y (height), z (depth)  //NOTE: x-y swap for vertical banks
    {1265.0, 1806.0, 285.00+15}, //0 - rear  {1265, 870+870+66, 285} //+20mm to avoid volume overlap of packs and the bank
    {1265.0, 590.18, 297.82},  // 1 - mid top {175+915+175, }
    {765.0, 447.45, 298.92}, // 2 - mid left
    {1265.0, 590.18, 297.82},  // 3 - mid bottom
    {765.0, 447.45, 298.92}, // 4 - mid right
    {1465.0, 945.00, 303.09}, // 5 - front top
    {1465.0, 1016.11, 302.99}, // 6 - front left
    {1465.0, 681.43, 302.99}, // 7 - front bottom
    {1465.0, 1016.11, 302.99}, // 8 - front right // changed from 1016.08
};

double bcsBanks::topmostPackHolderPositionInBankFromTopFront[9][2] = { // y (height from bank top(*)), z (depth from bank front) (*)rotation?
    {117.26, 35.65}, //0 - rear  
    {77.12, 31.33},  // 1 - mid top (upside down)
    {77.38, 31.33}, // 2 - mid left (upside down)
    {77.12, 31.33},  // 3 - mid bottom 
    {77.38, 31.33}, // 4 - mid right
    {77.94, 31.33}, // 5 - front top (upside down)
    {77.92, 31.33}, // 6 - front left (upside down)
    {77.92, 31.33}, // 7 - front bottom
    {77.92, 31.33}, // 8 - front right
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/// straw ///
double bcsBanks::getStrawOuterRadius() const{
  return strawOuterRadius;
}
double bcsBanks::getStrawInnerRadius() const{
  return strawOuterRadius - strawWallThickness;
}
double bcsBanks::getStrawWallThickness() const{
  return strawWallThickness;
}
double bcsBanks::getFrontTubeConverterThickness() const{
  return frontTubeConverterThickness;
}
double bcsBanks::getBackTubeConverterThickness() const{
  return backTubeConverterThickness;
}

  

/// tube ///
double bcsBanks::getTubeOuterRadius() const { 
  return tubeOuterRadius; 
  }
double bcsBanks::getTubeInnerRadius() const { 
  return tubeOuterRadius - tubeWallThickness; 
  }
//std::string bcsBanks::getTubeWallMaterial() const { 
//  return tubeWallMaterial; 
//  }

  

double bcsBanks::getStrawPositionX(const int strawId) const { 
  return strawPositionsInTube[strawId][0]; 
}
double bcsBanks::getStrawPositionY(const int strawId) const { 
  return strawPositionsInTube[strawId][1]; 
}

/// tube grid  ///
double bcsBanks::getHorizontalTubeDistanceInPack() const { 
  return tubeGridParallelogramBase; 
}
double bcsBanks::getVerticalTubeDistanceInPack() {  //28.40*cos(13.45 degree)=27.62
  return tubeGridParallelogramSide * std::cos(tubeGridParallelogramAngle); 
}
double bcsBanks::getPackRotation() const {
  return tubeGridParallelogramAngle; 
}
double bcsBanks::getTubeRotation() const { //compensated for the rotation of the packs 
  return tubeRotationAngle - tubeGridParallelogramAngle; 
}

double bcsBanks::getB4CMainPartHeight() { 
  return B4CMainPartHeight;
}
double bcsBanks::getB4CMainPartHorizontalOffset() { 
  return getHorizontalTubeCentreOffsetInPack() + 3.0* tubeGridParallelogramBase + B4CDistanceFromLastTubeCentre + 0.5* B4CPanelThickness;
}
double bcsBanks::getB4CMainPartVerticalOffset() { 
  return 0.5*B4CBottomPartHeight;
}

double bcsBanks::getB4CMiddlePartThickness() { 
  return B4CMiddlePartThickness;
}
double bcsBanks::getB4CMiddlePartHeight() { 
  return B4CMiddlePartHeight;
}
double bcsBanks::getB4CMiddlePartHorizontalOffset() { 
  return getB4CMainPartHorizontalOffset() - 0.5* B4CPanelThickness - 0.5* B4CMiddlePartThickness;
}
double bcsBanks::getB4CMiddlePartVerticalOffset() { 
  return getB4CMainPartVerticalOffset() - 0.5* B4CMainPartHeight + 0.5* B4CMiddlePartHeight;
}

double bcsBanks::getB4CBottomPartThickness() { 
  return B4CBottomPartThickness;
}
double bcsBanks::getB4CBottomPartHeight() { 
  return B4CBottomPartHeight;
}
double bcsBanks::getB4CBottomPartHorizontalOffset() { 
  return getB4CMiddlePartHorizontalOffset() - 0.5* B4CMiddlePartThickness + 0.5* B4CBottomPartThickness;
}
double bcsBanks::getB4CBottomPartVerticalOffset() { 
  return getB4CMiddlePartVerticalOffset() - 0.5* B4CMiddlePartHeight - 0.5* B4CBottomPartHeight;
}

double bcsBanks::getTopRowOffsetInPack() const { 
  return tubeGridParallelogramSide * std::sin(tubeGridParallelogramAngle); 
}

double bcsBanks::getHorizontalTubeCentreOffsetInPack() { 
  return -0.5*packBoxWidth + tubeCentreDistanceFromPackFront;
}

double bcsBanks::getVerticalTubeCentreOffsetInPack() {  //TODO
  return 0.5 * getVerticalTubeDistanceInPack(); 
}

double bcsBanks::getPackPackDistance() const { 
  return tubeGridParallelogramSide * 2; 
}

double bcsBanks::getPackBoxWidth() const {
  return packBoxWidth;
}
double bcsBanks::getPackBoxHeight() const {
  return packBoxHeight;
}
double bcsBanks::getPackBoxIdleLengthOnOneEnd() const {
  return packBoxIdleLengthOnOneEnd;
}

double bcsBanks::getB4CPanelThickness() const { 
  return B4CPanelThickness; 
}
double bcsBanks::getB4CLengthOverStrawOnOneEnd() const { 
  return B4CLengthOverStrawOnOneEnd; 
}


/// bank ///
double bcsBanks::getStrawLengthByBankId(const int bankId) const{
  return strawLengthInBank[bankId] * Units::mm;
}
int bcsBanks::getNumberOfPacksByBankId(const int bankId) const{
  return numberOfPacksInBank[bankId];
}
int bcsBanks::getNumberOfTubes(const int bankId){
  return numberOfPacksInBank[bankId] * 8;
}

double bcsBanks::getBankRotation(const int bankId, const int axisIndex) const { 
  return bankRotation[bankId][axisIndex]; 
}

double bcsBanks::getBankPosition(const int bankId, const int axisIndex) const { 
  if(bankId == 0){
    double rearBankPosition[] = {0, -detectorSystemCentreOffsetInBank(bankId, 1), rearBankDistance + detectorSystemCentreOffsetInBank(bankId, 2)};
    return rearBankPosition[axisIndex];
  }
  else{
    return bankPosition[bankId][axisIndex] + bankPositionOffset[bankId][axisIndex];
  }  
}
double bcsBanks::getBankSize(const int bankId, const int axisIndex) { 
  return bankSize[bankId][axisIndex]+0.05 *Units::mm;
}

double bcsBanks::packHolderToPackCentreCoordsInPack(const int axisIndex) {
  if(axisIndex == 1) { //y
    return 0.5*packBoxHeight - packHolderDistanceFromPackTop;
  }
  else {//z
    return 0.5*packBoxWidth - packHolderDistanceFromPackFront;
  }
}

double bcsBanks::getTopmostPackPositionInBank(const int bankId, const int axisIndex) { 
  if(axisIndex == 0){ //x direction
    return 0;
  }
  else if(axisIndex == 1) { //y direction
  const double packHolderPositionYFromBankTop = topmostPackHolderPositionInBankFromTopFront[bankId][0];
  const double packHolderPositionY = 0.5* getBankSize(bankId, 1)  - packHolderPositionYFromBankTop;
  
  const double positionOfPackCentreFromPackHolderY = packHolderToPackCentreCoordsInPack(2) *std::sin(tubeGridParallelogramAngle) - packHolderToPackCentreCoordsInPack(1) *std::cos(tubeGridParallelogramAngle);

  return packHolderPositionY + positionOfPackCentreFromPackHolderY;
  }
  else{ //z direction
  const double packHolderPositionZFromBankFront = topmostPackHolderPositionInBankFromTopFront[bankId][1];
  const double packHolderPositionZ = - (0.5*getBankSize(bankId, 2) - packHolderPositionZFromBankFront);

  const double positionOfPackCentreFromPackHolderZ = packHolderToPackCentreCoordsInPack(2) *std::cos(tubeGridParallelogramAngle) + packHolderToPackCentreCoordsInPack(1) *std::sin(tubeGridParallelogramAngle);
  return packHolderPositionZ + positionOfPackCentreFromPackHolderZ;
  }
}

double bcsBanks::packHolderToFirstTubeCentreCoordsInPack(const int axisIndex) {
  if(axisIndex == 1) { //y
    return 0.5*packBoxHeight + getVerticalTubeCentreOffsetInPack() - packHolderDistanceFromPackTop;
  }
  else {//z
    return tubeCentreDistanceFromPackFront - packHolderDistanceFromPackFront;
  }
}

double bcsBanks::detectorSystemFrontDistanceFromBankFront(const int bankId) {  
  const double packHolderPositionZFromBankFront = topmostPackHolderPositionInBankFromTopFront[bankId][1];
  const double positionOfFirstTubeCentreFromPackHolderZ = packHolderToFirstTubeCentreCoordsInPack(2) *std::cos(tubeGridParallelogramAngle) + packHolderToFirstTubeCentreCoordsInPack(1) *std::sin(tubeGridParallelogramAngle);

  return packHolderPositionZFromBankFront + positionOfFirstTubeCentreFromPackHolderZ - tubeOuterRadius;
}

double bcsBanks::detectorSystemCentreDistanceFromBankTop(const int bankId) {
  const double packHolderDistanceYFromBankTop = topmostPackHolderPositionInBankFromTopFront[bankId][0];

  const double distanceOfSecondRowTubeCentreFromPackHolderY = std::abs(packHolderToFirstTubeCentreCoordsInPack(2) * std::sin(tubeGridParallelogramAngle) - packHolderToFirstTubeCentreCoordsInPack(1) * std::cos(tubeGridParallelogramAngle));
  const double distanceOfFirstRowTubeCentreFromPackHolderY = distanceOfSecondRowTubeCentreFromPackHolderY - tubeGridParallelogramSide;
  const double detectorSystemSizeY = (numberOfPacksInBank[bankId] * 2 - 1) * tubeGridParallelogramSide; // from top row centre to lowest row centre
  
  const double distanceOfDetectorSystemCentreFromPackHolderY = distanceOfFirstRowTubeCentreFromPackHolderY + 0.5 * detectorSystemSizeY;

  return packHolderDistanceYFromBankTop + distanceOfDetectorSystemCentreFromPackHolderY;
}

double bcsBanks::detectorSystemCentreOffsetInBank(const int bankId, const int axisIndex) {
  if (axisIndex == 1) {
    const double distanceFromBankTop = detectorSystemCentreDistanceFromBankTop(bankId);
    return 0.5 * getBankSize(bankId, 1) - distanceFromBankTop;
  }
  else { //axisIndex = 2
    const double distanceFromBankFront = detectorSystemFrontDistanceFromBankFront(bankId);
    return 0.5 * getBankSize(bankId, 2) - distanceFromBankFront;
  }
}

/// Borom Masks ///

std::array<std::array<double, 7>, 6> bcsBanks::rearBoronMasks = {{
    //dx, dy, dz, x,y,z, rot  (x,y,z of its top,left,front corner (in front view))
    {132.0, 2*870.0, 5.0,  0.0, 0.0, 0.0,  0.0},//left
    {1031.0, 101.0, 5.0,  117.5, 0.0, 59.0,  0.0},//top face
    {132.0, 2*870.0, 5.0,  1132.0, 0.0, 0.0,  0.0},//right face
    {1031.0, 42.0, 5.0,   117.5, 870.0+870.0-42.0, 59.0,  0.0},//bottom face
    {1216.0, 5.0, 125.0,  24.5, 95.39, 66.05,  tubeGridParallelogramAngle}, //top
    {1216.0, 5.0, 125.0,  24.5, 1698.06, 64.47,  tubeGridParallelogramAngle} //bottom
  }};

std::array<std::array<double, 7>, 8> bcsBanks::midVerticalBoronMasks = {{
    //dx, dy, dz, x,y,z,  (x,y,z of its top,left,front corner (in front view))
    {150.0, 377.0, 5.0,  0.0, 70.45, 0.0,  0.0}, //left face
    {150.0, 377.0, 5.0,  150.0+465.0, 70.45, 0.0,  0.0}, //right face 
    {465.0, 90.0, 5.0,  150.0, 447.45-90.0, 0.0,  0.0}, //bottom face
    {700.0, 5.0, 215.0,   32.5, 416.49, 27.13,  tubeGridParallelogramAngle}, //bottom
    {720.0, 47.0, 5.0,   22.5, 8.92, 59.24, tubeGridParallelogramAngle}, //ortogonal close to beam
    {135.5, 5.0, 300.0-0.05,  14.5, 70.03, 6.04, tubeGridParallelogramAngle}, //top left part
    {135.5, 5.0, 300.0-0.05,  14.5+135.5+465.0, 70.03, 6.04, tubeGridParallelogramAngle}, //top right part
    {465.0, 5.0, 245.0-0.05,  14.5+135.5, 57.23, 59.53, tubeGridParallelogramAngle}, //top middle part //57.24=70.03-cos(a)*55 59.53=6.04+sin(a)
  }};

std::array<std::array<double, 7>, 8> bcsBanks::midHorizontalBoronMasks = {{
    //dx, dy, dz, x,y,z,  (x,y,z of its top,left,front corner (in front view))
     {175.0, 519.5, 5.0,  0.0, 70.68, 0.0,  0.0}, //left face
     {175.0, 519.5, 5.0,  175.0+915.0, 70.68, 0.0,  0.0}, //right face
     {915.0, 129.5, 5.0,   175.0, 590.18-129.5, 0.0,  0.0}, //top face
     {1200.0, 5.0, 215.0,   20.0+12.5, 532.53, 27.13,  tubeGridParallelogramAngle}, //bottom
     {1220.0, 49.0, 5.0,   22.5, 6.98, 57.68, tubeGridParallelogramAngle}, //ortogonal close to beam
     {160.5, 5.0, 300.0-0.05,  14.5, 70.03, 4.94, tubeGridParallelogramAngle}, //top left part
     {160.5, 5.0, 300.0-0.05,  14.5+160.5+915, 70.03, 4.94, tubeGridParallelogramAngle}, //top right part
     {915.0, 5.0, 245.0-0.05,  14.5+160.5, 57.23, 58.43, tubeGridParallelogramAngle} //top middle part //57.24=70.03-cos(a)*55 58.43=4.94+sin(a)
  }};

std::array<std::array<double, 7>, 8> bcsBanks::frontTopBoronMasks = {{
    //dx, dy, dz, x,y,z,  (x,y,z of its top,left,front corner (in front view))
    {210.0, 884.0, 5.0,  0.0, 61.0, 0.0,  0.0}, //left face
    //{37.0, 864.0-162.0, 5.0,  173.0, 71.0, 0.0,  0.0}, //left face addition  
    {210.0, 884.0, 5.0,  210.0+1045.0, 61.0, 0.0,  0.0}, //right face  
    //{37.0, 864.0-162.0, 5.0,  173.0+1119.0-37.0, 71.0, 0.0,  0.0}, //right face addition  
    {1045.0, 162.0, 5.0,  210.0, 945.0-162.0-10.0, 0.0,  0.0}, //top face
    {1300.0, 5.0, 215.0,  82.5, 872.62, 22.27, tubeGridParallelogramAngle}, //bottom
    {1465.0, 43.0, 5.0,   0.0, 12.84, 64.25, tubeGridParallelogramAngle}, //ortogonal close to beam
    {173.0, 5.0, 305.0-0.05,  0.0, 71.2, 5.35-0.05, tubeGridParallelogramAngle}, //top left part
    {173.0, 5.0, 305.0-0.05,  173.0+1119.0, 71.2, 5.35-0.05, tubeGridParallelogramAngle}, //top right part
    {1119, 5.0, 250.0-0.05,  173, 58.40, 58.84-0.05, tubeGridParallelogramAngle} //top middle part //58.40=71.20-cos(a)*55 58.84=5.35+sin(a)
  }};

  std::array<std::array<double, 7>, 8> bcsBanks::frontBottomBoronMasks = {{
    //dx, dy, dz, x,y,z,  (x,y,z of its top,left,front corner (in front view))
    {210.0, 620, 5.0,  0.0, 61.42, 0.0,  0.0}, //left face
    //{37.0, 600-125.5, 5.0,  173.0, 71.43, 0.0,  0.0}, //left face addition
    {210.0, 620, 5.0,  210.0+1045.0, 61.42, 0.0,  0.0}, //right face
    //{37.0, 600-125.5, 5.0,  173.0+1119.0-37.0, 71.43, 0.0,  0.0}, //right face addition
    {1045.0, 125.5, 5.0,   210.0, 681.43-125.5-10.0, 0.0,  0.0}, //bottom face
    {1300.0, 5, 215.0,   82.5, 645.51, 21.78,  tubeGridParallelogramAngle}, //bottom face
    {1465.0, 43.0, 5.0,   0.0, 12.84, 64.15, tubeGridParallelogramAngle}, //ortogonal close to beam
    {173.0, 5.0, 305.0-0.05,   0.0, 71.20, 5.25, tubeGridParallelogramAngle}, //top: left part
    {1119.0, 5.0, 250.0-0.05,   173.0, 58.40, 58.74, tubeGridParallelogramAngle}, //top: middle part //58.40=71.20-cos(a)*55 58.74=5.25+sin(a)*55
    {173.0, 5.0, 305.0-0.05,   173.0+1119.0, 71.20, 5.25, tubeGridParallelogramAngle} //top: right
  }};

  std::array<std::array<double, 7>, 8> bcsBanks::frontVerticalBoronMasks = {{
    //dx, dy, dz, x,y,z,  (x,y,z of its top,left,front corner (in front view))
    {173.0, 944.0, 5.0,  0.0, 1016.11-944.0, 0.0,  0.0},//left 
    {173.0, 944.0, 5.0,  173.0+1119.0, 1016.11-944.0, 0.0,  0.0}, //right face
    {1119.0, 129.5, 5.0,   173.0, 1016.11-129.5, 0.0,  0.0}, //bottom face
    {1300.0, 5.0, 215.0,   83.0, 986.31, 21.78, tubeGridParallelogramAngle}, //bottom
    {1465.0, 48.0, 5.0,   0.0, 7.98, 62.98, tubeGridParallelogramAngle}, //ortogonal close to beam
    {173.0, 5.0, 305.0-0.05,   0.0, 71.20, 5.25, tubeGridParallelogramAngle}, //top: left part
    {1119.0, 5.0, 250.0-0.05,   173.0, 58.40, 58.74, tubeGridParallelogramAngle}, //top: middle part //58.40=71.20-cos(a)*55 58.74=5.25+sin(a)*55
    {173.0, 5.0, 305.0-0.05,   173.0+1119.0, 71.20, 5.25, tubeGridParallelogramAngle} //top: right
  }};

   std::array<std::array<double, 10>, 4> bcsBanks::triangularBoronMasks = {{
    // dxHalf dyHalf dzHalf, xCutPoint, yCutPoint, xCutDir(up/down), yCutDir(up/down), xCentreDistanceFromBankCentre, yCentreDistanceFromBankCentre
    { 200, 215, 2.5, 64, 65,  1,  1,  5,  892.5, -137.53+16.5 }, // SI-7615-691
    { 280, 240, 2.5, 69, 52, -1,  1,  5, -972.5, -22.53+16.5 },  // SI-7615-692
    { 225, 190, 2.5, 74, 73,  1, -1,  7,  917.5, 119.51+16.5 },  // SI-7615-693
    { 185, 210, 2.5, 60,  0, -1, -1,  7, -877.5, 69.52+16.5 }    // SI-7615-694
  }};
  

double bcsBanks::boronMaskParameter(const int bankId, const int maskId, const int parameterIndex){
  switch (bankId) {
  case 0:
    return rearBoronMasks[maskId][parameterIndex] *Units::mm;
  case 1:
  case 3:
    return midHorizontalBoronMasks[maskId][parameterIndex] *Units::mm;
  case 2:
  case 4:
    return midVerticalBoronMasks[maskId][parameterIndex] *Units::mm;
  case 5:
    return frontTopBoronMasks[maskId][parameterIndex] *Units::mm;
  case 6:
  case 8:
    return frontVerticalBoronMasks[maskId][parameterIndex] *Units::mm;
  case 7:
    return frontBottomBoronMasks[maskId][parameterIndex] *Units::mm;
  default:
    return 0;
  }
}

int bcsBanks::getNumberOfBoronMasks(const int bankId) {
  switch (bankId) {
  case 0:
    return rearBoronMasks.size();
  case 1:
  case 3:
    return midHorizontalBoronMasks.size();
  case 2:
  case 4:
    return midVerticalBoronMasks.size();
  case 5:
    return frontTopBoronMasks.size();
  case 6:
  case 8:
    return frontVerticalBoronMasks.size();
  case 7:
    return frontBottomBoronMasks.size();
  default:
    return 0;
  } 
}

double bcsBanks::getBoronMaskSize(const int bankId, const int maskId, const int axisIndex){
  return boronMaskParameter(bankId, maskId, axisIndex);
}
double bcsBanks::getBoronMaskRotation(const int bankId, const int maskId){
  return boronMaskParameter(bankId, maskId, 6);
}

double bcsBanks::getBoronMaskPosition(const int bankId, const int maskId, const int axisIndex){
  const double thickness = boronMaskParameter(bankId, maskId, axisIndex);
  const double position = boronMaskParameter(bankId, maskId, axisIndex+3);
  const double bankSizeHalf = 0.5*getBankSize(bankId, axisIndex);
  const double rotation = getBoronMaskRotation(bankId, maskId);

  if(axisIndex == 0){//x direction
    return bankSizeHalf - (position + 0.5*thickness);
  }
  else if(axisIndex == 1){  //y direction
    const double rotationCorrection = (1 - std::cos(rotation)) * 0.5*getBoronMaskSize(bankId, maskId, 1) - std::sin(rotation) * 0.5* (-getBoronMaskSize(bankId, maskId, 2));
    return bankSizeHalf - (position + 0.5*thickness) + rotationCorrection; 
  }
  else{ //z direction
    const double rotationCorrection = (1 - std::cos(rotation)) * 0.5*(-getBoronMaskSize(bankId, maskId, 2)) + std::sin(rotation) * 0.5*getBoronMaskSize(bankId, maskId, 1);
    return - bankSizeHalf + position + 0.5*thickness + rotationCorrection; 
  }
}

double bcsBanks::getTriangularBoronMaskParameter(const int maskId, const int parameterIndex){
  return triangularBoronMasks[maskId][parameterIndex] *Units::mm; //mm for bank id...would work though
}

double bcsBanks::getTriangularBoronMaskPosition(const int maskId, const int axisIndex){
  const int bankId = triangularBoronMasks[maskId][7];
  const double distance = bankDistance[bankId];

  const double halfThickness = getTriangularBoronMaskParameter(maskId, 2);
  const double verticalPosInBank = getTriangularBoronMaskParameter(maskId, 9);

  const double rotation = bankRotation[bankId][2];
  const double bankPosAngle = bankPositionAngle[bankId] *Units::degree;
  const double detFrontBankFrontDistance = detectorSystemFrontDistanceFromBankFront(bankId);

  if(axisIndex == 0){//x direction
    return bankPositionOffset[bankId][axisIndex] + getTriangularBoronMaskParameter(maskId, 8);
  }
  else if(axisIndex == 1){  //y direction
    return (distance * std::sin(bankPosAngle) - (halfThickness + detFrontBankFrontDistance) * std::sin(rotation) + verticalPosInBank * std::cos(rotation))*triangularBoronMasks[maskId][6];
  }
  else{ //z direction
    return distance * std::cos(bankPosAngle) - (halfThickness + detFrontBankFrontDistance) * std::cos(rotation) - verticalPosInBank * std::sin(rotation);
  }
}

 bool bcsBanks::isVertical(const int bankId){
   return (bankId == 2 || bankId == 4 || bankId == 6 || bankId == 8);
 }
bool bcsBanks::areTubesInverselyNumbered(const int bankId){  
   return (bankId == 1 || bankId == 2 || bankId == 5 || bankId == 6);
 }

// analysis 
int bcsBanks::getNumberOfPixels(const int bankId) {
  const int numberOfStrawsInBank = getNumberOfTubes(bankId) * 7;
  return numberOfStrawsInBank * numberOfPixels[bankId];;
}

int bcsBanks::getTotalNumberOfPixels() {
  return getBankPixelOffset(9);
}

int bcsBanks::getBankPixelOffset(const int bankId) {
  int offset = 0;
  for (int bankIndex = 0; bankIndex < bankId; bankIndex++) {
    const int numberOfStrawsInBank = getNumberOfTubes(bankIndex) * 7;
    offset += numberOfStrawsInBank * numberOfPixels[bankIndex];
  }
  return offset;
}

int bcsBanks::getPositionPixelId(const int bankId, const double positionX, const double positionY) const{
  const double pixelLength = strawLengthInBank[bankId] / numberOfPixels[bankId];

  if (isVertical(bankId)) { //vertical straw
    const double strawBegin = getBankPosition(bankId, 1) - 0.5* strawLengthInBank[bankId];
    return std::floor((positionY - strawBegin) / pixelLength);
  }
  else { //horizontal straw
    const double strawBegin = getBankPosition(bankId, 0) - 0.5* strawLengthInBank[bankId];
    const int invertedPixelId = std::floor((positionX - strawBegin) / pixelLength);
    return (numberOfPixels[bankId] - 1) - invertedPixelId; //pixels are numbered in minus x direction
  }
}

int bcsBanks::getPixelId(const int bankId, const int tubeId, const int strawId, const double positionX, const double positionY) const{
  const int bankPixelOffset = getBankPixelOffset(bankId);
  const int strawPixelOffset = (tubeId * 7 + strawId) * numberOfPixels[bankId];
  const int positionPixelId = getPositionPixelId(bankId, positionX, positionY);
  return bankPixelOffset + strawPixelOffset + positionPixelId;
}


//////// Utilities for getting the centre coordinates of a pixel ////////
double bcsBanks::pixelCentrePosition[3] = { 0.0, 0.0, 0.0};

double bcsBanks::getPixelCentrePosition(const int axisIndex){
  return pixelCentrePosition[axisIndex];
}

void bcsBanks::calcPixelCentrePositionForMasking(const int pixelId) { 
  const bool isNewPixelNumbering = false;
  const int bankId = getBankId(pixelId);
  const int tubeId = getTubeId(pixelId, bankId);
  const int strawId = getStrawId(pixelId, bankId, tubeId);
  const int locPixelId = pixelId % numberOfPixels[bankId];
  
  
  ///////// pixel in straw /////////
  const double pixelLength = strawLengthInBank[bankId] / numberOfPixels[bankId];
  //const int vertical = isVertical(bankId);
  double positionZ = 0.0;

  /*
  if (vertical) { //vertical straw
    positionZ = - 0.5* strawLengthInBank[bankId] + (locPixelId + 0.5) * pixelLength;
  }
  else { //horizontal straw - pixels are numbered in minus x direction
    const int invertedPixelId = (numberOfPixels[bankId] - 1) - locPixelId;
    positionZ = - 0.5* strawLengthInBank[bankId] + (invertedPixelId + 0.5) * pixelLength;
    std::cout<<"\n locPixelId:"<<locPixelId<<"\t before: "<<positionZ << "\t";
    positionZ = 0.5* strawLengthInBank[bankId] - (locPixelId + 0.5) * pixelLength;
    std::cout<<" after: "<<positionZ << "\n";
  }*/
  positionZ = - 0.5* strawLengthInBank[bankId] + (locPixelId + 0.5) * pixelLength;

  ///////// straw in tube /////////
  double positionX = getStrawPositionX(strawId);
  double positionY = getStrawPositionY(strawId);

  ///////// tube in pack ///////// 
  const double tubeRotation = getTubeRotation();
  const double horizontalOffset = getHorizontalTubeCentreOffsetInPack();
  const int numberOfPacks = getNumberOfPacksByBankId(bankId);
  const int newTubeIdConvertedToOldId = ((tubeId % 2) * 4) + ((int) tubeId / (numberOfPacks * 2));
  const int inPackTubeId = isNewPixelNumbering ? 
                           areTubesInverselyNumbered(bankId) ? (newTubeIdConvertedToOldId + 4) % 8 : newTubeIdConvertedToOldId % 8 :
                           areTubesInverselyNumbered(bankId) ? (tubeId + 4) % 8 : tubeId % 8;
  const double horizontalTubeDistance = getHorizontalTubeDistanceInPack();
  const double topRowOffset = getTopRowOffsetInPack();  
  const double verticalOffset = getVerticalTubeCentreOffsetInPack(); 
  
  // apply tube rotation
  coordinateRotation(positionX, positionY, tubeRotation);

  // place tube in pack
  positionX += horizontalOffset + (double) (inPackTubeId % 4) * horizontalTubeDistance;
  if (inPackTubeId < 4) { positionX += topRowOffset; }
  positionY += (inPackTubeId < 4) ? verticalOffset : -verticalOffset;

  ///////// pack in bank /////////
  const double pack_pack_distance = getPackPackDistance();
  const double packRotation = getPackRotation();
  
  const int normalPackId = isNewPixelNumbering ? 
                           (int) (tubeId % (numberOfPacks * 2)) / 2 :
                           (int) tubeId / 8;
  const int packNumber = !areTubesInverselyNumbered(bankId) ? normalPackId : ((numberOfPacks - 1) - normalPackId);

  // apply pack rotation
  coordinateRotation(positionX, positionY, packRotation);

  // place pack in bank
  positionX += getTopmostPackPositionInBank(bankId, 2);
  positionY += -(packNumber * pack_pack_distance - getTopmostPackPositionInBank(bankId, 1));
  positionZ += getTopmostPackPositionInBank(bankId, 0);

  ///////// bank position /////////
  const double rotateZ = getBankRotation(bankId, 2);
  const double rotateX = getBankRotation(bankId, 0);
  const double rotateY = -getBankRotation(bankId, 1);

  // apply bank rotation
  coordinateRotation(positionY, positionX, rotateZ); // Assuming left-handed coordinate sytem
  coordinateRotation(positionZ, positionY, rotateX);
  coordinateRotation(positionZ, positionX, rotateY);

  // place bank in world 
  positionX += getBankPosition(bankId, 0);
  positionY += getBankPosition(bankId, 1);
  positionZ += getBankPosition(bankId, 2);
  
  ///////// END - set values as class properties for output /////////
  pixelCentrePosition[0] = positionX;
  pixelCentrePosition[1] = positionY;
  pixelCentrePosition[2] = positionZ;

  //const int packId = (int) tubeId / 8;
  //std::cout<< "pixelId: " << pixelId << '\t' <<"bankId: " << bankId << '\t' <<"packId: " << packId << '\t' <<"tubeId: " << tubeId << '\t' <<"strawId: " << strawId << '\t' <<"locPixeId: " << locPixelId << "vertical: " << vertical <<'\n' ; 
  return;
}

void bcsBanks::coordinateRotation(double &x, double &y, const double angle) {
  double tempX = std::cos(angle) * x - std::sin(angle) * y;
  double tempY = std::sin(angle) * x + std::cos(angle) * y;
  x = tempX;
  y = tempY;
}

int bcsBanks::getBankId(const int pixelId) {
  for (int bankId = 0; bankId < 9; bankId++){
    if(pixelId < getBankPixelOffset(bankId+1)){
      return bankId;
    }
  }
  return 0; //this is an error case, could be handled properly
}

int bcsBanks::getTubeId(const int pixelId, const int bankId) {
  const int pixelIdInBank = pixelId - getBankPixelOffset(bankId);
  const int numberOfPixelsInATube = numberOfPixels[bankId] * 7;
  return (int) pixelIdInBank / numberOfPixelsInATube;
}

int bcsBanks::getStrawId(const int pixelId, const int bankId, const int tubeId) {
  const int pixelIdInBank = pixelId - getBankPixelOffset(bankId);
  const int pixelIdInTube = pixelIdInBank - tubeId * 7 * numberOfPixels[bankId];
  return (int) pixelIdInTube / numberOfPixels[bankId]; 
}
