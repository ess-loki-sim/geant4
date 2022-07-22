#ifndef G4GeoLoki_pixelatedBanks_hh
#define G4GeoLoki_pixelatedBanks_hh

#include "G4GeoLoki/bcsBanks.hh"
#include <string>
#include <array>

class pixelatedBanks : public bcsBanks{
public:
  pixelatedBanks(double rearBankDistance): bcsBanks(rearBankDistance){} //use default rearBankPixelNumber
  pixelatedBanks(double rearBankDistance, int rearBankPixelNumber): bcsBanks(rearBankDistance){
    numberOfPixelsInStraw[0] = rearBankPixelNumber;
  }

  static int getTotalNumberOfPixels();
  static int getNumberOfPixels(const int bankId);
  int getPixelId(const int bankId, const int tubeId, const int strawId, const double positionX, const double positionY) const;

  static int getNumberOfPixelsInStraw(const int bankId);

protected:  
  static int getBankPixelOffset(const int bankId); 

private:
  static int numberOfPixelsInStraw[9];
  int getPositionPixelId(const int bankId, const double positionX, const double positionY) const;
};

#endif
