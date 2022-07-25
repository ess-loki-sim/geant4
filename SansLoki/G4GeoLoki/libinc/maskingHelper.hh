#ifndef G4GeoLoki_maskingHelper_hh
#define G4GeoLoki_maskingHelper_hh

#include "G4GeoLoki/pixelatedBanks.hh"

class maskingHelper : public pixelatedBanks {
public:
  using pixelatedBanks::pixelatedBanks; //inherit constructors of pixelatedBanks 

  void calcPixelCentrePositionForMasking(const int pixelId);
  double getPixelCentrePosition(const int axisIndex);

private:
  static double pixelCentrePosition[3]; //used for masking

  static void coordinateRotation(double &x, double &y, const double angle);
  static int getBankId(const int pixelId);
  static int getTubeId(const int pixelId, const int bankId);
  static int getStrawId(const int pixelId, const int bankId, const int tubeId);
};

#endif
