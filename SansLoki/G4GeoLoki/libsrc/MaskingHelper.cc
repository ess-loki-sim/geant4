#include "G4GeoLoki/MaskingHelper.hh"
#include "Core/Units.hh"
#include <cmath>
#include <iostream>
#include <array>
#include <cassert>

//////// Utilities for getting the centre coordinates of a pixel ////////
double MaskingHelper::pixelCentrePosition[3] = { 0.0, 0.0, 0.0 };

double MaskingHelper::getPixelCentrePosition(const int axisIndex) const{
  assert(0 <= axisIndex && axisIndex <= 2);
  return pixelCentrePosition[axisIndex];
}

void MaskingHelper::calcPixelCentrePositionForMasking(const int pixelId, const bool isNewPixelNumbering) { 
  const int bankId = getBankId(pixelId);
  const int tubeId = getTubeId(pixelId, bankId);
  const int inPackTubeId = getInPackTubeId(bankId, tubeId, isNewPixelNumbering);
  const int packId = getPackId(bankId, tubeId, isNewPixelNumbering);
  const int strawId = getStrawId(pixelId, bankId, tubeId);

  ///////// pixel in straw /////////
  double positionZ = getPixelPositionInStraw(pixelId, bankId);

  ///////// straw in tube /////////
  double positionX = BcsTube::getStrawPositionX(strawId);
  double positionY = BcsTube::getStrawPositionY(strawId);

  ///////// tube in pack ///////// 
  // apply tube rotation
  coordinateRotation(positionX, positionY, BcsPack::getTubeRotation());
  // place tube in pack
  positionX += BcsPack::getHorizontalTubeOffset(inPackTubeId);
  positionY += BcsPack::getVerticalTubeOffset(inPackTubeId);

  ///////// pack in bank /////////
  // apply pack rotation
  coordinateRotation(positionX, positionY, getPackRotation());
  // place pack in bank
  positionX += getPackPositionInBank(bankId, packId, 2);
  positionY += getPackPositionInBank(bankId, packId, 1);
  positionZ += getPackPositionInBank(bankId, packId, 0);

  ///////// bank position /////////
  // apply bank rotations
  coordinateRotation(positionY, positionX, getBankRotation(bankId, 2)); // Assuming left-handed coordinate sytem
  coordinateRotation(positionZ, positionY, getBankRotation(bankId, 0));
  coordinateRotation(positionZ, positionX, -getBankRotation(bankId, 1));
  // place bank in world 
  positionX += getBankPosition(bankId, 0);
  positionY += getBankPosition(bankId, 1);
  positionZ += getBankPosition(bankId, 2);
  
  ///////// END - set values /////////
  pixelCentrePosition[0] = positionX;
  pixelCentrePosition[1] = positionY;
  pixelCentrePosition[2] = positionZ;

  return;
}

void MaskingHelper::coordinateRotation(double &x, double &y, const double angle) {
  double tempX = std::cos(angle) * x - std::sin(angle) * y;
  double tempY = std::sin(angle) * x + std::cos(angle) * y;
  x = tempX;
  y = tempY;
}

int MaskingHelper::getBankId(const int pixelId) {
  for (int bankId = 0; bankId < 9; bankId++){
    if(pixelId < getBankPixelOffset(bankId+1)){
      return bankId;
    }
  }
  return 0; //this is an error case, could be handled properly
}

int MaskingHelper::getPackId(const int bankId, const int tubeId, const bool isNewPixelNumbering) {
  const int numberOfPacks = getNumberOfPacksByBankId(bankId);
  const int normalPackId = isNewPixelNumbering ? 
                           (int) (tubeId % (numberOfPacks * 2)) / 2 :
                           (int) tubeId / 8;
  return !areTubesInverselyNumbered(bankId) ? normalPackId : ((numberOfPacks - 1) - normalPackId);
}

int MaskingHelper::getInPackTubeId(const int bankId, const int tubeId, const bool isNewPixelNumbering) {
  const int numberOfPacks = getNumberOfPacksByBankId(bankId);
  const int newTubeIdConvertedToOldId = ((tubeId % 2) * 4) + ((int) tubeId / (numberOfPacks * 2));
  
  if(isNewPixelNumbering){
    return areTubesInverselyNumbered(bankId) ? (newTubeIdConvertedToOldId + 4) % 8 : newTubeIdConvertedToOldId % 8;
  }
  else{
    return areTubesInverselyNumbered(bankId) ? (tubeId + 4) % 8 : tubeId % 8;
  }
}

int MaskingHelper::getTubeId(const int pixelId, const int bankId) {
  const int pixelIdInBank = pixelId - getBankPixelOffset(bankId);
  const int numberOfPixelsInATube = getNumberOfPixelsInStraw(bankId) * 7;
  return (int) pixelIdInBank / numberOfPixelsInATube;
}

int MaskingHelper::getStrawId(const int pixelId, const int bankId, const int tubeId) {
  const int pixelIdInBank = pixelId - getBankPixelOffset(bankId);
  const int pixelIdInTube = pixelIdInBank - tubeId * 7 * getNumberOfPixelsInStraw(bankId);
  return (int) pixelIdInTube / getNumberOfPixelsInStraw(bankId); 
}

double MaskingHelper::getPixelPositionInStraw(const int pixelId, const int bankId) {
  const int locPixelId = pixelId % getNumberOfPixelsInStraw(bankId);
  const double pixelLength = getStrawLengthByBankId(bankId) / getNumberOfPixelsInStraw(bankId);

  return - 0.5* getStrawLengthByBankId(bankId) + (locPixelId + 0.5) * pixelLength;
}