#ifndef G4GeoLoki_bcsBanks_hh
#define G4GeoLoki_bcsBanks_hh

#include <string>
#include <array>
#include "G4GeoLoki/bcsTube.hh"
#include "G4GeoLoki/bcsPack.hh"
#include "G4GeoLoki/boronMasks.hh"

class bcsBanks {
public:
  bcsBanks(double locRearBankDistance){
    rearBankDistance = locRearBankDistance;
  }
  bcsBanks(double locRearBankDistance, int rearBankPixelNumber){
    rearBankDistance = locRearBankDistance;
    numberOfPixels[0] = rearBankPixelNumber;
  }

  static bcsTube* tubes;
  static bcsPack* packs;

  /// bank ///
  static double getPackRotation();
  double getPackPackDistance() const;// TODO better name?
  static double getTopmostPackPositionInBank(const int bankId, const int axisIndex); // 0 - x, 1 - y, 2 - z

  double getStrawLengthByBankId(const int bankId) const;
  int getNumberOfPacksByBankId(const int bankId) const; 
  static int getNumberOfTubes(const int bankId); 

  double getBankRotation(const int bankId, const int axisIndex) const; // 0 - x, 1 - y, 2 - z
  double getBankPosition(const int bankId, const int axisIndex) const; // 0 - x, 1 - y, 2 - z
  static double getBankSize(const int bankId, const int axisIndex); // 0 - x, 1 - y, 2 - z

  static double detectorSystemFrontDistanceFromBankFront(const int bankId);

  static bool isVertical(const int bankId);
  static bool areTubesInverselyNumbered(const int bankId);

  // boron masks
  static boronMasks * masks;
  static double getBoronMaskPosition(const int bankId, const int maskId, const int axisIndex);
  static double getTriangularBoronMaskPosition(const int maskId, const int axisIndex);

  // analysis
  static int getTotalNumberOfPixels();
  static int getNumberOfPixels(const int bankId);
  int getPixelId(const int bankId, const int tubeId, const int strawId, const double positionX, const double positionY) const;

  // masking
  void calcPixelCentrePositionForMasking(const int pixelId);
  double getPixelCentrePosition(const int axisIndex);

private:
  double rearBankDistance; //the only(?) non-static
  
  static double pixelCentrePosition[3]; //used for masking

  static double packHolderDistanceFromPackTop;
  static double packHolderDistanceFromPackFront;

  /// bank //
  static double strawLengthInBank[9];
  static int numberOfPacksInBank[9];

  static double bankRotation[9][3];
  static double bankPositionAngle[9];
  static double bankTiltAngle[9];
  static double calcBankRotation(const int bankId);

  static double bankPosition[9][3];
  static double bankPositionOffset[9][3]; 
  static double bankSize[9][3];
  static double topmostPackHolderPositionInBankFromTopFront[9][2];
  static double packHolderToPackCentreCoordsInPack(const int axisIndex);

  static double bankDistance[9];
  static double calcBankPositionZ(const int bankId);
  static double calcBankPositionXY(const int bankId);

  static double packHolderToFirstTubeCentreCoordsInPack(const int axisIndex);
  static double detectorSystemCentreDistanceFromBankTop(const int bankId);
  static double detectorSystemCentreOffsetInBank(const int bankId, const int axisIndex);

  // analysis
  static int numberOfPixels[9];
  static int getBankPixelOffset(const int bankId);
  int getPositionPixelId(const int bankId, const double positionX, const double positionY) const;

  // masking
  static void coordinateRotation(double &x, double &y, const double angle);
  static int getBankId(const int pixelId);
  static int getTubeId(const int pixelId, const int bankId);
  static int getStrawId(const int pixelId, const int bankId, const int tubeId);
};

#endif
