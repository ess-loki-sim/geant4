/////////////////////////////////////////
// Declaration of our geometry module: //
/////////////////////////////////////////

#include "G4Interfaces/GeoConstructPyExport.hh"
#include "G4Tubs.hh"
#include "G4Box.hh"
#include "G4Transform3D.hh"
#include "G4Vector3D.hh"
#include "G4SubtractionSolid.hh"
#include <cmath>
#include <string> 
#include <cassert>

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
  G4LogicalVolume * createTubeLV(double converter_thickness, double straw_length);
  G4LogicalVolume * createPackBoxLV(double strawLength, int packNumber, int numberOfPacksForInvertedNumbering, int numberOfPacks);
  G4LogicalVolume * createBankLV(int bankId);
  G4LogicalVolume * createTriangularMaskLV(int maskId);
  G4LogicalVolume * createCalibrationMaskLV(calibMasks::calibMasksBase calibMask);

  int getTubeVolumeNumber(int packNumber, int inPackTubeId, int numberOfPacksForInvertedNumbering, int numberOfPacks);
};

// this line is necessary to be able to declare the geometry in the python simulation script 
PYTHON_MODULE { GeoConstructPyExport::exportGeo<GeoBCS>("GeoBCSBanks"); } 

////////////////////////////////////////////
// Implementation of our geometry module: //
////////////////////////////////////////////

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
  
  addParameterBoolean("old_tube_numbering", false);
  
  addParameterString("world_material","G4_Vacuum");
  addParameterString("B4C_panel_material","ESS_B4C:b10_enrichment=0.95"); 
}

G4LogicalVolume * GeoBCS::createTubeLV(double converterThickness, double strawLength){
  const double effectiveStrawLength = strawLength - banks->tubes->getStrawWallThickness(); //This is only epsilon difference...

  auto lv_tube = new G4LogicalVolume(new G4Tubs("TubeWall",0,  banks->tubes->getTubeOuterRadius(), 0.5*strawLength, 0., 2*M_PI),
                                     banks->tubes->tubeWallMaterial, "TubeWall");

  auto lv_empty_tube = place(new G4Tubs("EmptyTube", 0., banks->tubes->getTubeInnerRadius(), 0.5*strawLength, 0., 2*M_PI),
                             banks->tubes->tubeInnerGas, 0,0,0, lv_tube, G4Colour(0,1,1),-2,0,0).logvol;
  
  for (int cpNo = 0; cpNo <= 6; cpNo++){
    auto lv_straw_wall = place(new G4Tubs("StrawWall", 0, banks->tubes->getStrawOuterRadius(), 0.5*strawLength, 0., 2 * M_PI),
                               banks->tubes->strawWallMaterial, banks->tubes->getStrawPositionX(cpNo), banks->tubes->getStrawPositionY(cpNo), 0, lv_empty_tube, ORANGE, cpNo, 0, 0).logvol;

    auto lv_converter = place(new G4Tubs("Converter", 0., banks->tubes->getStrawInnerRadius(), 0.5*effectiveStrawLength, 0., 2 * M_PI),
                              banks->tubes->converterMaterial, 0, 0, 0, lv_straw_wall, G4Colour(0, 1, 1), cpNo + 100, 0, 0).logvol;

    place(new G4Tubs("CountingGas", 0., banks->tubes->getStrawInnerRadius() - converterThickness, 0.5*effectiveStrawLength, 0., 2 * M_PI),
          banks->tubes->countingGas, 0, 0, 0, lv_converter, G4Colour(0, 0, 1), 0, 0, 0);
  }
  return lv_tube;
}

int GeoBCS::getTubeVolumeNumber(int packNumber, int inPackTubeId, int numberOfPacksForInvertedNumbering, int numberOfPacks){
  assert(0 <= inPackTubeId && inPackTubeId <= 7);
  const double rowNumber = packNumber + 0.5* ((int)inPackTubeId/4); // +0.5 for second row (inPackTubeId > 3)
  inPackTubeId %= 4;
  
  const bool oldTubeNumbering = getParameterBoolean("old_tube_numbering");
  if(oldTubeNumbering){
    if(numberOfPacksForInvertedNumbering == 0){
      return rowNumber * 8 + inPackTubeId;
    }
    else{
      return (numberOfPacksForInvertedNumbering*8 - 4) - rowNumber*8 + inPackTubeId;
    }
  }
  else{ // new numbering
    const int layerNr = inPackTubeId; //[0-3]
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
  auto lv_pack_box = new G4LogicalVolume(new G4Box("EmptyPackBox", 0.5*banks->packs->getPackBoxWidth(), 0.5*banks->packs->getPackBoxHeight(), 0.5 * strawLength + banks->packs->getPackBoxIdleLengthOnOneEnd()), banks->packs->packBoxFillMaterial, "EmptyPackBox");
  
  /// Add 8 BCS detector tubes ///
  auto lv_front_tube = createTubeLV(banks->tubes->getFrontTubeConverterThickness(), strawLength); 
  auto lv_back_tube = createTubeLV(banks->tubes->getBackTubeConverterThickness(), strawLength); 
  G4RotationMatrix* tubeRotationMatrix = new G4RotationMatrix(0, 0, banks->packs->getTubeRotation());

  for (int inPackTubeId = 0; inPackTubeId < 8; inPackTubeId++) {
    place((inPackTubeId % 4 < 2) ? lv_front_tube : lv_back_tube, 
          banks->packs->getHorizontalTubeOffset(inPackTubeId), banks->packs->getVerticalTubeOffset(inPackTubeId), 0, 
          lv_pack_box, SILVER, getTubeVolumeNumber(packNumber, inPackTubeId, numberOfPacksForInvertedNumbering, numberOfPacks), 0, tubeRotationMatrix);
  }
  /// Add B4C panel behind detectors in 3 parts ///
  const double B4CLengthHalf = 0.5*strawLength + banks->packs->getB4CLengthOverStrawOnOneEnd();

  for (int partId = 0; partId < 3; partId++){
    place(new G4Box("B4CPanel", 0.5*banks->packs->getB4CPartThickness(partId), 0.5*banks->packs->getB4CPartHeight(partId), B4CLengthHalf), 
          banks->packs->B4CPanelMaterial, 
          banks->packs->getB4CPartHorizontalOffset(partId), banks->packs->getB4CPartVerticalOffset(partId), 0, 
          lv_pack_box, G4Colour(0, 1, 0), -2, 0, new G4RotationMatrix());
    }
  /// Add Al behing the B4C panel in 2 parts ///
  for (int partId = 0; partId < 2; partId++){
    place(new G4Box("AlPanel", 0.5*banks->packs->getAlPartThickness(partId), 0.5*banks->packs->getAlPartHeight(partId), B4CLengthHalf), 
          banks->packs->AlPanelMaterial,
          banks->packs->getAlPartHorizontalOffset(partId),  banks->packs->getAlPartVerticalOffset(partId), 0,
          lv_pack_box, SILVER, -2, 0, new G4RotationMatrix());
    }
  return lv_pack_box;
}

///////////  CREATE CALIBRATION SLIT LOGICAL VOLUME  //////////////////////////
G4LogicalVolume *GeoBCS::createCalibrationMaskLV(calibMasks::calibMasksBase calibMask){
  const std::string maskName = "BoronMask-"+calibMask.name;
  const double maskThicknessHalf = 0.5*calibMask.thickness;
  const double maskHeightHalf = 0.5*calibMask.height;
  const double maskFullWidthHalf = 0.5*calibMask.getWidth();

  auto lv_calibrationMask = new G4LogicalVolume(new G4Box("EmptyCalibMaskBox", maskThicknessHalf, maskHeightHalf, maskFullWidthHalf), 
                                                 banks->calibMasks->maskBoxMaterial, "CalibMaskBox");
  
  double offset = 0.0;
  int i = 0;
  for(auto part = calibMask.pattern.begin(); part != calibMask.pattern.end(); part++,i++ ) {
    const double partWidth = *part;
    if(i % 2 == 0) { //The pattern is: maskPart ,slit, maskPart, slit...  
      place(new G4Box(maskName, maskThicknessHalf, maskHeightHalf, 0.5*partWidth), 
          banks->calibMasks->maskMaterial,
          0., 0., -maskFullWidthHalf + offset + 0.5*partWidth,
          lv_calibrationMask, DARKPURPLE, -5, 0, new G4RotationMatrix());
    }
    offset += partWidth;
  }
  return lv_calibrationMask;
}


///////////  CREATE DETECTOR BANK LOGICAL VOLUME  //////////////////////////
G4LogicalVolume *GeoBCS::createBankLV(int bankId){
  const double strawLength = banks->getStrawLengthByBankId(bankId);

  const double pack_pack_distance = banks->getPackPackDistance();
  const int numberOfPacks = banks->getNumberOfPacksByBankId(bankId); 
  
  const double packRotation = banks->getPackRotation();

  const double bankSizeXHalf = 0.5* banks->getBankSize(bankId, 0);
  const double bankSizeYHalf = 0.5* banks->getBankSize(bankId, 1);
  const double bankSizeZHalf = 0.5* banks->getBankSize(bankId, 2);

  auto lv_bank = new G4LogicalVolume(new G4Box("EmptyPanelBox", bankSizeZHalf, bankSizeYHalf, bankSizeXHalf),
                                     banks->packs->packBoxFillMaterial, "Bank");

  // override lv_bank to subtract some empty part of front left and right banks where front top and bottom banks would overlap with them
  if (bankId == 8 || bankId == 6) {  
    auto fullBankBox = new G4Box("EmptyPanelBox", bankSizeZHalf, bankSizeYHalf, bankSizeXHalf);
    auto bankBoxCut = new G4Box("EmptyPanelBox", 15.0, 70.0, 60.0);
    auto bankBox = new G4SubtractionSolid("EmptyPanelBox", fullBankBox, bankBoxCut, 0, G4ThreeVector(-145.0, -42.0, -425.0));
    
    lv_bank = new G4LogicalVolume(bankBox, banks->packs->packBoxFillMaterial, "Bank");
  }

  int numberOfPacksForInvertedNumbering = 0; 
  if (banks->areTubesInverselyNumbered(bankId)){ //not very nice solution...
    numberOfPacksForInvertedNumbering = numberOfPacks;
  }

  for (int packNumber = 0; packNumber < numberOfPacks; ++packNumber){
    auto lv_pack_box = createPackBoxLV(strawLength, packNumber, numberOfPacksForInvertedNumbering, numberOfPacks);         
    place(lv_pack_box, banks->getTopmostPackPositionInBank(bankId, 2), -(packNumber * pack_pack_distance - banks->getTopmostPackPositionInBank(bankId, 1)), banks->getTopmostPackPositionInBank(bankId, 0), lv_bank, G4Colour(0, 1, 1), -2, 0, new G4RotationMatrix(0, 0, packRotation));
  }

  const int numberOfBoronMasks = banks->masks->getNumberOfBoronMasks(bankId);
  for (int maskId = 0; maskId < numberOfBoronMasks; ++maskId){
    const std::string maskName = "BoronMask-"+std::to_string(bankId)+"-"+std::to_string(maskId);
    place(new G4Box(maskName, 0.5*banks->masks->getSize(bankId, maskId, 2), 0.5*banks->masks->getSize(bankId, maskId, 1), 0.5*banks->masks->getSize(bankId, maskId, 0)),
            banks->masks->maskMaterial, 
            banks->getBoronMaskPosition(bankId, maskId, 2), banks->getBoronMaskPosition(bankId, maskId, 1), banks->getBoronMaskPosition(bankId, maskId, 0), 
            lv_bank, BLACK, -2, 0, new G4RotationMatrix(0, 0, banks->masks->getRotation(bankId, maskId)));
  }

  // Add BeamStop to the Rear Bank volume
  const bool withBeamstop = getParameterBoolean("with_beamstop");
  if (bankId == 0 && withBeamstop) {
    const std::string maskName = "BoronMask-Beamstop";
    const double detBankFrontDistance = banks->detectorSystemFrontDistanceFromBankFront(bankId);
    
    place(new G4Box(maskName, 0.5 * 1*Units::mm, 0.5 * 5*Units::cm, 0.5 * 5*Units::cm),
          banks->masks->maskMaterial,
          -bankSizeZHalf + detBankFrontDistance - 5*Units::cm, 0, 0,
          lv_bank, BLACK, -5, 0, new G4RotationMatrix());
  }

/*
  // Add Calibration slit mask to the Rear Bank volume for Larmor rear bank experiment calibration
  const bool withCalibrationSlits = getParameterBoolean("with_calibration_slits");
  if (bankId == 0 && withCalibrationSlits) {
    const double detBankFrontDistance = banks->detectorSystemFrontDistanceFromBankFront(bankId);
    
    const double verticalBankPosition = 0.5 * banks->getBankSize(bankId, 1) - 1155 *Units::mm + (4+33) *Units::mm; // beam centre at 1155 mm above floor, including the 4 mm electrical isolation layer, that is 33 mm above the Larmor floor (due to the weels)
    const double verticalPosition = - verticalBankPosition;

    auto lv_calibrationMask = createCalibrationMaskLV();
    place(lv_calibrationMask,
          -bankSizeZHalf + detBankFrontDistance - 55*Units::mm, verticalPosition, 0, // 75mm in front of the first layer of tubes
          lv_bank, BLACK, -5, 0, new G4RotationMatrix());
  }*/

  return lv_bank;
 }

G4LogicalVolume *GeoBCS::createTriangularMaskLV(int maskId){
    //creating special triangular volume by subtraction of 2 boxes
    const double xHalf = banks->masks->getHalfSizeOfTriangularMask(maskId, 0);
    const double yHalf = banks->masks->getHalfSizeOfTriangularMask(maskId, 1);
    const double zHalf = banks->masks->getHalfSizeOfTriangularMask(maskId, 2);
    const double xSideToCut = 2 * xHalf - banks->masks->getCutPointOfTriangularMask(maskId, 0);
    const double ySideToCut = 2 * yHalf - banks->masks->getCutPointOfTriangularMask(maskId, 1);
    const double xCutDir = banks->masks->getCutDirOfTriangularMask(maskId, 0);
    const double yCutDir = banks->masks->getCutDirOfTriangularMask(maskId, 1);

    auto triangularMaskBox = new G4Box("TriangularMaskBox", xHalf, yHalf, zHalf);
    const double xCutHalf = sqrt(pow(ySideToCut, 2) + pow(xSideToCut, 2)) / 2;
    const double yCutHalf = ySideToCut * xSideToCut / (2 * xCutHalf) / 2;
    const double alpha = atan(ySideToCut / xSideToCut); //rad

    auto triangularMaskCut = new G4Box("TriangularMaskSustractBox", xCutHalf, yCutHalf, zHalf * 1.1);
    G4RotationMatrix *cutRotationMatrix = new G4RotationMatrix(0, 0, -alpha * xCutDir * yCutDir);
    auto cutCentre = G4ThreeVector(xCutDir * (xHalf - xCutHalf * cos(alpha) + yCutHalf * sin(alpha)), yCutDir * (-(ySideToCut - yHalf) + xCutHalf * sin(alpha) + yCutHalf * cos(alpha)), 0);
    auto triangularMask = new G4SubtractionSolid("TriangularMask", triangularMaskBox, triangularMaskCut, cutRotationMatrix, cutCentre);

    const int bankId = banks->masks->getBankIdOfTriangularMask(maskId);
    const std::string maskName = "BoronMask-triangular-"+std::to_string(bankId)+"-"+std::to_string(maskId);
    auto lv_triangularMask = new G4LogicalVolume(triangularMask, banks->masks->maskMaterial, maskName);

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
  bool rearBankOnly = getParameterBoolean("rear_detector_only");
  const bool larmorRearBankExperiment = getParameterBoolean("larmor_rear_bank_experiment");
  if(larmorRearBankExperiment) { rearBankOnly = true; }

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

    // Add Calibration slit masks
    const bool withCalibrationSlits = getParameterBoolean("with_calibration_slits");

    if (withCalibrationSlits && bankId!=6 && bankId!=8 ) {
      std::string calibMaskName = "lokiStandard-"+std::to_string(bankId);
      if (larmorRearBankExperiment) { calibMaskName = "larmorCdCalibMask"; }
      const auto calibMask = banks->calibMasks->getCalibMask(calibMaskName);
      auto lv_calibrationMask = createCalibrationMaskLV(calibMask);
      
      place(lv_calibrationMask,
            banks->getCalibMaskPosition(calibMask, bankId, 0), banks->getCalibMaskPosition(calibMask, bankId, 1), banks->getCalibMaskPosition(calibMask, bankId, 2),
            lvWorld, PURPLE, -5, 0, rotation);
      }
  }

  
   
  // Add 4 triangular boron masks (added to the World instead of the banks)
  if (!rearBankOnly) {
    for (int maskId = 0; maskId <= 3; maskId++) {
      auto lv_triangularMask = createTriangularMaskLV(maskId);

      const int bankId = banks->masks->getBankIdOfTriangularMask(maskId);       // 5 or 7
      const double rotateDir = banks->masks->getCutDirOfTriangularMask(maskId, 1); // +- 1.0

      auto rotation = new G4RotationMatrix();
      rotation->rotateX(banks->getBankRotation(bankId, 2) * rotateDir);

      place(lv_triangularMask, 
            banks->getTriangularBoronMaskPosition(maskId, 0), banks->getTriangularBoronMaskPosition(maskId, 1), banks->getTriangularBoronMaskPosition(maskId, 2), 
            lvWorld, BLACK, -5, 0, rotation);
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

