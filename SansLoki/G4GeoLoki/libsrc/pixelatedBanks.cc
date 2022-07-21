#include "G4GeoLoki/pixelatedBanks.hh"
#include "Core/Units.hh"
#include <cmath>
#include <iostream>
#include <array>

int pixelatedBanks::numberOfPixelsInStraw[9] = { // number of pixels along the straws
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

int pixelatedBanks::getNumberOfPixels(const int bankId) {
  const int numberOfStrawsInBank = getNumberOfTubes(bankId) * 7;
  return numberOfStrawsInBank * numberOfPixelsInStraw[bankId];
}

int pixelatedBanks::getTotalNumberOfPixels() {
  return getBankPixelOffset(9);
}

int pixelatedBanks::getBankPixelOffset(const int bankId) {
  int offset = 0;
  for (int bankIndex = 0; bankIndex < bankId; bankIndex++) {
    const int numberOfStrawsInBank = getNumberOfTubes(bankIndex) * 7;
    offset += numberOfStrawsInBank * numberOfPixelsInStraw[bankIndex];
  }
  return offset;
}

int pixelatedBanks::getPositionPixelId(const int bankId, const double positionX, const double positionY) const{
  const double pixelLength = getStrawLengthByBankId(bankId) / numberOfPixelsInStraw[bankId];

  if (isVertical(bankId)) { //vertical straw
    const double strawBegin = getBankPosition(bankId, 1) - 0.5* getStrawLengthByBankId(bankId);
    return std::floor((positionY - strawBegin) / pixelLength);
  }
  else { //horizontal straw
    const double strawBegin = getBankPosition(bankId, 0) - 0.5* getStrawLengthByBankId(bankId);
    const int invertedPixelId = std::floor((positionX - strawBegin) / pixelLength);
    return (numberOfPixelsInStraw[bankId] - 1) - invertedPixelId; //pixels are numbered in minus x direction
  }
}

int pixelatedBanks::getPixelId(const int bankId, const int tubeId, const int strawId, const double positionX, const double positionY) const{
  const int bankPixelOffset = getBankPixelOffset(bankId);
  const int strawPixelOffset = (tubeId * 7 + strawId) * numberOfPixelsInStraw[bankId];
  const int positionPixelId = getPositionPixelId(bankId, positionX, positionY);
  return bankPixelOffset + strawPixelOffset + positionPixelId;
}