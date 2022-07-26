#include "G4GeoLoki/calibMasks.hh"
#include "Core/Units.hh"
#include <cmath>
#include <cassert>

// NOTE: All values are expressed in [mm] unit


//Planned calibration slit masks for LoKI, with 94 mm wide parts and 6 mm wide slits
const calibMasks::calibMasksBase mask0("lokiStandard-0", 0.3, 1600.0, -47.0, 
  {94.,6., 94.,6., 94.,6., 94.,6., 94.,6., 94.,6., 94.,6., 94.,6., 94.,6., 94.,6., 94.});
const calibMasks::calibMasksBase mask1("lokiStandard-1", 0.3, 500.0, -47.0, 
  {94.,6., 94.,6., 94.,6., 94.,6., 94.,6., 94.,6., 94.,6., 94.,6., 94.,6., 94.,6., 94.});
const calibMasks::calibMasksBase mask2("lokiStandard-2", 0.3, 500.0, -47.0, 
  {94.,6., 94.,6., 94.,6., 94.,6., 94.,6., 94.});
const calibMasks::calibMasksBase mask3("lokiStandard-3", 0.3, 500.0, -47.0,
  {94.,6., 94.,6., 94.,6., 94.,6., 94.,6., 94.,6., 94.,6., 94.,6., 94.,6., 94.,6., 94.});
const calibMasks::calibMasksBase mask4("lokiStandard-4", 0.3, 500.0, -47.0, 
  {94.,6., 94.,6., 94.,6., 94.,6., 94.,6., 94.});
const calibMasks::calibMasksBase mask5("lokiStandard-5", 0.3, 820.0, -47.0, 
  {94.,6., 94.,6., 94.,6., 94.,6., 94.,6., 94.,6., 94.,6., 94.,6., 94.,6., 94.,6., 94.,6., 94.,6., 94.});
const calibMasks::calibMasksBase mask6("lokiStandard-6", 0.3, 820.0, -47.0, 
  {94.,6., 94.,6., 94.,6., 94.,6., 94.,6., 94.,6., 94.,6., 94.,6., 94.,6., 94.,6., 94.,6., 94.,6., 94.});
const calibMasks::calibMasksBase mask7("lokiStandard-7", 0.3, 600.0, -47.0, 
  {94.,6., 94.,6., 94.,6., 94.,6., 94.,6., 94.,6., 94.,6., 94.,6., 94.,6., 94.,6., 94.,6., 94.,6., 94.});
const calibMasks::calibMasksBase mask8("lokiStandard-8", 0.3, 820.0, -47.0, 
  {94.,6., 94.,6., 94.,6., 94.,6., 94.,6., 94.,6., 94.,6., 94.,6., 94.,6., 94.,6., 94.,6., 94.,6., 94.});

// B4C sheet(cadmium in real life) with holes(slits) cut into it, used for calibration at the LoKI rear bank experiment at Larmor(ISIS)
// From the right 76 mm (gap) – 74 mm (Cd) - 3 mm (slit) - 103 (Cd) - 3 (slit) - 103 (Cd) - 3 (slit) - 103 (Cd) - 3 (slit) - 103 (Cd) - 3 (slit) - 100 (Cd) - 3 (slit) - 100 (Cd) - 3 (slit) - 100 (Cd) - 3 (slit) - 100 (Cd) - 3 (slit) - 63 (Cd)
// '76 mm (gap)' means the distance from the right end of the detectors tubes, which translates to -50 mm distance from the left end of the BCS tubes 
calibMasks::calibMasksBase maskLarmor("larmorCdCalibMask", 0.3, 800., -50., (75.-59.0188),
  {63., 3.,100.,3.,100.,3.,100.,3.,100.,3., 103.,3.,103.,3.,103.,3.,103.,3., 74.});

const std::map<std::string, calibMasks::calibMasksBase> calibMasks::masks { 
  {maskLarmor.name, maskLarmor}, 
  {mask0.name, mask0},
  {mask1.name, mask1},
  {mask2.name, mask2},
  {mask3.name, mask3},
  {mask4.name, mask4},
  {mask5.name, mask5},
  {mask6.name, mask5},
  {mask7.name, mask7},
  {mask8.name, mask8},
};


calibMasks::calibMasksBase calibMasks::getCalibMask(std::string name){
  return masks.find(name)->second;
}
double calibMasks::calibMasksBase::getWidth() const{
  return std::accumulate(this->pattern.begin(), this->pattern.end(), decltype(this->pattern)::value_type(0));
}

G4Material* calibMasks::maskMaterial = NamedMaterialProvider::getMaterial("ESS_B4C:b10_enrichment=0.95");
G4Material* calibMasks::maskBoxMaterial = NamedMaterialProvider::getMaterial("G4_Vacuum");