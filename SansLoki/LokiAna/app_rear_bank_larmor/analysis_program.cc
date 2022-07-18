#include "GriffAnaUtils/All.hh"
#include "Core/Units.hh"
#include "Core/FPE.hh"
#include "Utils/ArrayMath.hh"
#include "Utils/NeutronMath.hh"
#include "SimpleHists/HistCollection.hh"
#include "LokiAna/DetHitApproximation.hh"
#include <string>
#include <cmath>
#include <iostream>
#include <fstream>
#include "MCPL/mcpl.h"

#include "G4GeoLoki/bcsBanks.hh"
//Griff analysis. See https://confluence.esss.lu.se/display/DG/Griff for more info.

#ifndef M_PI
#define M_PI  3.14159265358979323846  //  pi
#endif


int main(int argc, char**argv) {
  const bool createDetectionMcplFile = true;
  
  Core::catch_fpe();
  GriffDataReader dr(argc,argv);

  auto setup = dr.setup();
  auto &geo = setup->geo();
  //printf("QQQ=============  %s \n", geo.getName().c_str());
  if (geo.getName()!="G4GeoLoki/GeoBCSBanks" && geo.getName()!="G4GeoBCS/GeoLarmorBCSExperiment") {
    printf("Error: Wrong setup for this analysis\n");
    return 1;
  }

  auto &gen = setup->gen();

  double sourceSampleDistance = 0*Units::m;
  if (gen.getName()=="G4MCPLPlugins/MCPLGen") {
    sourceSampleDistance = 25.61*Units::m; //From Larmor McStas file
  }
  else if(gen.getName()=="LokiSim.FloodSourceGen/FloodSourceGen"){
    sourceSampleDistance = gen.getParameterDouble("source_sample_distance_meters") *Units::m;
  }
  printf("sourceSampleDistance: %f\n", sourceSampleDistance);

  setup->dump();
  
  DetHitApproximation hit(&dr,1.2*Units::cm,120*Units::keV,"CountingGas" ); // defined for He3 gas

  GriffAnaUtils::SegmentIterator segments_bank(&dr);
  segments_bank.addFilter(new GriffAnaUtils::SegmentFilter_Volume("Bank"));  

  GriffAnaUtils::TrackIterator primary_neutrons(&dr);
  primary_neutrons.addFilter(new GriffAnaUtils::TrackFilter_Primary());
  primary_neutrons.addFilter(new GriffAnaUtils::TrackFilter_PDGCode(2112));

  mcpl_outfile_t detMcpl;
  mcpl_particle_t *mcplParticle;

  if (createDetectionMcplFile == true) {
    detMcpl = mcpl_create_outfile("detectionEvents.mcpl");
    mcpl_hdr_add_comment(detMcpl, "Neutrons in this file are actually detection events and userflags are pixel ID's of hits. Created with ess_lokiana_rear_bank_larmor command.");
    mcpl_enable_userflags(detMcpl);
    mcplParticle = mcpl_get_empty_particle(detMcpl);
  }

  SimpleHists::HistCollection hc;

  const double sampleDetectorDistance = setup->geo().getParameterDouble("rear_detector_distance_m") *Units::m;
  //TODO: try catch block might be needed to handle existing result files with no rear_bank_pixel_number param
  const int rearBankPixelNumber = setup->geo().getParameterInt("rear_bank_pixel_number");

  bcsBanks banks = bcsBanks(sampleDetectorDistance, rearBankPixelNumber);

  const double tubeRadius = banks.tubes->getTubeOuterRadius(); //12.7; 

  const double ymin = -53; //20+1 tube in negative direction
  const int binsy = 1060/2;
  const int binsz = 160;
  int thetabins = 550/2;
  const double trueThetaMax = 55;

  const double zmin = (sampleDetectorDistance - tubeRadius) - 20; //[mm]  //4980 for 5 m sd distance
  const double zmax = (sampleDetectorDistance - tubeRadius) + 140; //[mm //]5140 for 5 m sd distance

  auto h_neutron_xy_bank = hc.book2D("Neutron xy (at bank entering)", 2500, -1250, 1250, 2500, -1250, 1250, "neutron_xy_bank");
       h_neutron_xy_bank->setXLabel("-x [mm]");
       h_neutron_xy_bank->setYLabel("y [mm]");

  auto h_neutron_xy_bank_filtered = hc.book2D("Neutron xy filtered (at bank entering)", 2500, -1250, 1250, 2500, -1250, 1250, "neutron_xy_bank_filtered");
       h_neutron_xy_bank_filtered->setXLabel("-x [mm]");
       h_neutron_xy_bank_filtered->setYLabel("y [mm]");

  auto h_neutron_xy_conv = hc.book2D("Neutron xy (conv)", 2500, -1250, 1250, 2500, -1250, 1250, "neutron_xy_conv");
       h_neutron_xy_conv->setXLabel("-x [mm]");
       h_neutron_xy_conv->setYLabel("y [mm]");
  
  auto h_neutron_xy_hit = hc.book2D("Neutron xy (hit)", 2500, -1250, 1250, 2500, -1250, 1250, "neutron_xy_hit");
       h_neutron_xy_hit->setXLabel("-x [mm]");
       h_neutron_xy_hit->setYLabel("y [mm]");

  auto h_neutron_zy_conv = hc.book2D("Neutron zy (conv)", binsz, zmin, zmax, binsy/2, ymin, -ymin, "neutron_yz_conv");
       h_neutron_zy_conv->setXLabel("z [mm]");
       h_neutron_zy_conv->setYLabel("y [cm]");

  auto h_neutron_zy_hit = hc.book2D("Neutron zy (hit)", binsz, zmin, zmax, binsy/2, ymin, -ymin, "neutron_yz_hit");
       h_neutron_zy_hit->setXLabel("z [mm]");
       h_neutron_zy_hit->setYLabel("y [cm]");


  auto h_neutron_theta = hc.book1D("Neutron true theta (all events)", thetabins, 0, trueThetaMax, "neutron_true_theta");
       h_neutron_theta->setXLabel("Angle [degree]");

  auto h_neutron_theta_hit = hc.book1D("Neutron theta (hit)", thetabins, 0, trueThetaMax, "neutron_theta_hit");
       h_neutron_theta_hit->setXLabel("Angle [degree]");

  const int numberOfPixels = banks.getNumberOfPixels(0);
  const int numberOfStraws = banks.getNumberOfTubes(0) * 7;
  const int numberOfPixelsPerStraw = numberOfPixels / numberOfStraws;
  printf("Number of pixels per straw: %d \t Total number of pixels: %d \n", numberOfPixelsPerStraw, numberOfPixels);

  auto h_neutron_pixel_hit = hc.book2D("Sum weight of hits in pixels (hit)", numberOfPixelsPerStraw, 0, numberOfPixelsPerStraw, numberOfStraws, 0, numberOfStraws, "h_neutron_pixel_hit");
       h_neutron_pixel_hit->setXLabel("Pixel ID along straw");
       h_neutron_pixel_hit->setYLabel("Straw ID");

  auto h_neutron_panelConvCounter = hc.book1D("Neutron conversion counter for 4 panels", 4, 0, 4, "neutron_panelConvCounter");
       h_neutron_panelConvCounter->setXLabel("Panel number");

  auto h_neutron_panelHitCounter = hc.book1D("Neutron hit counter for 4 panels", 4, 0, 4, "neutron_panelHitCounter");
       h_neutron_panelHitCounter->setXLabel("Panel number");

  auto h_panel0_lambda_hit = hc.book1D("Detection neutron wavelength for panel 0", 325, 0, 14, "panel_lambda0_hit");
       h_panel0_lambda_hit->setXLabel("Wavelength [angstrom]");
  auto h_panel1_lambda_hit = hc.book1D("Detection neutron wavelength for panel 1", 325, 0, 14, "panel_lambda1_hit");
       h_panel1_lambda_hit->setXLabel("Wavelength [angstrom]");
  auto h_panel2_lambda_hit = hc.book1D("Detection neutron wavelength for panel 2", 325, 0, 14, "panel_lambda2_hit");
       h_panel2_lambda_hit->setXLabel("Wavelength [angstrom]");
  auto h_panel3_lambda_hit = hc.book1D("Detection neutron wavelength for panel 3", 325, 0, 14, "panel_lambda3_hit");
       h_panel3_lambda_hit->setXLabel("Wavelength [angstrom]");

  auto h_neutron_counters = hc.bookCounts("General neutron counters","neutron_counters"); /////////////
  auto count_initial_neutrons = h_neutron_counters->addCounter("count_initial_neutrons");
  auto count_neutrons_converted = h_neutron_counters->addCounter("count_neutrons_converted");
  auto count_neutrons_hit = h_neutron_counters->addCounter("count_neutrons_hit");


  const double xWidthVacuumTankEnd = 790 *Units::mm;
  const double yHeightVacuumTankEnd = 700 *Units::mm;
  const double zVacuumTankEnd = sampleDetectorDistance - 90*Units::mm; // "The front of the Loki detector was 90 mm in front of the tank”
  const double zBankFront = sampleDetectorDistance - banks.detectorSystemFrontDistanceFromBankFront(0) *Units::mm;
  const double xBankEnterLimit = (xWidthVacuumTankEnd * 0.5) * (zBankFront / zVacuumTankEnd);
  const double yBankEnterLimit = (yHeightVacuumTankEnd * 0.5) * (zBankFront / zVacuumTankEnd);
  //std::cout<<"\n ***** \n xBankEnterLimit: " << xBankEnterLimit << "\n yBankEnterLimit: "<< yBankEnterLimit<< "\n ****** \n";
  
  double tmp_x_offset_meters = gen.hasParameterDouble("x_offset_meters") ? gen.getParameterDouble("x_offset_meters") *Units::m : 0.0;
  double tmp_dx_meter = gen.hasParameterDouble("dx_meter") ? gen.getParameterDouble("dx_meter") *Units::m : 0.0;
  if(tmp_x_offset_meters && tmp_dx_meter){
    printf("Error: both 'x_offset_meters' and 'dx_meter' parameters are defined.\n");
    return 1;
  }
  const double xBeamOffset = tmp_x_offset_meters ? tmp_x_offset_meters :tmp_dx_meter;

  while (dr.loopEvents()) {  
    
    while (auto neutron = primary_neutrons.next()) {
      count_initial_neutrons += 1;

      auto segBegin = neutron->segmentBegin();
      auto stepFirst = segBegin->firstStep();
      const double initialPosition[] = {stepFirst->preGlobalX(), stepFirst->preGlobalY(), stepFirst->preGlobalZ()};

      auto dir_true = stepFirst->preMomentumArray();
      const double theta_true = Utils::theta(dir_true)/Units::degree;
      h_neutron_theta->fill(theta_true, neutron->weight());

      if(auto bankSegment = segments_bank.next()){
        auto stepFirstInBank = bankSegment->firstStep();
        const double xBankEnter = stepFirstInBank->preGlobalX()/Units::mm;
        const double yBankEnter = stepFirstInBank->preGlobalY()/Units::mm;
        h_neutron_xy_bank->fill(-xBankEnter, yBankEnter, neutron->weight());

        //if (xBankEnterLimit < std::fabs(xBankEnter) || yBankEnterLimit < std::fabs(yBankEnter)){
        if ( yBankEnterLimit < std::fabs(yBankEnter) ||
          xBankEnter > xBeamOffset + xBankEnterLimit || xBeamOffset - xBankEnterLimit > xBankEnter ){
          continue;
        }

        h_neutron_xy_bank_filtered->fill(-xBankEnter, yBankEnter, neutron->weight());
      }
      //else{
      //  std::cout<<"\n No bank segment??";
      //}

      auto segL = neutron->lastSegment();
      if (segL->volumeName()=="Converter") {
        count_neutrons_converted += 1;
        
        auto stepL = segL->lastStep();
        const double position_conv[3] = {stepL->postGlobalX(), stepL->postGlobalY(), stepL->postGlobalZ()};
            
        /// volumeCopyNumber() = CountingGas; volumeCopyNumber(1) = Converter; volumeCopyNumber(2) = straw wall; volumeCopyNumber(3) = EmptyTube;
        /// volumeCopyNumber(4) = TubeWall; volumeCopyNumber(5) = EmptyPackBox; volumeCopyNumber(6) = Bank; volumeCopyNumber(7) = World
        const int strawId_conv = segL->volumeCopyNumber(1);
        const int tubeId_conv = segL->volumeCopyNumber(3);
        //const int bankId_conv = segL->volumeCopyNumber(5); only the rear bank is present in the geometry with bankId=0

        
        h_neutron_zy_conv->fill(position_conv[2]/Units::mm, position_conv[1]/Units::cm, neutron->weight());
        h_neutron_xy_conv->fill(-position_conv[0]/Units::mm, position_conv[1]/Units::mm, neutron->weight());

        const int panelNumber_conv = tubeId_conv % 4;
        h_neutron_panelConvCounter->fill(panelNumber_conv, neutron->weight());

        if (hit.eventHasHit()) {
          count_neutrons_hit += 1;
          const double position_hit[3] = {hit.eventHitPositionX(), hit.eventHitPositionY(), hit.eventHitPositionZ()};
 
          h_neutron_zy_hit->fill(position_hit[2]/Units::mm, position_hit[1]/Units::cm, hit.eventHitWeight());
          h_neutron_xy_hit->fill(-position_hit[0]/Units::mm, position_hit[1]/Units::mm, hit.eventHitWeight());

          const double theta_hit = Utils::theta(hit.eventHitPosition())/Units::degree;
          h_neutron_theta_hit->fill(theta_hit, hit.eventHitWeight());

          const int pixelId = banks.getPixelId(0, tubeId_conv, strawId_conv, position_hit[0], position_hit[1]);

          h_neutron_pixel_hit->fill(pixelId%numberOfPixelsPerStraw, std::floor(pixelId/numberOfPixelsPerStraw), hit.eventHitWeight());
          h_neutron_panelHitCounter->fill(panelNumber_conv, hit.eventHitWeight());

          //TODO should implement method (in bcsBanks class) to get positionOnWire_hit coordinate. Ask Judit, how it is done in real data reduction.
          const double sampleToExactHitPositionDistance = std::sqrt(std::pow((position_hit[0] - initialPosition[0]), 2) +
                                                                    std::pow((position_hit[1] - initialPosition[1]), 2) +
                                                                    std::pow((position_hit[2] - initialPosition[2]), 2));

          const double tof_hit = hit.eventHitTime()/Units::ms;
          double velocity_calculated = -1;
          if (tof_hit > 0.0) {
            velocity_calculated = ((sampleToExactHitPositionDistance + sourceSampleDistance) / Units::m) / (hit.eventHitTime() / Units::s);
          }
          else {
            printf("Error in hit tof value, tof zero or negative \n");
            return 1;
          }
          const double lambda_hit_calculated = Utils::neutron_meters_per_second_to_angstrom(velocity_calculated);

          if (panelNumber_conv == 0){
            h_panel0_lambda_hit->fill(lambda_hit_calculated, hit.eventHitWeight());
          }
          else if(panelNumber_conv == 1){
            h_panel1_lambda_hit->fill(lambda_hit_calculated, hit.eventHitWeight());
          }
          else if(panelNumber_conv == 2){
            h_panel2_lambda_hit->fill(lambda_hit_calculated, hit.eventHitWeight());
          }
          else if(panelNumber_conv == 3){
            h_panel3_lambda_hit->fill(lambda_hit_calculated, hit.eventHitWeight());
          }

          if (createDetectionMcplFile == true) {
            mcplParticle->time = hit.eventHitTime()/Units::ms;
            mcplParticle->weight = hit.eventHitWeight();
            mcplParticle->userflags = pixelId; 

            mcplParticle->position[0] = position_hit[0] / Units::cm; //not used
            mcplParticle->position[1] = position_hit[1] / Units::cm; //not used
            mcplParticle->position[2] = position_hit[2] / Units::cm; //not used
            mcplParticle->direction[0] = 0; //dummy
            mcplParticle->direction[1] = 0; //dummy
            mcplParticle->direction[2] = 1; //dummy
            mcplParticle->ekin = 0; //dummy
            mcplParticle->pdgcode = 0; //dummy
            
            mcpl_add_particle(detMcpl, mcplParticle);
          }

        } // end hit in event
      } // end if conversion condition
    }//end of loop over primary neutrons
  } //end of event loop

  //mcpl_close_outfile(f);
  
  if (createDetectionMcplFile == true) {
    mcpl_close_outfile(detMcpl);
  }
  hc.saveToFile("bcsloki_sans", true);
  return 0;
}


