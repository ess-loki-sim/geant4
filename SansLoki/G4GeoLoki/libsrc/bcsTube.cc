#include "G4GeoLoki/bcsTube.hh"
#include "Core/Units.hh"
#include <cmath>
#include <cassert>

G4Material* bcsTube::tubeWallMaterial = NamedMaterialProvider::getMaterial("NCrystal:cfg=Al_sg225.ncmat"); 
G4Material* bcsTube::tubeInnerGas = NamedMaterialProvider::getMaterial("IdealGas:formula=0.8*Ar+0.2*CO2:pressure_atm=0.7"); 
G4Material* bcsTube::strawWallMaterial = NamedMaterialProvider::getMaterial("NCrystal:cfg=Cu_sg225.ncmat"); 
G4Material* bcsTube::converterMaterial = NamedMaterialProvider::getMaterial("ESS_B4C:b10_enrichment=0.95"); 
G4Material* bcsTube::countingGas = NamedMaterialProvider::getMaterial("IdealGas:formula=0.8*Ar+0.2*CO2:pressure_atm=0.7"); 

double bcsTube::strawOuterRadius = 3.75 *Units::mm;
double bcsTube::strawWallThickness = 0.0254 *Units::mm; //0.001 inch

double bcsTube::getStrawOuterRadius(){
  return strawOuterRadius;
}
double bcsTube::getStrawInnerRadius(){
  return strawOuterRadius - strawWallThickness;
}
double bcsTube::getStrawWallThickness(){
  return strawWallThickness;
}


double bcsTube::frontTubeConverterThickness = 0.65 *Units::um;
double bcsTube::backTubeConverterThickness = 1.00 *Units::um;

double bcsTube::getFrontTubeConverterThickness(){
  return frontTubeConverterThickness;
}
double bcsTube::getBackTubeConverterThickness(){
  return backTubeConverterThickness;
}


double bcsTube::tubeOuterRadius = 12.7 *Units::mm; // 1 inch diameter
double bcsTube::tubeWallThickness = 0.889 *Units::mm; //0.035 inch  //0.94 

double bcsTube::getTubeOuterRadius() { 
  return tubeOuterRadius; 
}
double bcsTube::getTubeInnerRadius() { 
  return tubeOuterRadius - tubeWallThickness; 
}


double bcsTube::strawStrawDistance = 7.75 *Units::mm; 
double bcsTube::strawPositionsInTube[7][2] = { //TODO reorder to change volume numbering
    { -0.5*strawStrawDistance * tan(M_PI/3.), -0.5*strawStrawDistance }, //20
    { 0, -strawStrawDistance }, //0
    { 0.5*strawStrawDistance * tan(M_PI/3.), -0.5*strawStrawDistance }, //10
    { 0, 0},  //30
    { -0.5*strawStrawDistance * tan(M_PI/3.), 0.5*strawStrawDistance }, //50
    { 0, strawStrawDistance}, //60
    { 0.5*strawStrawDistance * tan(M_PI/3.), 0.5*strawStrawDistance }, //40
  };

double bcsTube::getStrawPositionX(const int strawId) {
  assert(0 <= strawId && strawId <= 6);
  return strawPositionsInTube[strawId][0]; 
}
double bcsTube::getStrawPositionY(const int strawId) { 
  assert(0 <= strawId && strawId <= 6);
  return strawPositionsInTube[strawId][1]; 
}