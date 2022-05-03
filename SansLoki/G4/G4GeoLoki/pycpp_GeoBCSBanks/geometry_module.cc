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
  G4LogicalVolume * createPackBoxLV(double strawLength, int packNumber, int numberOfPacksForInvertedNumbering, int numberOfPacks);
  G4LogicalVolume * createBankLV(int bankId);
  G4LogicalVolume * createTriangularMaskLV(int maskId);
  G4LogicalVolume * createCalibrationSlitMaskLV();


  int getTubeNumber(double rowNumber, int localIndex, int numberOfPacksForInvertedNumbering, int numberOfPacks);
};

// this line is necessary to be able to declare the geometry in the python simulation script 
PYTHON_MODULE { GeoConstructPyExport::exportGeo<GeoBCS>("GeoBCSBanks"); } 

////////////////////////////////////////////
// Implementation of our geometry module: //
////////////////////////////////////////////


//#include "G4Box.hh"

GeoBCS::GeoBCS()
  : GeoConstructBase("G4GeoLoki/GeoBCSBanks"){
  // declare all parameters that can be used from the command line,
  // define their type, pick a self-explanatory name and _unit
  // give the default value, the last 2 ones are optional (min, max)

  addParameterDouble("rear_detector_distance_m", 5.0, 4.0, 10.0);
  addParameterBoolean("rear_detector_only", false);
  addParameterBoolean("with_beamstop", false);
  addParameterBoolean("larmor_rear_bank_experiment", false);
  addParameterBoolean("with_calibration_slits", false);
  
  addParameterBoolean("old_pixel_numbering", false);

  addParameterInt("number_of_pixels", 1); //physically segmentig the gas volume. Not used currently
  
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
  const double tubeOuterRadius = banks->getTubeOuterRadius();
  const double tubeInnerRadius = banks->getTubeInnerRadius();

  const double strawOuterRadius = banks->getStrawOuterRadius();
  const double strawInnerRadius = banks->getStrawInnerRadius();
  const double strawWallThickness = banks->getStrawWallThickness();
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
                               strawWallMaterial, banks->getStrawPositionX(cpNo), banks->getStrawPositionY(cpNo), 0, lv_empty_tube, ORANGE, cpNo, 0, 0).logvol;

    auto lv_converter = place(new G4Tubs("Converter", 0., strawInnerRadius, 0.5*effectiveStrawLength, 0., 2 * M_PI),
                              converterMaterial, 0, 0, 0, lv_straw_wall, G4Colour(0, 1, 1), cpNo + 100, 0, 0).logvol;

    for (int i = 0; i < pixelNumber; i++){
      place(new G4Tubs("CountingGas", 0., strawInnerRadius - converterThickness, 0.5 * pixelLength, 0., 2 * M_PI),
            counting_gas, 0, 0, -(i * pixelLength + 0.5 * pixelLength - 0.5*effectiveStrawLength), lv_converter, G4Colour(0, 0, 1), i, 0, 0);
    }
  }
  return lv_tube;
}

int GeoBCS::getTubeNumber(double rowNumber, int localIndex, int numberOfPacksForInvertedNumbering, int numberOfPacks){
  const bool oldPixelNumbering = getParameterBoolean("old_pixel_numbering");
  if(oldPixelNumbering){
    if(numberOfPacksForInvertedNumbering == 0){
      return rowNumber * 8 + localIndex;
    }
    else{
      return (numberOfPacksForInvertedNumbering*8 - 4) - rowNumber*8 + localIndex;
    }
  }
  else{ // new numbering
    const int layerNr = localIndex; //[0-3]
    if(numberOfPacksForInvertedNumbering == 0){
      return rowNumber * 2 + layerNr * numberOfPacks * 2;
    }
    else{ 
      return (numberOfPacks*2 - 1) - rowNumber*2 + layerNr * numberOfPacks * 2;
    }

  }
}

///////////  CREATE PACK BOX LOGICAL VOLUME  //////////////////////////

G4LogicalVolume *GeoBCS::createPackBoxLV(double strawLength, int packNumber, int numberOfPacksForInvertedNumbering, int numberOfPacks){
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

  auto lv_front_tube = createTubeLV(banks->getFrontTubeConverterThickness(), strawLength); 
  auto lv_back_tube = createTubeLV(banks->getBackTubeConverterThickness(), strawLength); 

  place(lv_front_tube, horizontalOffset + topRowOffset, verticalOffset, 0, lv_pack_box, SILVER, getTubeNumber(packNumber, 0,numberOfPacksForInvertedNumbering, numberOfPacks), 0, tubeRotationMatrix);
  place(lv_front_tube, horizontalOffset + topRowOffset + horizontalTubeDistance, verticalOffset, 0, lv_pack_box, SILVER, getTubeNumber(packNumber, 1,numberOfPacksForInvertedNumbering, numberOfPacks), 0, tubeRotationMatrix);
  place(lv_back_tube, horizontalOffset + topRowOffset + 2.0 * horizontalTubeDistance, verticalOffset, 0, lv_pack_box, SILVER, getTubeNumber(packNumber, 2, numberOfPacksForInvertedNumbering, numberOfPacks), 0, tubeRotationMatrix);
  place(lv_back_tube, horizontalOffset + topRowOffset + 3.0 * horizontalTubeDistance, verticalOffset, 0, lv_pack_box, SILVER, getTubeNumber(packNumber, 3, numberOfPacksForInvertedNumbering, numberOfPacks), 0, tubeRotationMatrix); // 1st trans is z, 2nd trans is y, 3rd trans is x

  place(lv_front_tube, horizontalOffset, -verticalOffset, 0, lv_pack_box, SILVER, getTubeNumber(packNumber+0.5, 0, numberOfPacksForInvertedNumbering, numberOfPacks), 0, tubeRotationMatrix);
  place(lv_front_tube, horizontalOffset + horizontalTubeDistance, -verticalOffset, 0, lv_pack_box, SILVER, getTubeNumber(packNumber+0.5, 1,numberOfPacksForInvertedNumbering, numberOfPacks), 0, tubeRotationMatrix);
  place(lv_back_tube, horizontalOffset + 2.0 * horizontalTubeDistance, -verticalOffset, 0, lv_pack_box, SILVER, getTubeNumber(packNumber+0.5, 2,numberOfPacksForInvertedNumbering, numberOfPacks), 0, tubeRotationMatrix);
  place(lv_back_tube, horizontalOffset + 3.0 * horizontalTubeDistance, -verticalOffset, 0, lv_pack_box, SILVER, getTubeNumber(packNumber+0.5, 3, numberOfPacksForInvertedNumbering, numberOfPacks), 0, tubeRotationMatrix);

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

///////////  CREATE CALIBRATION SLIT LOGICAL VOLUME  //////////////////////////
// B4C sheet(cadmium in real life) with holes(slits) cut into it, used for calibration at the LoKI rear bank experiment at Larmor(ISIS), with hardcoded parameters
G4LogicalVolume *GeoBCS::createCalibrationSlitMaskLV(){
  const std::string maskName = "BoronMask-CalibrationSlits";
  auto B4C_panel_material = getParameterMaterial("B4C_panel_material");

  const double slitWidth = 3*Units::mm; // 3 mm wide slits
  const double slitWidthHalf =  0.5 * slitWidth; // 3 mm wide slits
  const double maskThicknessHalf = 0.5 * 0.3*Units::mm; // The Gd is probably 300um thick coated on a 3mm Al. Al neglected
  const double cutThicknessHalf = 1.5 * maskThicknessHalf; // A bit thicker, to be safe when substracting
  const double maskWidthHalf = 0.5 * 976*Units::mm; // 63+100*4+103*4+74 + (3*9) = 976
  const double maskHeightHalf = 0.5 * 800*Units::mm; // larger than reality
  const double slitHeightHalf = 1.5 * maskHeightHalf; // A bit larger, to be safe when substracting

  auto fullBox = new G4Box("EmptyFullBox", maskThicknessHalf, maskHeightHalf, maskWidthHalf);
  auto slitCut = new G4Box("EmptySlitBox", cutThicknessHalf, slitHeightHalf, slitWidthHalf);

  const double slitPositions[9]  = { //counting from the right side
    maskWidthHalf - 74*Units::mm - slitWidthHalf,
    maskWidthHalf - (74 + 1*103)*Units::mm - 1*slitWidth - slitWidthHalf,
    maskWidthHalf - (74 + 2*103)*Units::mm - 2*slitWidth - slitWidthHalf,
    maskWidthHalf - (74 + 3*103)*Units::mm - 3*slitWidth - slitWidthHalf,
    maskWidthHalf - (74 + 4*103)*Units::mm - 4*slitWidth - slitWidthHalf,
    maskWidthHalf - (74 + 4*103 + 1*100)*Units::mm - 5*slitWidth - slitWidthHalf,
    maskWidthHalf - (74 + 4*103 + 2*100)*Units::mm - 6*slitWidth - slitWidthHalf,
    maskWidthHalf - (74 + 4*103 + 3*100)*Units::mm - 7*slitWidth - slitWidthHalf,
    maskWidthHalf - (74 + 4*103 + 4*100)*Units::mm - 8*slitWidth - slitWidthHalf,
  };

  auto slitBox = new G4SubtractionSolid("EmptyCalSlitBox", fullBox, slitCut, 0, G4ThreeVector(0,0,slitPositions[0]));
  for(int i=1; i<=8; i++ ){
    slitBox = new G4SubtractionSolid("EmptyCalSlitBox", slitBox, slitCut, 0, G4ThreeVector(0,0,slitPositions[i]));
  }

  auto lv_calibrationSlits = new G4LogicalVolume(slitBox, B4C_panel_material, maskName);

  return lv_calibrationSlits;
}


///////////  CREATE DETECTOR BANK LOGICAL VOLUME  //////////////////////////

G4LogicalVolume *GeoBCS::createBankLV(int bankId){
  auto B4C_panel_material = getParameterMaterial("B4C_panel_material");
  const double strawLength = banks->getStrawLengthByBankId(bankId);

  const double pack_pack_distance = banks->getPackPackDistance();
  const int numberOfPacks = banks->getNumberOfPacksByBankId(bankId); 

  auto pack_fill_material = getParameterMaterial("pack_box_fill_material");
  
  const double packRotation = banks->getPackRotation();

  const double bankSizeXHalf = 0.5* banks->getBankSize(bankId, 0);
  const double bankSizeYHalf = 0.5* banks->getBankSize(bankId, 1);
  const double bankSizeZHalf = 0.5* banks->getBankSize(bankId, 2);

  auto lv_bank = new G4LogicalVolume(new G4Box("EmptyPanelBox", bankSizeZHalf, bankSizeYHalf, bankSizeXHalf),
                                       pack_fill_material, "Bank");

  // override lv_bank to subtract some empty part of front left and right banks where front top and bottom banks would overlap with them
  if (bankId == 8 || bankId == 6) {  
    auto fullBankBox = new G4Box("EmptyPanelBox", bankSizeZHalf, bankSizeYHalf, bankSizeXHalf);
    auto bankBoxCut = new G4Box("EmptyPanelBox", 15.0, 70.0, 60.0);
    auto bankBox = new G4SubtractionSolid("EmptyPanelBox", fullBankBox, bankBoxCut, 0, G4ThreeVector(-145.0, -42.0, -425.0));
    
    lv_bank = new G4LogicalVolume(bankBox, pack_fill_material, "Bank");
  }

  int numberOfPacksForInvertedNumbering = 0; 
  if (banks->areTubesInverselyNumbered(bankId)){ //not very nice solution...
    numberOfPacksForInvertedNumbering = numberOfPacks;
  }

  for (int packNumber = 0; packNumber < numberOfPacks; ++packNumber){
    auto lv_pack_box = createPackBoxLV(strawLength, packNumber, numberOfPacksForInvertedNumbering, numberOfPacks);         
    place(lv_pack_box, banks->getTopmostPackPositionInBank(bankId, 2), -(packNumber * pack_pack_distance - banks->getTopmostPackPositionInBank(bankId, 1)), banks->getTopmostPackPositionInBank(bankId, 0), lv_bank, G4Colour(0, 1, 1), -2, 0, new G4RotationMatrix(0, 0, packRotation));
  }

  const int numberOfBoronMasks = banks->getNumberOfBoronMasks(bankId);
  for (int maskId = 0; maskId < numberOfBoronMasks; ++maskId){
    const std::string maskName = "BoronMask-"+std::to_string(bankId)+"-"+std::to_string(maskId);
    place(new G4Box(maskName, 0.5*banks->getBoronMaskSize(bankId, maskId, 2), 0.5*banks->getBoronMaskSize(bankId, maskId, 1), 0.5*banks->getBoronMaskSize(bankId, maskId, 0)),
            B4C_panel_material, 
            banks->getBoronMaskPosition(bankId, maskId, 2), banks->getBoronMaskPosition(bankId, maskId, 1), banks->getBoronMaskPosition(bankId, maskId, 0), 
            lv_bank, BLACK, -2, 0, new G4RotationMatrix(0, 0, banks->getBoronMaskRotation(bankId, maskId)));
  }

  // Add BeamStop to the Rear Bank volume
  const bool withBeamstop = getParameterBoolean("with_beamstop");
  if (bankId == 0 && withBeamstop) {
    const std::string maskName = "BoronMask-Beamstop";
    const double detBankFrontDistance = banks->detectorSystemFrontDistanceFromBankFront(bankId);
    
    place(new G4Box(maskName, 0.5 * 1*Units::mm, 0.5 * 5*Units::cm, 0.5 * 5*Units::cm),
          B4C_panel_material,
          -bankSizeZHalf + detBankFrontDistance - 5*Units::cm, 0, 0,
          lv_bank, BLACK, -5, 0, new G4RotationMatrix());
  }

/*
  // Add Calibration slit system the Rear Bank volume for Larmor rear bank experiment calibration
  const bool withCalibrationSlits = getParameterBoolean("with_calibration_slits");
  if (bankId == 0 && withCalibrationSlits) {
    const double detBankFrontDistance = banks->detectorSystemFrontDistanceFromBankFront(bankId);
    
    const double verticalBankPosition = 0.5 * banks->getBankSize(bankId, 1) - 1155 *Units::mm + (4+33) *Units::mm; // beam centre at 1155 mm above floor, including the 4 mm electrical isolation layer, that is 33 mm above the Larmor floor (due to the weels)
    const double verticalPosition = - verticalBankPosition;

    auto lv_calibrationSlits = createCalibrationSlitMaskLV();
    place(lv_calibrationSlits,
          -bankSizeZHalf + detBankFrontDistance - 55*Units::mm, verticalPosition, 0, // 75mm in front of the first layer of tubes
          lv_bank, BLACK, -5, 0, new G4RotationMatrix());
  }*/

  return lv_bank;
 }

G4LogicalVolume *GeoBCS::createTriangularMaskLV(int maskId){
    //creating special triangular volume by subtraction of 2 boxes
    const double xHalf = banks->getTriangularBoronMaskParameter(maskId, 0) * Units::mm;
    const double yHalf = banks->getTriangularBoronMaskParameter(maskId, 1) * Units::mm;
    const double zHalf = banks->getTriangularBoronMaskParameter(maskId, 2) * Units::mm;
    const double xSideToCut = 2 * xHalf - banks->getTriangularBoronMaskParameter(maskId, 3) * Units::mm;
    const double ySideToCut = 2 * yHalf - banks->getTriangularBoronMaskParameter(maskId, 4) * Units::mm;
    const double xCutDir = banks->getTriangularBoronMaskParameter(maskId, 5);
    const double yCutDir = banks->getTriangularBoronMaskParameter(maskId, 6);

    auto triangularMaskBox = new G4Box("TriangularMaskBox", xHalf, yHalf, zHalf);
    const double xCutHalf = sqrt(pow(ySideToCut, 2) + pow(xSideToCut, 2)) / 2;
    const double yCutHalf = ySideToCut * xSideToCut / (2 * xCutHalf) / 2;
    const double alpha = atan(ySideToCut / xSideToCut); //rad

    auto triangularMaskCut = new G4Box("TriangularMaskSustractBox", xCutHalf, yCutHalf, zHalf * 1.1);
    G4RotationMatrix *cutRotationMatrix = new G4RotationMatrix(0, 0, -alpha * xCutDir * yCutDir);
    auto cutCentre = G4ThreeVector(xCutDir * (xHalf - xCutHalf * cos(alpha) + yCutHalf * sin(alpha)), yCutDir * (-(ySideToCut - yHalf) + xCutHalf * sin(alpha) + yCutHalf * cos(alpha)), 0);
    auto triangularMask = new G4SubtractionSolid("TriangularMask", triangularMaskBox, triangularMaskCut, cutRotationMatrix, cutCentre);

    auto B4C_panel_material = getParameterMaterial("B4C_panel_material");
    const int bankId = banks->getTriangularBoronMaskParameter(maskId, 7);
    const std::string maskName = "BoronMask-triangular-"+std::to_string(bankId)+"-"+std::to_string(maskId);
    auto lv_triangularMask = new G4LogicalVolume(triangularMask, B4C_panel_material, maskName);

    return lv_triangularMask;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

G4VPhysicalVolume* GeoBCS::Construct(){
  // this is where we put the entire geometry together, the private functions creating the logical volumes are meant to facilitate the code below  

  auto world_material = getParameterMaterial("world_material"); 
  const double sdd = getParameterDouble("rear_detector_distance_m")*Units::m;

  banks = new bcsBanks(sdd);

  // calculate a value that is big enough to fit your world volume, the "super mother"
  double big_dimension = 1.1*( 1 *Units::m + sdd);

  //World volume:
  auto worldvols = place(new G4Box("World", big_dimension, big_dimension, big_dimension), world_material, 0, 0, 0, 0, INVISIBLE);
  auto lvWorld = worldvols.logvol;
  auto pvWorld = worldvols.physvol;

  // Create and place detector banks
  const bool rearBankOnly = getParameterBoolean("rear_detector_only");
  const bool larmorRearBankExperiment = getParameterBoolean("larmor_rear_bank_experiment");
  const int numberOfBanks = rearBankOnly ? 1 : 9;
  for (int bankId = 0; bankId < numberOfBanks; bankId++){
    auto lv_bank = createBankLV(bankId);

    auto rotation = new G4RotationMatrix();
    rotation->rotateY(banks->getBankRotation(bankId, 1));
    rotation->rotateX(banks->getBankRotation(bankId, 0));
    rotation->rotateZ(banks->getBankRotation(bankId, 2));

    // only for rear bank experiment at Larmor
    double verticalBankPosition = banks->getBankPosition(bankId, 1);
    if(larmorRearBankExperiment){
      std::cout<<"\n ***** \n original vertical position: " << verticalBankPosition;
      verticalBankPosition = 0.5 * banks->getBankSize(bankId, 1) - 1155 *Units::mm + (4+33) *Units::mm; // beam centre at 1155 mm above floor, including the 4 mm electrical isolation layer, that is 33 mm above the Larmor floor (due to the weels) #EDITED 1159 mm->1155 mm
      std::cout<<"\n new vertical position: " << verticalBankPosition << "\n ***** \n";
    }

    place(lv_bank, banks->getBankPosition(bankId, 0), verticalBankPosition, banks->getBankPosition(bankId, 2), lvWorld, ORANGE, bankId, 0, rotation);

    // Add Calibration slit system for Larmor rear bank experiment calibration
    const bool withCalibrationSlits = getParameterBoolean("with_calibration_slits");
    if (bankId == 0 && withCalibrationSlits) {
      const double detBankFrontDistance = banks->detectorSystemFrontDistanceFromBankFront(bankId);
      const double bankSizeZHalf = 0.5* banks->getBankSize(bankId, 2);

      const double verticalPosition = 0;
      const double maskWidthHalf = 0.5 * 976*Units::mm; // 63+100*4+103*4+74 + (3*9) = 976 REDUNDANT!!!
      const double horisontalPosition = maskWidthHalf - 500*Units::mm + 76*Units::mm; //76 mm from the left edge of the B4C mask (that is -500 mm from the centre)


      auto lv_calibrationSlits = createCalibrationSlitMaskLV();
      place(lv_calibrationSlits,
            horisontalPosition, verticalPosition, banks->getBankPosition(bankId, 2) -bankSizeZHalf + detBankFrontDistance - 75*Units::mm, // 75mm in front of the first layer of tubes
            lvWorld, BLACK, -5, 0, rotation);
  }

  }

  
   
  // Add 4 triangular boron masks (added to the World instead of the banks)
  if (!rearBankOnly) {
    for (int maskId = 0; maskId <= 3; maskId++) {
      auto lv_triangularMask = createTriangularMaskLV(maskId);

      const int bankId = banks->getTriangularBoronMaskParameter(maskId, 7);       // 5 or 7
      const double rotateDir = banks->getTriangularBoronMaskParameter(maskId, 6); // +- 1.0

      auto rotation = new G4RotationMatrix();
      rotation->rotateX(banks->getBankRotation(bankId, 2) * rotateDir);

      place(lv_triangularMask, banks->getTriangularBoronMaskPosition(maskId, 0), banks->getTriangularBoronMaskPosition(maskId, 1), banks->getTriangularBoronMaskPosition(maskId, 2), lvWorld, BLACK, -5, 0, rotation);
    }
  }

  delete banks;
  return pvWorld;
}


bool GeoBCS::validateParameters() {
// you can apply conditions to control the sanity of the geometry parameters and warn the user of possible mistakes
  // a nice example: Projects/SingleCell/G4GeoSingleCell/libsrc/GeoB10SingleCell.cc
    return true;  
}

