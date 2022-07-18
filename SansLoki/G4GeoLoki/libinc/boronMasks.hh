#ifndef G4GeoLoki_boronMasks_hh
#define G4GeoLoki_boronMasks_hh

#include <string>
#include <array>

class boronMasks {
public:
  /// regular rectangular masks ///
  static int getNumberOfBoronMasks(const int bankId);
  static double getSize(const int bankId, const int maskId, const int axisIndex);
  static double getPosition(const int bankId, const int maskId, const int axisIndex);
  static double getRotation(const int bankId, const int maskId);
  
  /// triangular masks ///
  static double getHalfSizeOfTriangularMask(const int maskId, const int axisIndex);
  static double getCutPointOfTriangularMask(const int maskId, const int axisIndex);
  static double getCutDirOfTriangularMask(const int maskId, const int axisIndex);
  static double getBankIdOfTriangularMask(const int maskId);
  static double getPosInBankOfTriangularMask(const int maskId, const int axisIndex);

private:
  /// regular rectangular masks ///
  static double getBoronMaskParameter(const int bankId, const int maskId, const int parameterIndex); 
  static std::array<std::array<double, 7>, 6> rearBoronMasks;
  static std::array<std::array<double, 7>, 8> midVerticalBoronMasks;
  static std::array<std::array<double, 7>, 8> midHorizontalBoronMasks;
  static std::array<std::array<double, 7>, 8> frontTopBoronMasks;
  static std::array<std::array<double, 7>, 8> frontBottomBoronMasks; 
  static std::array<std::array<double, 7>, 8> frontVerticalBoronMasks;

  /// triangular masks ///
  static double getTriangularBoronMaskParameter(const int maskId, const int parameterIndex);
  static std::array<std::array<double, 10>, 4> triangularBoronMasks;
};

#endif
