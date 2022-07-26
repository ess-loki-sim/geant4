#include "G4GeoLoki/MaskingHelper.hh"
#include "Core/Units.hh"
#include <cmath>
#include <iostream>
#include <array>
#include <cassert>

//////// Utilities for getting the centre coordinates of a pixel ////////
double MaskingHelper::pixelCentrePosition[3] = { 0.0, 0.0, 0.0};

double MaskingHelper::getPixelCentrePosition(const int axisIndex){
  assert(0 <= axisIndex && axisIndex <= 2);
  return pixelCentrePosition[axisIndex];
}

void MaskingHelper::calcPixelCentrePositionForMasking(const int pixelId) { 
  const bool isNewPixelNumbering = false;
  const int bankId = getBankId(pixelId);
  const int tubeId = getTubeId(pixelId, bankId);
  const int strawId = getStrawId(pixelId, bankId, tubeId);
  const int locPixelId = pixelId % getNumberOfPixelsInStraw(bankId);
  
  
  ///////// pixel in straw /////////
  const double pixelLength = getStrawLengthByBankId(bankId) / getNumberOfPixelsInStraw(bankId);
  //const int vertical = isVertical(bankId);
  double positionZ = 0.0;

  /*
  if (vertical) { //vertical straw
    positionZ = - 0.5* strawLengthInBank[bankId] + (locPixelId + 0.5) * pixelLength;
  }
  else { //horizontal straw - pixels are numbered in minus x direction
    const int invertedPixelId = (numberOfPixelsInStraw[bankId] - 1) - locPixelId;
    positionZ = - 0.5* strawLengthInBank[bankId] + (invertedPixelId + 0.5) * pixelLength;
    std::cout<<"\n locPixelId:"<<locPixelId<<"\t before: "<<positionZ << "\t";
    positionZ = 0.5* strawLengthInBank[bankId] - (locPixelId + 0.5) * pixelLength;
    std::cout<<" after: "<<positionZ << "\n";
  }*/
  positionZ = - 0.5* getStrawLengthByBankId(bankId) + (locPixelId + 0.5) * pixelLength;

  ///////// straw in tube /////////
  double positionX = tubes->getStrawPositionX(strawId);
  double positionY = tubes->getStrawPositionY(strawId);

  ///////// tube in pack ///////// 
  const double tubeRotation = packs->getTubeRotation();
  const double horizontalOffset = packs->getHorizontalTubeCentreOffsetInPack();
  const int numberOfPacks = getNumberOfPacksByBankId(bankId);
  const int newTubeIdConvertedToOldId = ((tubeId % 2) * 4) + ((int) tubeId / (numberOfPacks * 2));
  const int inPackTubeId = isNewPixelNumbering ? 
                           areTubesInverselyNumbered(bankId) ? (newTubeIdConvertedToOldId + 4) % 8 : newTubeIdConvertedToOldId % 8 :
                           areTubesInverselyNumbered(bankId) ? (tubeId + 4) % 8 : tubeId % 8;
  const double horizontalTubeDistance = packs->getHorizontalTubeDistanceInPack();
  const double topRowOffset = packs->getTopRowOffsetInPack();  
  const double verticalOffset = 0.5*packs->getVerticalTubeDistanceInPack(); 
  
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
