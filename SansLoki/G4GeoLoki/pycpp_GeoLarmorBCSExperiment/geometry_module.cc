/////////////////////////////////////////
// Declaration of our geometry module: //
/////////////////////////////////////////

#include "G4Interfaces/GeoConstructPyExport.hh"
#include "G4Tubs.hh"
#include "G4Box.hh"
#include "G4Transform3D.hh"
#include "G4Vector3D.hh"
#include "G4SubtractionSolid.hh"
//#include "G4LogicalVolume.hh"  ///might not be needed? 
//#include <fstream>
#include <cmath>
#include <string> 

#include "G4GeoLoki/bcsBanks.hh"

class GeoBCS : public G4Interfaces::GeoConstructBase
{
public:
  GeoBCS();
  virtual ~GeoBCS(){}
  virtual G4VPhysicalVolume* Construct();
  
  bcsBanks * banks;
protected:
  virtual bool validateParameters();
private:
  //Functions
  G4LogicalVolume * createTubeLV(double converter_thickness, double straw_length);  // create the logical volume of a tube for easy placement in a panel
  G4LogicalVolume * createPackBoxLV(double strawLength, int packNumber, int numberOfPacksForInvertedNumbering);
  G4LogicalVolume * createBankLV(int bankId);

  int getTubeNumber(double rowNumber, int localIndex, int numberOfPacksForInvertedNumbering);
};

// this line is necessary to be able to declare the geometry in the python simulation script 
PYTHON_MODULE { GeoConstructPyExport::exportGeo<GeoBCS>("GeoLarmorBCSExperiment"); } 

////////////////////////////////////////////
// Implementation of our geometry module: //
////////////////////////////////////////////


//#include "G4Box.hh"

GeoBCS::GeoBCS()
  : GeoConstructBase("G4GeoLoki/GeoLarmorBCSExperiment"){
  // declare all parameters that can be used from the command line,
  // define their type, pick a self-explanatory name and _unit
  // give the default value, the last 2 ones are optional (min, max)

  addParameterDouble("rear_detector_distance_m", 4.420, 3.0, 10.0);
  addParameterBoolean("withBeamstop", false);
  addParameterDouble("generator_detector_distance_cm", 400, 0, 1000); //
  
  addParameterInt("number_of_pixels", 1); 
  
  addParameterString("world_material","G4_Vacuum");
  addParameterString("pack_box_fill_material", "G4_Vacuum");
  addParameterString("B4C_panel_material","ESS_B4C:b10_enrichment=0.95"); //TODO  B4C S-DOUGH IS AN 18% EPOXY RESIN, 82% BORON CARBIDE MIX.IT IS MANUFACTURED BY STFC's ADVANCED MATERIALS GROUP.
  addParameterString("converter_material","ESS_B4C:b10_enrichment=0.95");
  addParameterString("counting_gas","IdealGas:formula=0.8*Ar+0.2*CO2:pressure_atm=0.7");
  addParameterString("tube_inner_gas","IdealGas:formula=0.8*Ar+0.2*CO2:pressure_atm=0.7");    /// corrected to 80:20 Ar:CO2 ratio (04/06/2019)
  addParameterString("tube_wall_material", "NCrystal:cfg=Al_sg225.ncmat");
  addParameterString("straw_wall_material", "NCrystal:cfg=Cu_sg225.ncmat");
}

G4LogicalVolume * GeoBCS::createTubeLV(double converterThickness, double strawLength){
  const double tubeOuterRadius = banks->tubes->getTubeOuterRadius();
  const double tubeInnerRadius = banks->tubes->getTubeInnerRadius();

  const double strawOuterRadius = banks->tubes->getStrawOuterRadius();
  const double strawInnerRadius = banks->tubes->getStrawInnerRadius();
  const double strawWallThickness = banks->tubes->getStrawWallThickness();
  const double effectiveStrawLength = strawLength - strawWallThickness; //This is only epsilon difference...

  auto tubeWallMaterial = getParameterMaterial("tube_wall_material");
  auto tubeInnerGas = getParameterMaterial("tube_inner_gas");
  auto strawWallMaterial = getParameterMaterial("straw_wall_material");
  auto counting_gas = getParameterMaterial("counting_gas");
  auto converterMaterial = getParameterMaterial("converter_material");

  const int pixelNumber = getParameterInt("number_of_pixels");
  const double pixelLength = effectiveStrawLength / pixelNumber;

  auto lv_tube = new G4LogicalVolume(new G4Tubs("TubeWall",0, tubeOuterRadius, 0.5*strawLength, 0., 2*M_PI),
                                     tubeWallMaterial, "TubeWall");

  auto lv_empty_tube = place(new G4Tubs("EmptyTube", 0., tubeInnerRadius, 0.5*strawLength, 0., 2*M_PI),
                                           tubeInnerGas, 0,0,0, lv_tube, G4Colour(0,1,1),-2,0,0).logvol;
  
  for (int cpNo = 0; cpNo <= 6; cpNo++){
    auto lv_straw_wall = place(new G4Tubs("StrawWall", 0, strawOuterRadius, 0.5 * strawLength, 0., 2 * M_PI),
                               strawWallMaterial, banks->tubes->getStrawPositionX(cpNo), banks->tubes->getStrawPositionY(cpNo), 0, lv_empty_tube, ORANGE, cpNo, 0, 0).logvol;

    auto lv_converter = place(new G4Tubs("Converter", 0., strawInnerRadius, 0.5*effectiveStrawLength, 0., 2 * M_PI),
                              converterMaterial, 0, 0, 0, lv_straw_wall, G4Colour(0, 1, 1), cpNo + 100, 0, 0).logvol;

    for (int i = 0; i < pixelNumber; i++){
      place(new G4Tubs("CountingGas", 0., strawInnerRadius - converterThickness, 0.5 * pixelLength, 0., 2 * M_PI),
            counting_gas, 0, 0, -(i * pixelLength + 0.5 * pixelLength - 0.5*effectiveStrawLength), lv_converter, G4Colour(0, 0, 1), i, 0, 0);
    }
  }
  return lv_tube;
}

int GeoBCS::getTubeNumber(double rowNumber, int localIndex, int numberOfPacksForInvertedNumbering){
  if(numberOfPacksForInvertedNumbering == 0){
    return rowNumber * 8 + localIndex;
  }
  else{
    return (numberOfPacksForInvertedNumbering*8 - 4) - rowNumber*8 + localIndex;
  }
}

///////////  CREATE PACK BOX LOGICAL VOLUME  //////////////////////////

G4LogicalVolume *GeoBCS::createPackBoxLV(double strawLength, int packNumber, int numberOfPacksForInvertedNumbering){
  auto pack_fill_material = getParameterMaterial("pack_box_fill_material");
  auto B4C_panel_material = getParameterMaterial("B4C_panel_material");
  auto Al_panel_material = getParameterMaterial("tube_wall_material"); //TODO check if the matterial is the same

  const double packBoxHeightHalf = 0.5 * banks->getPackBoxHeight();
  const double horizontalTubeDistance = banks->getHorizontalTubeDistanceInPack();
  const double idleLengthOnOneEnd = banks->getPackBoxIdleLengthOnOneEnd();

  const double horizontalOffset = banks->getHorizontalTubeCentreOffsetInPack();
  const double verticalOffset = banks->getVerticalTubeCentreOffsetInPack();

  G4RotationMatrix* tubeRotationMatrix = new G4RotationMatrix(0, 0, banks->getTubeRotation());

  const double topRowOffset = banks->getTopRowOffsetInPack(); /// offset by 28.4sin(13.45)

    auto lv_pack_box = new G4LogicalVolume(new G4Box("EmptyPackBox", 0.5*banks->getPackBoxWidth(), packBoxHeightHalf, 0.5 * strawLength + idleLengthOnOneEnd), pack_fill_material, "EmptyPackBox");

  double frontB4CThickness = 0.65 *Units::um;
  double backB4CThickness = packNumber != 3 ? 1.00 *Units::um : 0.65 *Units::um;

  auto lv_front_tube = createTubeLV(frontB4CThickness, strawLength); 
  auto lv_back_tube = createTubeLV(backB4CThickness, strawLength); 

  place(lv_front_tube, horizontalOffset + topRowOffset, verticalOffset, 0, lv_pack_box, SILVER, getTubeNumber(packNumber, 0,numberOfPacksForInvertedNumbering), 0, tubeRotationMatrix);
  place(lv_front_tube, horizontalOffset + topRowOffset + horizontalTubeDistance, verticalOffset, 0, lv_pack_box, SILVER, getTubeNumber(packNumber, 1,numberOfPacksForInvertedNumbering), 0, tubeRotationMatrix);
  place(lv_back_tube, horizontalOffset + topRowOffset + 2.0 * horizontalTubeDistance, verticalOffset, 0, lv_pack_box, SILVER, getTubeNumber(packNumber, 2, numberOfPacksForInvertedNumbering), 0, tubeRotationMatrix);
  place(lv_back_tube, horizontalOffset + topRowOffset + 3.0 * horizontalTubeDistance, verticalOffset, 0, lv_pack_box, SILVER, getTubeNumber(packNumber, 3, numberOfPacksForInvertedNumbering), 0, tubeRotationMatrix); // 1st trans is z, 2nd trans is y, 3rd trans is x

  place(lv_front_tube, horizontalOffset, -verticalOffset, 0, lv_pack_box, SILVER, getTubeNumber(packNumber+0.5, 0, numberOfPacksForInvertedNumbering), 0, tubeRotationMatrix);
  place(lv_front_tube, horizontalOffset + horizontalTubeDistance, -verticalOffset, 0, lv_pack_box, SILVER, getTubeNumber(packNumber+0.5, 1,numberOfPacksForInvertedNumbering), 0, tubeRotationMatrix);
  place(lv_back_tube, horizontalOffset + 2.0 * horizontalTubeDistance, -verticalOffset, 0, lv_pack_box, SILVER, getTubeNumber(packNumber+0.5, 2,numberOfPacksForInvertedNumbering), 0, tubeRotationMatrix);
  place(lv_back_tube, horizontalOffset + 3.0 * horizontalTubeDistance, -verticalOffset, 0, lv_pack_box, SILVER, getTubeNumber(packNumber+0.5, 3, numberOfPacksForInvertedNumbering), 0, tubeRotationMatrix);


  /// Add B4C panel in 3 parts //
  const double B4CLengthHalf = 0.5*strawLength + banks->getB4CLengthOverStrawOnOneEnd();
  const double B4CMainThicknessHalf = 0.5*banks->getB4CPanelThickness();
  const double B4CMainPartHeightHalf = 0.5*banks->getB4CMainPartHeight();
  const double B4CMainPartHorizontalOffset = banks->getB4CMainPartHorizontalOffset();
  const double B4CMainPartVerticalOffset = banks->getB4CMainPartVerticalOffset();

  const double B4CMiddlePartThicknessHalf = 0.5*banks->getB4CMiddlePartThickness();
  const double B4CMiddlePartHeightHalf = 0.5*banks->getB4CMiddlePartHeight();
  const double B4CMiddlePartHorizontalOffset = banks->getB4CMiddlePartHorizontalOffset();
  const double B4CMiddlePartVerticalOffset = banks->getB4CMiddlePartVerticalOffset();

  const double B4CBottomPartThicknessHalf = 0.5*banks->getB4CBottomPartThickness();
  const double B4CBottomPartHeightHalf = 0.5*banks->getB4CBottomPartHeight();
  const double B4CBottomPartHorizontalOffset = banks->getB4CBottomPartHorizontalOffset();
  const double B4CBottomPartVerticalOffset = banks->getB4CBottomPartVerticalOffset();

  auto lv_B4C_main_part = new G4LogicalVolume(new G4Box("B4CPanel", B4CMainThicknessHalf, B4CMainPartHeightHalf, B4CLengthHalf), B4C_panel_material, "B4CPanel");
  auto lv_B4C_middle_part = new G4LogicalVolume(new G4Box("B4CPanel", B4CMiddlePartThicknessHalf, B4CMiddlePartHeightHalf, B4CLengthHalf), B4C_panel_material, "B4CPanel");
  auto lv_B4C_bottom_part = new G4LogicalVolume(new G4Box("B4CPanel", B4CBottomPartThicknessHalf, B4CBottomPartHeightHalf, B4CLengthHalf), B4C_panel_material, "B4CPanel");

  place(lv_B4C_main_part, B4CMainPartHorizontalOffset, B4CMainPartVerticalOffset, 0, lv_pack_box, G4Colour(0, 1, 0), -2, 0, new G4RotationMatrix());
  place(lv_B4C_middle_part, B4CMiddlePartHorizontalOffset, B4CMiddlePartVerticalOffset, 0, lv_pack_box, G4Colour(0, 1, 0), -2, 0, new G4RotationMatrix());
  place(lv_B4C_bottom_part, B4CBottomPartHorizontalOffset, B4CBottomPartVerticalOffset, 0, lv_pack_box, G4Colour(0, 1, 0), -2, 0, new G4RotationMatrix());

  // Add Al behing the B4C panel //This part should be cleaned up
  const double AlMainThicknessHalf = B4CBottomPartHeightHalf;
  const double AlMainHeightHalf = B4CMainPartHeightHalf + B4CBottomPartHeightHalf;
  const double AlMainPartHorizontalOffset = B4CMainPartHorizontalOffset + B4CMainThicknessHalf + AlMainThicknessHalf;
  const double AlBottomThicknessHalf = B4CMiddlePartThicknessHalf - B4CBottomPartThicknessHalf + B4CMainThicknessHalf;
  const double AlBottomPartHorizontalOffset = AlMainPartHorizontalOffset - AlMainThicknessHalf - AlBottomThicknessHalf;

  auto lv_Al_main_part = new G4LogicalVolume(new G4Box("AlPanel", AlMainThicknessHalf, AlMainHeightHalf, B4CLengthHalf), Al_panel_material, "AlPanel");
  auto lv_Al_bottom_part = new G4LogicalVolume(new G4Box("AlPanel", AlBottomThicknessHalf, B4CBottomPartHeightHalf, B4CLengthHalf), Al_panel_material, "AlPanel");
  
  place(lv_Al_main_part, AlMainPartHorizontalOffset, 0, 0, lv_pack_box, SILVER, -2, 0, new G4RotationMatrix());
  place(lv_Al_bottom_part, AlBottomPartHorizontalOffset, B4CBottomPartVerticalOffset, 0, lv_pack_box, SILVER, -2, 0, new G4RotationMatrix());

  return lv_pack_box;
}

///////////  CREATE DETECTOR BANK LOGICAL VOLUME  //////////////////////////

G4LogicalVolume *GeoBCS::createBankLV(int bankId){
  //auto B4C_panel_material = getParameterMaterial("B4C_panel_material");

  const double pack_pack_distance = banks->getPackPackDistance();
  const int numberOfPacks = 4; //banks->getNumberOfPacksByBankId(bankId); 

  auto pack_fill_material = getParameterMaterial("pack_box_fill_material");
  
  const double packRotation = banks->getPackRotation();

  const double bankSizeXHalf = 0.5* banks->getBankSize(bankId, 0) * 1.5;
  const double bankSizeYHalf = 0.5* banks->getBankSize(bankId, 1) * 0.3;
  const double bankSizeZHalf = 0.5* banks->getBankSize(bankId, 2);

  auto lv_bank = new G4LogicalVolume(new G4Box("EmptyPanelBox", bankSizeZHalf, bankSizeYHalf, bankSizeXHalf),
                                       pack_fill_material, "Bank");

  const int numberOfPacksForInvertedNumbering = 0;

  const double bankPositionZ = banks->getTopmostPackPositionInBank(bankId, 2);
  const double topmostBankPositionY = 2 * pack_pack_distance + 45.00 *Units::mm;
  for (int packNumber = 0; packNumber < numberOfPacks; ++packNumber){
    double strawLength = packNumber < 2 ? 1500.0 * Units::mm : 1200.0 * Units::mm;
    auto lv_pack_box = createPackBoxLV(strawLength, packNumber, numberOfPacksForInvertedNumbering);         
    place(lv_pack_box, bankPositionZ, topmostBankPositionY - packNumber * pack_pack_distance, 0, lv_bank, G4Colour(0, 1, 1), -2, 0, new G4RotationMatrix(0, 0, packRotation));
  }

  // Add BeamStop to the Rear Bank volume
  const bool withBeamstop = getParameterBoolean("withBeamstop");
  if ( withBeamstop) {
    const std::string maskName = "BoronMask-Beamstop";
    auto B4C_panel_material = getParameterMaterial("B4C_panel_material");
    const double detBankFrontDistance = banks->detectorSystemFrontDistanceFromBankFront(bankId);
    auto emptyRotation = new G4RotationMatrix();
    
    place(new G4Box(maskName, 0.5 * 1*Units::mm, 0.5 * 5*Units::cm, 0.5 * 5*Units::cm),
          B4C_panel_material,
          -bankSizeZHalf + detBankFrontDistance - 5*Units::cm, -banks->getBankPosition(bankId, 1), 40*Units::mm,
          lv_bank, BLACK, -5, 0, emptyRotation);
  }

  return lv_bank;
 }




G4VPhysicalVolume* GeoBCS::Construct(){
  // this is where we put the entire geometry together, the private functions creating the logical volumes are meant to facilitate the code below  

  auto world_material = getParameterMaterial("world_material"); 
  const double generator_detector_distance = getParameterDouble("generator_detector_distance_cm")*Units::cm;
  const double sdd = getParameterDouble("rear_detector_distance_m")*Units::m;

  banks = new bcsBanks(sdd);

  // calculate a value that is big enough to fit your world volume, the "super mother"
  double big_dimension = 1.1*( 1 *Units::m + generator_detector_distance + sdd);

  //World volume:
  auto worldvols = place(new G4Box("World", big_dimension, big_dimension, big_dimension), world_material, 0, 0, 0, 0, INVISIBLE);
  auto lvWorld = worldvols.logvol;
  auto pvWorld = worldvols.physvol;

  //for (int bankId = 0; bankId < 9; bankId++){
  const int bankId = 0;
  auto lv_bank = createBankLV(bankId);

  auto rotation = new G4RotationMatrix();
  rotation->rotateY(banks->getBankRotation(bankId, 1));
  rotation->rotateX(banks->getBankRotation(bankId, 0));
  rotation->rotateZ(banks->getBankRotation(bankId, 2));

  place(lv_bank, 0.0 /*banks->getBankPosition(bankId, 0)*/, banks->getBankPosition(bankId, 1), banks->getBankPosition(bankId, 2), lvWorld, ORANGE, bankId, 0, rotation);
  //}

  delete banks;
  return pvWorld;
}


bool GeoBCS::validateParameters() {
// you can apply conditions to control the sanity of the geometry parameters and warn the user of possible mistakes
  // a nice example: Projects/SingleCell/G4GeoSingleCell/libsrc/GeoB10SingleCell.cc
    return true;  
}

