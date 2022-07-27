#ifndef G4GeoLoki_PixelatedBanks_hh
#define G4GeoLoki_PixelatedBanks_hh

#include "G4GeoLoki/BcsBanks.hh"

class PixelatedBanks : public BcsBanks{
public:
  PixelatedBanks(double rearBankDistance): BcsBanks(rearBankDistance){} //use default rearBankPixelNumber
  PixelatedBanks(double rearBankDistance, int rearBankPixelNumber): BcsBanks(rearBankDistance){
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
