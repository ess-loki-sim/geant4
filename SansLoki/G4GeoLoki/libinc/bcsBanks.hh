#ifndef G4GeoLoki_bcsBanks_hh
#define G4GeoLoki_bcsBanks_hh

#include <array>
#include "G4GeoLoki/bcsTube.hh"
#include "G4GeoLoki/bcsPack.hh"
#include "G4GeoLoki/boronMasks.hh"
#include "G4GeoLoki/calibMasks.hh"

class bcsBanks {
public:
  bcsBanks(double rearBankDistance): rearBankDistance(rearBankDistance){}

  /// bcs tubes ///
  const static bcsTube* tubes;

  /// bcs packs ///
  const static bcsPack* packs;

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
  const static boronMasks * masks;
  static double getBoronMaskPosition(const int bankId, const int maskId, const int axisIndex);
  static double getTriangularBoronMaskPosition(const int maskId, const int axisIndex);

    /// calibration masks ///
  const static calibMasks* calibMasks;
  double getCalibMaskPosition(calibMasks::calibMasksBase calibMask,const int bankId, const int axisIndex) const;

private:
  double rearBankDistance;

  const static double packHolderDistanceFromPackTop;
  const static double packHolderDistanceFromPackFront;

  /// bank //
  const static double strawLengthInBank[9];
  const static int numberOfPacksInBank[9];

  const static double bankRotation[9][3];
  const static double bankPositionAngle[9];
  const static double bankTiltAngle[9];
  static double calcBankRotation(const int bankId);

  const static int bankPosDir[9]; //indicate direction along respective (X or Y) axis
  const static double bankPosition[9][3];
  const static double bankPositionOffset[9][3]; 
  const static double bankSize[9][3];
  const static double topmostPackHolderPositionInBankFromTopFront[9][2];
  static double packHolderToPackCentreCoordsInPack(const int axisIndex);

  const static double bankDistance[9];
  static double calcBankPositionZ(const int bankId);
  static double calcBankPositionXY(const int bankId);

  static double packHolderToFirstTubeCentreCoordsInPack(const int axisIndex);
  static double detectorSystemCentreDistanceFromBankTop(const int bankId);
  static double detectorSystemCentreOffsetInBank(const int bankId, const int axisIndex);
};

#endif
