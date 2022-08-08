#include "GriffAnaUtils/All.hh"
#include "Core/Units.hh"
#include "Core/FPE.hh"
#include "Utils/ArrayMath.hh"
#include "Utils/NeutronMath.hh"
#include "SimpleHists/HistCollection.hh"
// #include "GriffB10Common/DetHitApproximation.hh"
#include "LokiAna/DetHitApproximation.hh"
#include <string>
#include <cmath>
#include <iostream>
#include <fstream>
#include "MCPL/mcpl.h"

#include "G4GeoLoki/PixelatedBanks.hh"
//Griff analysis. See https://confluence.esss.lu.se/display/DG/Griff for more info.

#ifndef M_PI
#define M_PI  3.14159265358979323846  //  pi
#endif

/// /// TODOGEANTINO
bool getPrimaryNeutronAndGeantino(GriffDataReader& dr,
                                                  const GriffDataRead::Track*& trk_neutron,
                                                  const GriffDataRead::Track*& trk_geantino)
{
  if (dr.nPrimaryTracks()!=2)
    return false;
  trk_neutron = dr.primaryTrackBegin();
  trk_geantino = trk_neutron +1;
  if (trk_neutron->pdgCode()!=2112)
    std::swap(trk_neutron,trk_geantino);
  if (trk_neutron->pdgCode()!=2112)
    return false;
  if (trk_geantino->pdgCode()!=999)
    return false;
  return true;
}


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
  if (gen.getName()=="G4MCPLPlugins/MCPLGen" && geo.getName()!="G4GeoBCS/GeoLarmorBCSExperiment") { // . changed to /
    sourceSampleDistance = 23.5966*Units::m; //changed from 22.5, might change //23.5966 from McStas mcdisplay-webgl
  }

  setup->dump();

  DetHitApproximation hit(&dr,1.2*Units::cm,120*Units::keV,"CountingGas" ); // defined for He3 gas

  GriffAnaUtils::SegmentIterator segments_gas(&dr);
  segments_gas.addFilter(new GriffAnaUtils::SegmentFilter_Volume("CountingGas"));  

  GriffAnaUtils::TrackIterator primary_neutrons(&dr);
  primary_neutrons.addFilter(new GriffAnaUtils::TrackFilter_Primary());
  primary_neutrons.addFilter(new GriffAnaUtils::TrackFilter_PDGCode(2112));

  GriffAnaUtils::SegmentIterator segments_World(&dr);   
  segments_World.addFilter(new GriffAnaUtils::SegmentFilter_Volume("World"));
  segments_World.addFilter(new GriffAnaUtils::TrackFilter_Primary());
  segments_World.addFilter(new GriffAnaUtils::TrackFilter_PDGCode(2112));

  GriffAnaUtils::SegmentIterator segments_TubeWall(&dr); 
  segments_TubeWall.addFilter(new GriffAnaUtils::SegmentFilter_Volume("TubeWall"));
  segments_TubeWall.addFilter(new GriffAnaUtils::TrackFilter_Primary());
  segments_TubeWall.addFilter(new GriffAnaUtils::TrackFilter_PDGCode(2112));

  GriffAnaUtils::SegmentIterator segments_B4CPanel(&dr); 
  segments_B4CPanel.addFilter(new GriffAnaUtils::SegmentFilter_Volume("B4CPanel"));
  segments_B4CPanel.addFilter(new GriffAnaUtils::TrackFilter_Primary());
  segments_B4CPanel.addFilter(new GriffAnaUtils::TrackFilter_PDGCode(2112));

  GriffAnaUtils::SegmentIterator segments_all(&dr); 
  segments_all.addFilter(new GriffAnaUtils::TrackFilter_Primary());
  segments_all.addFilter(new GriffAnaUtils::TrackFilter_PDGCode(2112));

  //////////LOOOOOK HERE!!
  /*mcpl_outfile_t f = mcpl_create_outfile("testbcs.mcpl");
  mcpl_hdr_add_comment(f,"Neutrons in this file are actually detection hits and userflags are pixel ID's of hits. Created with ess_lokians_bcsloki_ana command.");
  mcpl_enable_userflags(f);
  mcpl_particle_t *p = mcpl_get_empty_particle(f);*/

  mcpl_outfile_t detMcpl;
  mcpl_particle_t *mcplParticle;

  if (createDetectionMcplFile == true) {
    detMcpl = mcpl_create_outfile("detectionEvents.mcpl");
    mcpl_hdr_add_comment(detMcpl, "Neutrons in this file are actually detection events and userflags are pixel ID's of hits. Created with ess_lokiana_loki_ana command.");
    mcpl_enable_userflags(detMcpl);
    mcplParticle = mcpl_get_empty_particle(detMcpl);
  }

  SimpleHists::HistCollection hc;

  auto userData = setup->userData();
  PixelatedBanks* banks;
  const double sampleDetectorDistance = setup->geo().getParameterDouble("rear_detector_distance_m") *Units::m;
  if(userData.count("analysis_rear_bank_pixel_number")){
    const int rearBankPixelNumber = std::stoi(userData["analysis_rear_bank_pixel_number"].c_str());
    banks = new PixelatedBanks(sampleDetectorDistance, rearBankPixelNumber);
  }
  else{ // use default rear bank pixel number
    banks = new PixelatedBanks(sampleDetectorDistance);
  }

  const double tubeRadius = BcsTube::getTubeOuterRadius(); //12.7; 

  //float xmin = -53;
  const double ymin = -53; //20+1 tube in negative direction
  //int tubeNr = 40 + 2; // 40 plus 1-1 empty space on both ends to make the plot nice
  //int binsx = 1060;
  const int binsy = 1060/2;
  const int binsz = 160;
  int thetabins = 550/2;
  const double trueThetaMax = 55;
  //float thetamax = 9.0;
  //float dthetamin = -5; //-0.35
  //float dthetamax = 5; //3 
  //int dthetabins = 1000;
  const double zmin = (sampleDetectorDistance - tubeRadius) - 20; //[mm]  //4980 for 5 m sd distance
  const double zmax = (sampleDetectorDistance - tubeRadius) + 140; //[mm //]5140 for 5 m sd distance
  //int zbins = 160;

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


  auto h_neutron_zy_big_conv = hc.book2D("Neutron zy big scale (conv)", 5200, 0, 5200, 2200, -1000, 1200, "neutron_zy_big_conv");
       h_neutron_zy_big_conv->setXLabel("z [mm]");
       h_neutron_zy_big_conv->setYLabel("y [mm]");

  auto h_neutron_zx_big_conv = hc.book2D("Neutron zx big scale (conv)", 5200, 0, 5200, 2500, -1250, 1250, "neutron_zx_big_conv");
       h_neutron_zx_big_conv->setXLabel("z [mm]");
       h_neutron_zx_big_conv->setYLabel("x [mm]");

  auto h_neutron_theta = hc.book1D("Neutron true theta (all events)", thetabins, 0, trueThetaMax, "neutron_true_theta");
       h_neutron_theta->setXLabel("Angle [degree]");

  auto h_neutron_theta_conv = hc.book1D("Neutron theta (conv)", thetabins, 0, trueThetaMax, "neutron_theta_conv");
       h_neutron_theta_conv->setXLabel("Angle [degree]");

  auto h_neutron_theta_hit = hc.book1D("Neutron theta (hit)", thetabins, 0, trueThetaMax, "neutron_theta_hit");
       h_neutron_theta_hit->setXLabel("Angle [degree]");
  
  auto h_neutron_bank_theta_hit = hc.book2D("Neutron Theta for banks [degree](hit)", thetabins, 0, trueThetaMax, 9, 0, 9, "neutron_bank_theta_hit");
       h_neutron_bank_theta_hit->setXLabel("Theta [degree]");
       h_neutron_bank_theta_hit->setYLabel("Bank id");

  const int numberOfPixels = banks->getTotalNumberOfPixels();
  printf("Number of pixels: %d\n", numberOfPixels);
  // auto h_neutron_pixel_hit_count = hc.book1D("Number of hits in pixels (all banks)", numberOfPixels, 0, numberOfPixels, "neutron_pixel_hit_count");
  //      h_neutron_pixel_hit_count->setXLabel("Pixel ID");
  // auto h_neutron_pixel_hit_weight = hc.book1D("Sum weight of hits in pixels (all banks)", numberOfPixels, 0, numberOfPixels, "neutron_pixel_hit_weight");
  //      h_neutron_pixel_hit_weight->setXLabel("Pixel ID");
  auto h_neutron_pixel_hit = hc.book2D("Sum weight of hits in pixels (hit)", 256, 0, 256, numberOfPixels/256, 0, numberOfPixels/256, "h_neutron_pixel_hit");
       h_neutron_pixel_hit->setXLabel("Pixel ID along straw");
       h_neutron_pixel_hit->setYLabel("Straw ID");

  auto h_neutron_pixel_geantino = hc.book2D("Show pixels the geantinos entered", 256, 0, 256, numberOfPixels/256, 0, numberOfPixels/256, "h_neutron_pixel_geantino");
       h_neutron_pixel_geantino->setXLabel("Pixel ID along straw");
       h_neutron_pixel_geantino->setYLabel("Straw ID");

  auto h_neutron_pixel_geantino_masking = hc.book2D("Show pixels the geantinos entered (no mask transmission)", 256, 0, 256, numberOfPixels/256, 0, numberOfPixels/256, "h_neutron_pixel_geantino_masking");
       h_neutron_pixel_geantino_masking->setXLabel("Pixel ID along straw");
       h_neutron_pixel_geantino_masking->setYLabel("Straw ID");


  const int numberOfPixels_rear = banks->getNumberOfPixels(0);
  auto h_neutron_pixel_rear_hit = hc.book2D("Sum weight of hits in pixels of rear bank (hit)", 256, 0, 256, numberOfPixels_rear/256, 0, numberOfPixels_rear/256, "h_neutron_pixel_rear_hit");
       h_neutron_pixel_rear_hit->setXLabel("Pixel ID along straw");
       h_neutron_pixel_rear_hit->setYLabel("Straw ID");

  auto h_neutron_segment_number_World = hc.book1D("Number of World segments per neutron", 50, 0, 50, "neutron_segment_number_World");

  auto h_neutron_bankIncidentCounter = hc.book1D("Incident neutron counter for the 9 banks", 9, 0, 9, "neutron_bankIncidentCounter");
       h_neutron_bankIncidentCounter->setXLabel("Bank id");

  auto h_neutron_nonFirstBankIncident = hc.book1D("Incident neutrons for banks (only neutrons which had entered different banks previousl)", 9, 0, 9, "neutron_nonFirstBankIncident");
       h_neutron_nonFirstBankIncident->setXLabel("Bank id");

  auto h_neutron_bankToBankIncident = hc.book2D("Incident neutrons for banks (only neutrons which had entered different banks previousl)", 9, 0, 9, 9, 0, 9,"neutron_bankToBankIncident");
       h_neutron_bankToBankIncident->setXLabel("Bank id (hit)");
       h_neutron_bankToBankIncident->setYLabel("Bank id (previous)");

  auto h_neutron_bankPanelConvCounter = hc.book2D("Neutron conversion counter for 4 panels of 9 banks", 9, 0, 9, 4, 0, 4, "neutron_bankPanelConvCounter");
       h_neutron_bankPanelConvCounter->setXLabel("Bank id");
       h_neutron_bankPanelConvCounter->setYLabel("Panel number");

  auto h_neutron_bankPanelHitCounter = hc.book2D("Neutron hit counter for 4 panels of 9 banks", 9, 0, 9, 4, 0, 4, "neutron_bankPanelHitCounter");
       h_neutron_bankPanelHitCounter->setXLabel("Bank id");
       h_neutron_bankPanelHitCounter->setYLabel("Panel number");

  auto h_neutron_panelConvCounter = hc.book1D("Neutron conversion counter for 4 panels", 4, 0, 4, "neutron_panelConvCounter");
       h_neutron_panelConvCounter->setXLabel("Panel number");

  auto h_neutron_panelHitCounter = hc.book1D("Neutron hit counter for 4 panels", 4, 0, 4, "neutron_panelHitCounter");
       h_neutron_panelHitCounter->setXLabel("Panel number");

  auto h_panel_neutronNr_all = hc.book1D("Number of neutrons entering each panel", 4, 0, 4, "panel_neutronNr_all");

  auto h_panel_lambda = hc.book2D("Incident neutron wavelength for panels", 325, 0, 14, 4, 0, 4, "panel_lambda");
       h_panel_lambda->setXLabel("Wavelength [angstrom]");
       h_panel_lambda->setYLabel("Panel number");

  auto h_panel_lambda_hit = hc.book2D("Detection neutron wavelength for panels", 325, 0, 14, 4, 0, 4, "panel_lambda_hit");
       h_panel_lambda_hit->setXLabel("Wavelength [angstrom]");
       h_panel_lambda_hit->setYLabel("Panel number");

  auto h_bank_lambda = hc.book2D("Incident neutron wavelength for banks", 325, 0, 14, 9, 0, 9, "bank_lambda");
       h_bank_lambda->setXLabel("Wavelength [angstrom]");
       h_bank_lambda->setYLabel("Bank id");
  
  auto h_bank_lambda_hit = hc.book2D("Neutron wavelength for banks (hit)", 325, 0, 14, 9, 0, 9, "bank_lambda_hit");
       h_bank_lambda_hit->setXLabel("Wavelength [angstrom]");
       h_bank_lambda_hit->setYLabel("Bank id");

  auto h_neutron_Q_hit = hc.book1D("Neutron Q [1/angstrom] (hit)", 1000, 0, 2, "neutron_Q_hit");
       h_neutron_Q_hit->setXLabel("Q [1/angstrom]");

  auto h_neutron_Q_rear_bank_hit = hc.book1D("Neutron Q rear [1/angstrom] (hit)", 1000, 0, 2, "neutron_Q_rear_bank_hit");
       h_neutron_Q_rear_bank_hit->setXLabel("Q [1/angstrom]");

  auto h_neutron_bank_Q_hit = hc.book2D("Neutron Q for banks [1/angstrom] (hit)", 250, 0, 2, 9, 0, 9, "neutron_bank_Q_hit");
       h_neutron_bank_Q_hit->setXLabel("Q [1/angstrom]");
       h_neutron_bank_Q_hit->setYLabel("Bank id");
       
       
  
  auto h_neutron_counters = hc.bookCounts("General neutron counters","neutron_counters"); /////////////
  //auto count_entering_aluminium = h_neutron_counter->addCounter("count_entering_aluminium");
  auto count_initial_neutrons = h_neutron_counters->addCounter("count_initial_neutrons");
  auto count_neutrons_entering_aluminium = h_neutron_counters->addCounter("count_neutrons_entering_aluminium");
  auto count_neutrons_converted = h_neutron_counters->addCounter("count_neutrons_converted");
  auto count_neutrons_hit = h_neutron_counters->addCounter("count_neutrons_hit");
  auto count_neutrons_entering_B4CPanel = h_neutron_counters->addCounter("count_neutrons_entering_B4CPanel");
  auto count_neutrons_only_World = h_neutron_counters->addCounter("count_neutrons_only_World");
  auto countTestGeantino = h_neutron_counters->addCounter("all_geantino");
  auto countTestGeantinoAbsInMask = h_neutron_counters->addCounter("geantino_in_Mask");
  //auto count_neutrons_enter_B4CPanel_before_TubeWall = h_neutron_counters->addCounter("count_neutrons_enter_B4CPanel_before_TubeWall");
  //auto count_neutrons_enter_B4CPanel_after_TubeWall = h_neutron_counters->addCounter("count_neutrons_enter_B4CPanel_after_TubeWall");

  auto neutron_ending_counters = hc.bookCounts("General neutron counters","neutron_ending_counters"); /////////////
  auto count_neutrons_abs_TubeWall = neutron_ending_counters->addCounter("count_neutrons_abs_TubeWall");
  auto count_neutrons_abs_StrawWall = neutron_ending_counters->addCounter("count_neutrons_abs_StrawWall");
  auto count_neutrons_abs_Converter = neutron_ending_counters->addCounter("count_neutrons_abs_Converter");
  auto count_neutrons_abs_B4CPanel = neutron_ending_counters->addCounter("count_neutrons_abs_B4CPanel");
  auto count_neutrons_abs_AlPanel = neutron_ending_counters->addCounter("count_neutrons_abs_AlPanel");
  auto count_neutrons_abs_BoronMask = neutron_ending_counters->addCounter("count_neutrons_abs_BoronMask");
  auto count_neutrons_abs_Gas = neutron_ending_counters->addCounter("count_neutrons_abs_Gas");
  auto count_neutrons_end_World = neutron_ending_counters->addCounter("count_neutrons_end_World");

  if (numberOfPixels != 1605632) {
    printf("Error: Wrong pixel number for this analysis\n");
    return 1;
  } 

  bool enteredArrayGeantino [1605632]; //array to find pixels the geantinos enter
  bool enteredArrayGeantino_masking[1605632]; //array to find pixels the geantinos enter
  memset(enteredArrayGeantino, false, sizeof(enteredArrayGeantino[0]) * numberOfPixels); //set all values zero
  memset(enteredArrayGeantino_masking, false, sizeof(enteredArrayGeantino_masking[0]) * numberOfPixels); //set all values zero

  while (dr.loopEvents()) {  
    
    
    while (auto neutron = primary_neutrons.next()) {
      count_initial_neutrons += 1;

      auto segBegin = neutron->segmentBegin();
      auto stepFirst = segBegin->firstStep();
      const double initialPosition[] = {stepFirst->preGlobalX(), stepFirst->preGlobalY(), stepFirst->preGlobalZ()};

      auto dir_true = stepFirst->preMomentumArray();
      const double theta_true = Utils::theta(dir_true)/Units::degree;
      h_neutron_theta->fill(theta_true, neutron->weight());

      int seg_count_World = 0;
      double seg_length_World = 0.0;
      double neutron_weight = 0.0;
      //int isBackScattered = 0;

            /// TODOGEANTINO
      const GriffDataRead::Track* trk_neutron;
      const GriffDataRead::Track* trk_geantino;
      //if (!getPrimaryNeutronAndGeantino(dr,trk_neutron,trk_geantino)) {
      if (getPrimaryNeutronAndGeantino(dr,trk_neutron,trk_geantino)) {
        countTestGeantino += 1;

        bool geantinoAbsorbed = false;
        for (auto seg = trk_geantino->segmentBegin(); seg!=trk_geantino->segmentEnd(); ++seg){
               
          
          if (!geantinoAbsorbed && (seg->volumeName().find("BoronMask-") != std::string::npos || seg->volumeName() == "B4CPanel")){
            countTestGeantinoAbsInMask += 1;
            geantinoAbsorbed = true;
            //break;
          }
          else if (seg->volumeName()=="Converter"){
            const int strawId_conv = seg->volumeCopyNumber(1);
            const int tubeId_conv = seg->volumeCopyNumber(3);
            const int bankId_conv = seg->volumeCopyNumber(5);

            auto step = seg->lastStep();
            const int pixelId = banks->getPixelId(bankId_conv, tubeId_conv, strawId_conv, step->postGlobalX(), step->postGlobalY());
           
            //TESTING
            
            if (enteredArrayGeantino[pixelId] == false){ //only the first time
               h_neutron_pixel_geantino->fill(pixelId%256, std::floor(pixelId/256), 1);
               enteredArrayGeantino[pixelId] = true;
            } 
            if (!geantinoAbsorbed && enteredArrayGeantino_masking[pixelId] == false ){
              h_neutron_pixel_geantino_masking->fill(pixelId%256, std::floor(pixelId/256), 1);
              enteredArrayGeantino_masking[pixelId] = true;
            }
          }
        }
        
      }
      /// TODOGEANTINO END

      while (auto segment = segments_World.next()) { // loop over all segments of the World
        ++seg_count_World;
        seg_length_World += segment->segmentLength();
        neutron_weight = segment->getTrack()->weight();
        
        /*if (seg_count_World > 1) {
          auto step0 = segment->firstStep();
          double dir_init[3];
          Utils::normalise(step0->preMomentumArray(), dir_init);
          auto pos_init = step0->preGlobalArray();
          
          if (dir_init[2] > 0) {
            //isBackScattered = 1;

            p->position[0] = pos_init[0] / Units::cm; //mcpl file unit
            p->position[1] = pos_init[1] / Units::cm; //mcpl file unit
            p->position[2] = pos_init[2] / Units::cm; //mcpl file unit
            p->direction[0] = dir_init[0];
            p->direction[1] = dir_init[1];
            p->direction[2] = dir_init[2];
            p->ekin = step0->preEKin();
            p->time = step0->preTime();
            p->weight = neutron->weight();
            p->pdgcode = segment->getTrack()->pdgCode();

            mcpl_add_particle(f, p);
          }
        }*/
      }
      if (seg_length_World /*&& isBackScattered == 1*/) {
        h_neutron_segment_number_World->fill(seg_count_World, neutron_weight);
      }
 
      
      bool hasOnlyWorldSegments = true;
      //bool hasEnteredTubeWall = false;

      while (auto segment = segments_all.next()) { // loop over all segments
        if(segment->volumeName() != "World") {
          hasOnlyWorldSegments = false;
          break;
        }
        /*
        auto step0 = segment->firstStep();
        double dir_init[3];
        Utils::normalise(step0->preMomentumArray(), dir_init);
          
        if (dir_init[2] < 0) {
           break;
        }
    
        if(segment->volumeName() == "TubeWall") {
          hasEnteredTubeWall = true;
        }
        else if(segment->volumeName() == "B4CPanel") {          
          if(hasEnteredTubeWall) {
            count_neutrons_enter_B4CPanel_after_TubeWall += 1;
          }
          else {
            count_neutrons_enter_B4CPanel_before_TubeWall += 1;

            auto pos_init = stepFirst->preGlobalArray();
            double dir_first[3];
            Utils::normalise(stepFirst->preMomentumArray(), dir_first);
            p->position[0] = pos_init[0] / Units::cm; //mcpl file unit
            p->position[1] = pos_init[1] / Units::cm; //mcpl file unit
            p->position[2] = pos_init[2] / Units::cm; //mcpl file unit
            p->direction[0] = dir_first[0];
            p->direction[1] = dir_first[1];
            p->direction[2] = dir_first[2];
            p->ekin = stepFirst->preEKin();
            p->time = stepFirst->preTime();
            p->weight = neutron->weight();
            p->pdgcode = segment->getTrack()->pdgCode();
            // auto pos_init = step0->preGlobalArray();
            // p->position[0] = pos_init[0] / Units::cm; //mcpl file unit
            // p->position[1] = pos_init[1] / Units::cm; //mcpl file unit
            // p->position[2] = pos_init[2] / Units::cm; //mcpl file unit
            // p->direction[0] = dir_init[0];
            // p->direction[1] = dir_init[1];
            // p->direction[2] = dir_init[2];
            // p->ekin = step0->preEKin();
            // p->time = step0->preTime();
            // p->weight = neutron->weight();
            // p->pdgcode = segment->getTrack()->pdgCode();

            mcpl_add_particle(f, p);
          }
          break;
        }*/
      }
      if(hasOnlyWorldSegments) {
        count_neutrons_only_World += 1;
      }
      

      bool firstBankEnter = true;

      int previousPanelNumber = -42; //anything except [0-6] would do it
      int panelNumber = 0;
      int previousBankNumber = -42; //anything except [0-6] would do it
      int bankNumber = 0;
      while (auto tubeWallSegment = segments_TubeWall.next()) { // loop over all segments of the TubeWall

        bankNumber = (int)tubeWallSegment->volumeCopyNumber(2);
        panelNumber = (int)tubeWallSegment->volumeCopyNumber() % 4;
        neutron_weight = tubeWallSegment->getTrack()->weight();


        if (panelNumber != previousPanelNumber && tubeWallSegment->startEKin()) {
          previousPanelNumber = panelNumber;

          h_panel_neutronNr_all->fill(panelNumber, neutron_weight);

          const double actualEkin = tubeWallSegment->startEKin();
          const double actualLambda = Utils::neutronEKinToWavelength(actualEkin) / Units::angstrom;
          h_panel_lambda->fill(actualLambda, panelNumber, neutron_weight);
        }
        if (bankNumber!= previousBankNumber) { //his includes first enter
          if (!firstBankEnter) {
            h_neutron_nonFirstBankIncident->fill(bankNumber, neutron_weight);
            h_neutron_bankToBankIncident->fill(bankNumber, previousBankNumber, neutron_weight);
          }
          firstBankEnter = false;

          previousBankNumber = bankNumber;
          h_neutron_bankIncidentCounter->fill(bankNumber, neutron_weight);
          
          const double actualEkin = tubeWallSegment->startEKin();
          const double actualLambda = Utils::neutronEKinToWavelength(actualEkin) / Units::angstrom;
          h_bank_lambda->fill(actualLambda, bankNumber, neutron_weight);
        }
      }
      segments_TubeWall.reset(); //it is needed

      if (segments_TubeWall.next()) {
        count_neutrons_entering_aluminium += 1;
      }
      segments_TubeWall.reset(); //it is done for each event automatically

      if (segments_B4CPanel.next()) {
        count_neutrons_entering_B4CPanel += 1;
      }
      



      auto segL = neutron->lastSegment();
      if (segL->volumeName()=="Converter") {
        count_neutrons_converted += 1;
        count_neutrons_abs_Converter += 1;
        
        auto stepL = segL->lastStep();
        const double position_conv[3] = {stepL->postGlobalX(), stepL->postGlobalY(), stepL->postGlobalZ()};

        double dir_conv[3];
        Utils::subtract(stepL->postGlobalArray(), stepFirst->preGlobalArray(), dir_conv);
        const double theta_conv = Utils::theta(dir_conv)/Units::degree;
        h_neutron_theta_conv->fill(theta_conv, neutron->weight());
            
        /// volumeCopyNumber() = CountingGas; volumeCopyNumber(1) = Converter; volumeCopyNumber(2) = straw wall; volumeCopyNumber(3) = EmptyTube;
        /// volumeCopyNumber(4) = TubeWall; volumeCopyNumber(5) = EmptyPackBox; volumeCopyNumber(6) = Bank; volumeCopyNumber(7) = World
        const int strawId_conv = segL->volumeCopyNumber(1);
        const int tubeId_conv = segL->volumeCopyNumber(3);
        const int bankId_conv = segL->volumeCopyNumber(5);

        

        h_neutron_zy_conv->fill(position_conv[2]/Units::mm, position_conv[1]/Units::cm, neutron->weight());
        h_neutron_zy_big_conv->fill(position_conv[2]/Units::mm, position_conv[1]/Units::mm, neutron->weight());

        h_neutron_zx_big_conv->fill(position_conv[2]/Units::mm, position_conv[0]/Units::mm, neutron->weight());

        h_neutron_xy_conv->fill(-position_conv[0]/Units::mm, position_conv[1]/Units::mm, neutron->weight());

        const int panelNumber_conv = tubeId_conv % 4;
        h_neutron_bankPanelConvCounter->fill(bankId_conv, panelNumber_conv, neutron->weight());
        h_neutron_panelConvCounter->fill(panelNumber_conv, neutron->weight());

        if (hit.eventHasHit()) {
          count_neutrons_hit += 1;
          const double position_hit[3] = {hit.eventHitPositionX(), hit.eventHitPositionY(), hit.eventHitPositionZ()};
 
          h_neutron_zy_hit->fill(position_hit[2]/Units::mm, position_hit[1]/Units::cm, hit.eventHitWeight());
          h_neutron_xy_hit->fill(-position_hit[0]/Units::mm, position_hit[1]/Units::mm, hit.eventHitWeight());

          const double theta_hit = Utils::theta(hit.eventHitPosition())/Units::degree;
          h_neutron_theta_hit->fill(theta_hit, hit.eventHitWeight());

          const int pixelId = banks->getPixelId(bankId_conv, tubeId_conv, strawId_conv, position_hit[0], position_hit[1]);
          //h_neutron_pixel_hit_count->fill(pixelId, 1);
          //h_neutron_pixel_hit_weight->fill(pixelId, hit.eventHitWeight());
          h_neutron_pixel_hit->fill(pixelId%256, std::floor(pixelId/256), hit.eventHitWeight());

          
          h_neutron_bankPanelHitCounter->fill(bankId_conv, panelNumber_conv, hit.eventHitWeight());
          h_neutron_panelHitCounter->fill(panelNumber_conv, hit.eventHitWeight());

          if (bankId_conv == 0) {
            h_neutron_pixel_rear_hit->fill(pixelId%256, std::floor(pixelId/256), hit.eventHitWeight());
          }

          //TODO should implement method (in PixelatedBanks class) to get positionOnWire_hit coordinate. Ask Judit, how it is done in real data reduction.
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
          const double Q_hit_calculated = 4 * M_PI * sin(0.5 * theta_hit*Units::degree) / lambda_hit_calculated;
          h_neutron_Q_hit->fill(Q_hit_calculated, hit.eventHitWeight());
          h_neutron_bank_Q_hit->fill(Q_hit_calculated, bankId_conv, hit.eventHitWeight());
          h_neutron_bank_theta_hit->fill(theta_hit, bankId_conv, hit.eventHitWeight());

          if(bankId_conv == 0) {//rear bank only
            h_neutron_Q_rear_bank_hit->fill(Q_hit_calculated, hit.eventHitWeight());
          }
          

          h_bank_lambda_hit -> fill(lambda_hit_calculated, bankId_conv, hit.eventHitWeight());
          h_panel_lambda_hit->fill(lambda_hit_calculated, panelNumber, hit.eventHitWeight());

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
      else if (segL->volumeName().find("BoronMask-") != std::string::npos) {
        count_neutrons_abs_BoronMask += 1;
        //Count them for 9 banks?
        //Theta conv? to overlay with detection?
      }      
      else if (segL->volumeName() == "TubeWall") {
        count_neutrons_abs_TubeWall += 1;
      }
      else if (segL->volumeName() == "StrawWall") {
        count_neutrons_abs_StrawWall += 1;
      }
      else if (segL->volumeName() == "EmptyTube" || segL->volumeName() == "CountingGas") {
        count_neutrons_abs_Gas += 1;
      }
      else if (segL->volumeName() == "B4CPanel") { ///
        count_neutrons_abs_B4CPanel += 1;
      }
      else if (segL->volumeName() == "AlPanel") {
        count_neutrons_abs_AlPanel += 1;
      }
      else if (segL->volumeName() == "World") {
        count_neutrons_end_World += 1;
      }

      

    }//end of loop over primary neutrons
  } //end of event loop

  //mcpl_close_outfile(f);
  
  if (createDetectionMcplFile == true) {
    mcpl_close_outfile(detMcpl);
  }
  hc.saveToFile("bcsloki_sans", true);
    
  int indexOffset = 11;
  std::ofstream maskFile;
  maskFile.open ("maskFile.xml");
  maskFile << "<?xml version=\"1.0\"?>\n";
  maskFile << "<detector-masking>\n";
  maskFile << "\t<group>\n";
  maskFile << "\t\t<detids> ";
  
  for(int i=0; i<numberOfPixels; i++){
    if(enteredArrayGeantino_masking[i] == false){
      maskFile << i+indexOffset <<", "; //TODO OFFSET?
    }
  }
  maskFile.seekp(-2, std::ios_base::cur);//Go back with the write pointer to override the last coma and space ", "
  maskFile << " </detids>\n";
  maskFile << "\t</group>\n";
  maskFile << "</detector-masking>";
  maskFile.close();

  return 0;
}


