
#include "G4GeoLoki/bcsBanks.hh"
#include "Core/Units.hh"
#include <cmath>
#include <iostream>
#include <array>
#include <cassert>

double bcsBanks::packHolderDistanceFromPackTop = 7.6 *Units::mm;
double bcsBanks::packHolderDistanceFromPackFront = 7.5 *Units::mm;

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
  assert(0 <= bankId && bankId <= 8);
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
  assert(0 <= bankId && bankId <= 8);
  double intendedPosition = bankDistance[bankId] * std::cos(bankPositionAngle[bankId]*Units::degree);
  double bankCentreOffsetZ = detectorSystemCentreOffsetInBank(bankId, 2) * std::cos(calcBankRotation(bankId)) - detectorSystemCentreOffsetInBank(bankId, 1) * std::sin(calcBankRotation(bankId));
  return intendedPosition + bankCentreOffsetZ;
}
double bcsBanks::calcBankPositionXY(const int bankId) {
  assert(0 <= bankId && bankId <= 8);
  double intendedPosition = bankDistance[bankId] * std::sin(bankPositionAngle[bankId]*Units::degree);
  double bankCentreOffsetXY = detectorSystemCentreOffsetInBank(bankId, 2) * std::sin(calcBankRotation(bankId)) + detectorSystemCentreOffsetInBank(bankId, 1) * std::cos(calcBankRotation(bankId));

  return intendedPosition + bankCentreOffsetXY;
}

const int bcsBanks::bankPosDir[9] = { 1, 1, 1, -1, -1, 1, 1, -1, -1}; 

double bcsBanks::bankPosition[9][3] = {
    {0, 0, 0}, // 0 - rear !calculated in getBankPosition function!
    {0, calcBankPositionXY(1), calcBankPositionZ(1)},  // 1 - mid top
    {calcBankPositionXY(2), 0, calcBankPositionZ(2)}, // 2 - mid left
    {0, calcBankPositionXY(3)*bankPosDir[3], calcBankPositionZ(3)},  // 3 - mid bottom
    {calcBankPositionXY(4)*bankPosDir[3], 0, calcBankPositionZ(4)}, // 4 - mid right
    {0, calcBankPositionXY(5), calcBankPositionZ(5)}, // 5 - front top
    {calcBankPositionXY(6), 0, calcBankPositionZ(6)}, // 6 - front left
    {0, calcBankPositionXY(7)*bankPosDir[3], calcBankPositionZ(7)}, // 7 - front bottom
    {calcBankPositionXY(8)*bankPosDir[3], 0, calcBankPositionZ(8)}, // 8 - front right
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

 double bcsBanks::getPackRotation() {
   return packs->getTubeGridParallelogramAngle(); 
 }

double bcsBanks::getPackPackDistance() { 
  return packs->getTubeGridParallelogramSide() * 2; 
}


/// bank ///
double bcsBanks::getStrawLengthByBankId(const int bankId) {
  assert(0 <= bankId && bankId <= 8);
  return strawLengthInBank[bankId] * Units::mm;
}
int bcsBanks::getNumberOfPacksByBankId(const int bankId) {
  assert(0 <= bankId && bankId <= 8);
  return numberOfPacksInBank[bankId];
}
int bcsBanks::getNumberOfTubes(const int bankId){
  assert(0 <= bankId && bankId <= 8);
  return numberOfPacksInBank[bankId] * 8;
}

double bcsBanks::getBankRotation(const int bankId, const int axisIndex) {
  assert(0 <= bankId && bankId <= 8);
  assert(0 <= axisIndex && axisIndex <= 2); 
  return bankRotation[bankId][axisIndex]; 
}

double bcsBanks::getBankPosition(const int bankId, const int axisIndex) const {
  assert(0 <= bankId && bankId <= 8);
  assert(0 <= axisIndex && axisIndex <= 2); 
  if(bankId == 0){
    double rearBankPosition[] = {0, -detectorSystemCentreOffsetInBank(bankId, 1), rearBankDistance + detectorSystemCentreOffsetInBank(bankId, 2)};
    return rearBankPosition[axisIndex];
  }
  else{
    return bankPosition[bankId][axisIndex] + bankPositionOffset[bankId][axisIndex];
  }  
}
double bcsBanks::getBankSize(const int bankId, const int axisIndex) {
  assert(0 <= bankId && bankId <= 8);
  assert(0 <= axisIndex && axisIndex <= 2); 
  return bankSize[bankId][axisIndex]+0.05 *Units::mm;
}

double bcsBanks::packHolderToPackCentreCoordsInPack(const int axisIndex) {
  assert(1 <= axisIndex && axisIndex <= 2);
  if(axisIndex == 1) { //y
    return 0.5*packs->getPackBoxHeight() - packHolderDistanceFromPackTop;
  }
  else {//z
    return 0.5*packs->getPackBoxWidth() - packHolderDistanceFromPackFront;
  }
}

double bcsBanks::getTopmostPackPositionInBank(const int bankId, const int axisIndex) {
  assert(0 <= bankId && bankId <= 8);
  assert(0 <= axisIndex && axisIndex <= 2); 
  if(axisIndex == 0){ //x direction
    return 0;
  }
  else if(axisIndex == 1) { //y direction
  const double packHolderPositionYFromBankTop = topmostPackHolderPositionInBankFromTopFront[bankId][0];
  const double packHolderPositionY = 0.5* getBankSize(bankId, 1)  - packHolderPositionYFromBankTop;
  
  const double positionOfPackCentreFromPackHolderY = packHolderToPackCentreCoordsInPack(2) *std::sin(getPackRotation()) - packHolderToPackCentreCoordsInPack(1) *std::cos(getPackRotation());

  return packHolderPositionY + positionOfPackCentreFromPackHolderY;
  }
  else{ //z direction
  const double packHolderPositionZFromBankFront = topmostPackHolderPositionInBankFromTopFront[bankId][1];
  const double packHolderPositionZ = - (0.5*getBankSize(bankId, 2) - packHolderPositionZFromBankFront);

  const double positionOfPackCentreFromPackHolderZ = packHolderToPackCentreCoordsInPack(2) *std::cos(getPackRotation()) + packHolderToPackCentreCoordsInPack(1) *std::sin(getPackRotation());
  return packHolderPositionZ + positionOfPackCentreFromPackHolderZ;
  }
}

double bcsBanks::packHolderToFirstTubeCentreCoordsInPack(const int axisIndex) {
  assert(1 <= axisIndex && axisIndex <= 2);
  if(axisIndex == 1) { //y
    return 0.5*packs->getPackBoxHeight() + 0.5*packs->getVerticalTubeDistanceInPack() - packHolderDistanceFromPackTop;
  }
  else {//z
    return packs->getTubeCentreDistanceFromPackFront() - packHolderDistanceFromPackFront;
  }
}

double bcsBanks::detectorSystemFrontDistanceFromBankFront(const int bankId) {
  assert(0 <= bankId && bankId <= 8); 
  const double packHolderPositionZFromBankFront = topmostPackHolderPositionInBankFromTopFront[bankId][1];
  const double positionOfFirstTubeCentreFromPackHolderZ = packHolderToFirstTubeCentreCoordsInPack(2) *std::cos(getPackRotation()) + packHolderToFirstTubeCentreCoordsInPack(1) *std::sin(getPackRotation());

  return packHolderPositionZFromBankFront + positionOfFirstTubeCentreFromPackHolderZ - tubes->getTubeOuterRadius();
}

double bcsBanks::detectorSystemCentreDistanceFromBankTop(const int bankId) {
  assert(0 <= bankId && bankId <= 8);
  const double packHolderDistanceYFromBankTop = topmostPackHolderPositionInBankFromTopFront[bankId][0];

  const double distanceOfSecondRowTubeCentreFromPackHolderY = std::abs(packHolderToFirstTubeCentreCoordsInPack(2) * std::sin(getPackRotation()) - packHolderToFirstTubeCentreCoordsInPack(1) * std::cos(getPackRotation()));
  const double distanceOfFirstRowTubeCentreFromPackHolderY = distanceOfSecondRowTubeCentreFromPackHolderY - packs->getTubeGridParallelogramSide();
  const double detectorSystemSizeY = (numberOfPacksInBank[bankId] * 2 - 1) * packs->getTubeGridParallelogramSide(); // from top row centre to lowest row centre
  
  const double distanceOfDetectorSystemCentreFromPackHolderY = distanceOfFirstRowTubeCentreFromPackHolderY + 0.5 * detectorSystemSizeY;

  return packHolderDistanceYFromBankTop + distanceOfDetectorSystemCentreFromPackHolderY;
}

double bcsBanks::detectorSystemCentreOffsetInBank(const int bankId, const int axisIndex) {
  assert(1 <= axisIndex && axisIndex <= 2);
  if (axisIndex == 1) {
    const double distanceFromBankTop = detectorSystemCentreDistanceFromBankTop(bankId);
    return 0.5 * getBankSize(bankId, 1) - distanceFromBankTop;
  }
  else { //axisIndex = 2
    const double distanceFromBankFront = detectorSystemFrontDistanceFromBankFront(bankId);
    return 0.5 * getBankSize(bankId, 2) - distanceFromBankFront;
  }
}

bool bcsBanks::isVertical(const int bankId) {
  assert(0 <= bankId && bankId <= 8);
  return (bankId == 2 || bankId == 4 || bankId == 6 || bankId == 8);
}
bool bcsBanks::areTubesInverselyNumbered(const int bankId) {
  assert(0 <= bankId && bankId <= 8);
  return (bankId == 1 || bankId == 2 || bankId == 5 || bankId == 6);
}

/// Borom Masks ///

double bcsBanks::getBoronMaskPosition(const int bankId, const int maskId, const int axisIndex) {
  assert(0 <= axisIndex && axisIndex <= 2);
  const double thickness =  masks->getSize(bankId, maskId, axisIndex);
  const double position =  masks->getPosition(bankId, maskId, axisIndex);
  const double bankSizeHalf = 0.5*getBankSize(bankId, axisIndex);
  const double rotation =  masks->getRotation(bankId, maskId);

  if(axisIndex == 0){//x direction
    return bankSizeHalf - (position + 0.5*thickness);
  }
  else if(axisIndex == 1){  //y direction
    const double rotationCorrection = (1 - std::cos(rotation)) * 0.5*masks->getSize(bankId, maskId, 1) - std::sin(rotation) * 0.5* (-masks->getSize(bankId, maskId, 2));
    return bankSizeHalf - (position + 0.5*thickness) + rotationCorrection; 
  }
  else{ //z direction
    const double rotationCorrection = (1 - std::cos(rotation)) * 0.5*(-masks->getSize(bankId, maskId, 2)) + std::sin(rotation) * 0.5*masks->getSize(bankId, maskId, 1);
    return - bankSizeHalf + position + 0.5*thickness + rotationCorrection; 
  }
}

double bcsBanks::getTriangularBoronMaskPosition(const int maskId, const int axisIndex) {
  assert(0 <= axisIndex && axisIndex <= 2);
  const int bankId = masks->getBankIdOfTriangularMask(maskId);
  const double distance = bankDistance[bankId];

  const double halfThickness = masks->getHalfSizeOfTriangularMask(maskId, 2);
  const double verticalPosInBank = masks->getPosInBankOfTriangularMask(maskId, 1);

  const double rotation = bankRotation[bankId][2];
  const double bankPosAngle = bankPositionAngle[bankId] *Units::degree;
  const double detFrontBankFrontDistance = detectorSystemFrontDistanceFromBankFront(bankId);

  if(axisIndex == 0){//x direction
    return bankPositionOffset[bankId][axisIndex] + masks->getPosInBankOfTriangularMask(maskId, 0);
  }
  else if(axisIndex == 1){  //y direction
    return (distance * std::sin(bankPosAngle) - (halfThickness + detFrontBankFrontDistance) * std::sin(rotation) + verticalPosInBank * std::cos(rotation))*bankPosDir[bankId];
  }
  else{ //z direction
    return distance * std::cos(bankPosAngle) - (halfThickness + detFrontBankFrontDistance) * std::cos(rotation) - verticalPosInBank * std::sin(rotation);
  }
}

 double bcsBanks::getCalibMaskPosition(calibMasks::calibMasksBase calibMask, const int bankId, const int axisIndex) const {
  assert(0 <= axisIndex && axisIndex <= 2);
  //TODO: instead of vertical/horisontal use along tube / perpendicular
  //TODO: mention that elevation is up or to the left

  const double offsetPerpendicularToBCSTubes = 0.; //possible extension, needs implementation in calibMasks::calibMasksBase class
  const double offsetAlongBCSTubes = 0.5*getStrawLengthByBankId(bankId) - calibMask.leftTubeEndDistance - 0.5*calibMask.getWidth();
  const double elevationFromBankCentre = 0.5*getBankSize(bankId,2) + 0.5*calibMask.thickness + calibMask.elevationFromMaskFront;
  const double bankRot = bankRotation[bankId][2];

  if((axisIndex == 0 && !isVertical(bankId)) || (axisIndex == 1 && isVertical(bankId))) {
    return bankPositionOffset[bankId][axisIndex] + offsetAlongBCSTubes;
  }
  else if((axisIndex == 0 && isVertical(bankId)) || (axisIndex == 1 && !isVertical(bankId))) {
    return getBankPosition(bankId,axisIndex) 
           - (elevationFromBankCentre * std::sin(bankRot) *bankPosDir[bankId]) 
           + (offsetPerpendicularToBCSTubes * std::cos(bankRot));
  }
  else{ //z direction
    return getBankPosition(bankId, 2) 
           - elevationFromBankCentre * std::cos(bankRot) 
           - offsetPerpendicularToBCSTubes * std::sin(bankRot) *bankPosDir[bankId];
  }
 }