#ifndef G4GeoLoki_bcsBanks_hh
#define G4GeoLoki_bcsBanks_hh

#include <string>
#include <array>

class bcsBanks {
public:
  bcsBanks(double locRearBankDistance){
    rearBankDistance = locRearBankDistance;
  }

  /// straw ///
  double getStrawOuterRadius() const;
  double getStrawInnerRadius() const;
  double getStrawWallThickness() const;
  double getFrontTubeConverterThickness() const;
  double getBackTubeConverterThickness() const;

  /// tube ///
  double getTubeOuterRadius() const;
  double getTubeInnerRadius() const;
  double getStrawPositionX(const int strawId) const;
  double getStrawPositionY(const int strawId) const;
  //std::string getTubeWallMaterial() const;

  /// tube grid - pack ///
  double getHorizontalTubeDistanceInPack() const;
  static double getVerticalTubeDistanceInPack();
  double getPackRotation() const;
  double getTubeRotation() const;

  double getTopRowOffsetInPack() const;
  static double getHorizontalTubeCentreOffsetInPack();
  static double getVerticalTubeCentreOffsetInPack();
  double getPackPackDistance() const;// TODO better name?

  double getPackBoxWidth() const;
  double getPackBoxHeight() const;
  double getPackBoxIdleLengthOnOneEnd() const;

  double getB4CPanelThickness() const;
  double getB4CLengthOverStrawOnOneEnd() const;
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

  static double getTopmostPackPositionInBank(const int bankId, const int axisIndex); // 0 - x, 1 - y, 2 - z

  /// bank ///
  double getStrawLengthByBankId(const int bankId) const;
  int getNumberOfPacksByBankId(const int bankId) const; 
  static int getNumberOfTubes(const int bankId); 

  double getBankRotation(const int bankId, const int axisIndex) const; // 0 - x, 1 - y, 2 - z
  double getBankPosition(const int bankId, const int axisIndex) const; // 0 - x, 1 - y, 2 - z
  static double getBankSize(const int bankId, const int axisIndex); // 0 - x, 1 - y, 2 - z

  static int getNumberOfBoronMasks(const int bankId);
  static double getBoronMaskSize(const int bankId, const int maskId, const int axisIndex);
  static double getBoronMaskPosition(const int bankId, const int maskId, const int axisIndex);
  static double getBoronMaskRotation(const int bankId, const int maskId);
  
  static double getTriangularBoronMaskParameter(const int maskId, const int parameterIndex);
  static double getTriangularBoronMaskPosition(const int maskId, const int axisIndex);

  static double detectorSystemFrontDistanceFromBankFront(const int bankId);

  static bool isVertical(const int bankId);
  static bool areTubesInverselyNumbered(const int bankId);

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

  /// straw ///
  static double strawOuterRadius;
  static double strawWallThickness; 
  static double frontTubeConverterThickness;
  static double backTubeConverterThickness;

  /// tube ///
  static double tubeOuterRadius;
  static double tubeWallThickness;
  static double strawPositionsInTube[7][2];
  static double strawStrawDistance;
  //static std::string tubeWallMaterial;
  

  /// tube grid - pack ///
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

  //boron masks

  static std::array<std::array<double, 7>, 6> rearBoronMasks;
  static std::array<std::array<double, 7>, 8> midVerticalBoronMasks;
  static std::array<std::array<double, 7>, 8> midHorizontalBoronMasks;
  static std::array<std::array<double, 7>, 8> frontTopBoronMasks;
  static std::array<std::array<double, 7>, 8> frontBottomBoronMasks; 
  static std::array<std::array<double, 7>, 8> frontVerticalBoronMasks;

  static std::array<std::array<double, 10>, 4> triangularBoronMasks;

  static double boronMaskParameter(const int bankId, const int maskId, const int parameterIndex);
  
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
