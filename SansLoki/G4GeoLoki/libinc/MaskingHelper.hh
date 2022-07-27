#ifndef G4GeoLoki_MaskingHelper_hh
#define G4GeoLoki_MaskingHelper_hh

#include "G4GeoLoki/PixelatedBanks.hh"

class MaskingHelper : public PixelatedBanks {
public:
  using PixelatedBanks::PixelatedBanks; //inherit constructors of PixelatedBanks 

  void calcPixelCentrePositionForMasking(const int pixelId, const bool isNewPixelNumbering);
  double getPixelCentrePosition(const int axisIndex) const;

private:
  static double pixelCentrePosition[3]; //used for masking

  static void coordinateRotation(double &x, double &y, const double angle);
  static int getBankId(const int pixelId);
  static int getPackId(const int bankId, const int tubeId, const bool isNewPixelNumbering);
  static int getInPackTubeId(const int bankId, const int tubeId, const bool isNewPixelNumbering);
  static int getTubeId(const int pixelId, const int bankId);
  static int getStrawId(const int pixelId, const int bankId, const int tubeId);
  static double getPixelPositionInStraw(const int pixelId, const int bankId);
};

#endif
