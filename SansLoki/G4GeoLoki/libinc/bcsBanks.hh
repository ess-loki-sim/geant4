#ifndef G4GeoLoki_bcsBanks_hh
#define G4GeoLoki_bcsBanks_hh

#include <array>
#include "G4GeoLoki/bcsTube.hh"
#include "G4GeoLoki/bcsPack.hh"
#include "G4GeoLoki/boronMasks.hh"

class bcsBanks {
public:
  bcsBanks(double rearBankDistance): rearBankDistance(rearBankDistance){}

  /// bcs tubes ///
  static bcsTube* tubes;

  /// bcs packs ///
  static bcsPack* packs;

  /// banks ///
  static double getPackRotation();
  static double getPackPackDistance();// TODO better name?
  static double getTopmostPackPositionInBank(const int bankId, const int axisIndex); // 0 - x, 1 - y, 2 - z

  static double getStrawLengthByBankId(const int bankId);
  static int getNumberOfPacksByBankId(const int bankId); 
  static int getNumberOfTubes(const int bankId); 

  static double getBankRotation(const int bankId, const int axisIndex); // 0 - x, 1 - y, 2 - z
  double getBankPosition(const int bankId, const int axisIndex) const; // 0 - x, 1 - y, 2 - z
  static double getBankSize(const int bankId, const int axisIndex); // 0 - x, 1 - y, 2 - z

  static double detectorSystemFrontDistanceFromBankFront(const int bankId);

  static bool isVertical(const int bankId);
  static bool areTubesInverselyNumbered(const int bankId);

  /// boron masks ///
  static boronMasks * masks;
  static double getBoronMaskPosition(const int bankId, const int maskId, const int axisIndex);
  static double getTriangularBoronMaskPosition(const int maskId, const int axisIndex);

private:
  double rearBankDistance;

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
};

#endif
