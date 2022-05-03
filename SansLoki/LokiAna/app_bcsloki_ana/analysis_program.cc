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

//Griff analysis. See https://confluence.esss.lu.se/display/DG/Griff for more info.

int main(int argc, char**argv) {
  
  Core::catch_fpe();
  GriffDataReader dr(argc,argv);

  auto setup = dr.setup();
  auto &geo = setup->geo();
  //printf("QQQ=============  %s \n", geo.getName().c_str());
  if (geo.getName()!="G4GeoLoki/GeoLarmorBCSExperiment" && geo.getName()!="G4GeoBCS/GeoLarmorBCSExperiment") {
    printf("Error: Wrong setup for this analysis\n");
    return 1;
  }

  auto &gen = setup->gen();

  double source_to_sample_distance = 0*Units::m;
  if (gen.getName()=="G4MCPLPlugins.MCPLGen")
    source_to_sample_distance = 22.5*Units::m;

  setup->dump();

  DetHitApproximation hit(&dr,1.2*Units::cm,120*Units::keV,"CountingGas" ); // defined for He3 gas
  
  GriffAnaUtils::SegmentIterator segments_gas(&dr);
  segments_gas.addFilter(new GriffAnaUtils::SegmentFilter_Volume("CountingGas"));

  GriffAnaUtils::TrackIterator primary_neutrons(&dr);
  primary_neutrons.addFilter(new GriffAnaUtils::TrackFilter_Primary());
  primary_neutrons.addFilter(new GriffAnaUtils::TrackFilter_PDGCode(2112));


  mcpl_outfile_t f = mcpl_create_outfile("testbcs.mcpl");
  mcpl_hdr_add_comment(f,"Neutrons in this file are actually detection hits and userflags are pixel ID's of hits. Created with ess_lokians_bcsloki_ana command.");
  mcpl_enable_userflags(f);
  mcpl_particle_t *p = mcpl_get_empty_particle(f);


  //////////LOOOOOK HERE!!
  
  
  SimpleHists::HistCollection hc;

  float xmin = -500.0;
  float ymin = -500.0;
  int binsx = 500;
  int binsy = binsx;
  int thetabins = 150;
  float thetamax = 3;
  float qmax = 0.5;
  float qmin = 0.;
  int qbins = 500;
 
  auto h_neutron_x = hc.book1D("Neutron true x",binsx,xmin,-xmin,"neutron_x");
  auto h_neutron_x_FP = hc.book1D("Neutron x (first pass)",binsx,xmin,-xmin,"neutron_x_FP");
  auto h_neutron_x_conv = hc.book1D("Neutron x (converted)",binsx,xmin,-xmin,"neutron_x_conv");
  auto h_hit_x = hc.book1D("hit x [cm]",binsx,xmin,-xmin,"hit_x"); 
  auto h_hit_x_true = hc.book1D("hit x true [cm]",binsx,xmin,-xmin,"hit_x_true"); 
  
  auto h_neutron_y = hc.book1D("Neutron true y",binsy,ymin,-ymin,"neutron_y");
  auto h_neutron_y_FP = hc.book1D("Neutron y (first pass)",binsy,ymin,-ymin,"neutron_y_FP");
  auto h_neutron_y_conv = hc.book1D("Neutron y (converted)",binsy,ymin,-ymin,"neutron_y_conv");
  auto h_hit_y = hc.book1D("hit y [cm]",binsy,ymin,-ymin,"hit_y");
  auto h_hit_y_true = hc.book1D("hit y true [cm]",binsy,ymin,-ymin,"hit_y_true");

  auto h_neutron_theta = hc.book1D("Neutron true theta (all events)",thetabins,0,thetamax,"neutron_theta");
  auto h_neutron_theta_FP = hc.book1D("Neutron theta (first pass)",thetabins,0,thetamax,"neutron_theta_FP");
  auto h_neutron_theta_conv = hc.book1D("Neutron theta (converted)",thetabins,0,thetamax,"neutron_theta_conv");
  auto h_hit_theta = hc.book1D("hit theta [degree]", thetabins,0,thetamax,"hit_theta");
  auto h_hit_theta_true = hc.book1D("hit theta true [degree]", thetabins,0,thetamax,"hit_theta_true");
  auto h_hit_dthth = hc.book1D("hit deltaTheta/thetaTrue", 200,-4,4,"hit_dthth");

  auto h_neutron_phi = hc.book1D("Neutron true phi (all events)", 380,-190.0,190.0,"neutron_phi");
  auto h_hit_phi = hc.book1D("hit phi [degree]", 360,-180,180,"hit_phi");

  auto h_neutron_theta_conv_cut = hc.book1D("Neutron theta (converted)",thetabins,0,thetamax,"neutron_theta_conv_cut");
  auto h_neutron_x_conv_cut = hc.book1D("Neutron x (converted)",binsx,xmin,-xmin,"neutron_x_conv_cut");
  auto h_neutron_y_conv_cut = hc.book1D("Neutron y (converted)",binsy,ymin,-ymin,"neutron_y_conv_cut");

  auto h_neutron_theta_FP_cut = hc.book1D("Neutron theta (first pass)",thetabins,0,thetamax,"neutron_theta_FP_cut");
  auto h_neutron_x_FP_cut = hc.book1D("Neutron x (first pass)",binsx,xmin,-xmin,"neutron_x_FP_cut");
  auto h_neutron_y_FP_cut = hc.book1D("Neutron y (first pass)",binsy,ymin,-ymin,"neutron_y_FP_cut");

  auto h_neutron_xy = hc.book2D("Neutron true xy",binsx,xmin,-xmin,binsy,ymin,-ymin,"neutron_xy");
  h_neutron_xy->setXLabel("x [cm]");
  h_neutron_xy->setXLabel("y [cm]");
 
  auto h_neutron_xy_FP = hc.book2D("Neutron xy (first pass)",binsx,xmin,-xmin,binsy,ymin,-ymin,"neutron_xy_FP");
  h_neutron_xy_FP->setXLabel("x [cm]");
  h_neutron_xy_FP->setXLabel("y [cm]");

  auto h_neutron_xy_conv = hc.book2D("Neutron xy (converted)",binsx,xmin,-xmin,binsy,ymin,-ymin,"neutron_xy_conv");
  h_neutron_xy_conv->setXLabel("x [cm]");
  h_neutron_xy_conv->setXLabel("y [cm]");

  auto h_hit_xy = hc.book2D("hit xy [cm]",binsx,xmin,-xmin,binsy,ymin,-ymin,"hit_xy");
  h_hit_xy->setXLabel("x [cm]");
  h_hit_xy->setYLabel("y [cm]");
  
  auto h_neutron_Q = hc.book1D("Neutron true Q",qbins,qmin,qmax,"neutron_Q");
  auto h_neutron_Q_FP = hc.book1D("Neutron Q from first pass in gas",qbins,qmin,qmax,"neutron_Q_FP");
  auto h_neutron_Q_conv = hc.book1D("Neutron Q at conversion point",qbins,qmin,qmax,"neutron_Q_conv");
  auto h_hit_Q = hc.book1D("Hit Q [1/angstrom]",qbins,qmin,qmax,"hit_Q");
  auto h_hit_Q_true = hc.book1D("Hit Q true [1/angstrom]",qbins,qmin,qmax,"hit_Q_true");
  auto h_hit_dQQ = hc.book1D("hit deltaQ/Qtrue",200,-4,4,"hit_dQQ");

  auto h_neutron_ekin = hc.book1D("Neutron true ekin",500,0,100,"neutron_ekin");
  auto h_neutron_ekin_FP = hc.book1D("Neutron ekin (first pass)",500,0,100,"neutron_ekin_FP");
  auto h_neutron_ekin_conv = hc.book1D("Neutron ekin at conversion point",500,0,100,"neutron_ekin_conv");
  auto h_neutron_ekin_eff = hc.book1D("Effective neutron ekin",500,0,100,"neutron_ekin_eff");

  auto h_hit_dll = hc.book1D("hit deltaLambda/lambda_true",400,-0.05,0.05,"hit_dll"); 

  auto h_hit_phivstheta = hc.book2D("hit phi vs theta [degree]", 180,0,90,360,-180,180,"hit_phivstheta");
  h_hit_phivstheta->setXLabel("theta [degree]");
  h_hit_phivstheta->setYLabel("phi [degree]");

  auto h_edep = hc.book1D("Energy deposited in counting gas [keV]",200,0,800,"energyDeposition");

  auto h_neutron_tof = hc.book1D("Neutron true tof (all events)",500,0,500,"neutron_tof");
  //auto h_neutron_tof_FP = hc.book1D("Neutron theta (first pass)",300,0,6,"neutron_theta_FP");
  //auto h_neutron_tof_conv = hc.book1D("Neutron theta (converted)",300,0,6,"neutron_theta_converted");
  auto h_hit_tof = hc.book1D("hit tof [ms]",500,0,500,"hit_tof");
  auto h_neutron_lambda = hc.book1D("Neutron true wavelength (all events)",100,0,15,"neutron_lambda");
  auto h_neutron_nsegments = hc.book1D("Number of segments per neutron",25,0,25,"neutron_nsegments");
  auto h_neutron_steel=hc.book1D(" Number of times crossed the steel",25,0,25, "neutron_steel");
  auto h_neutron_steel_morethanonestep = hc.book1D("More than one time crossed the steel",25,0,25, "neutron_steel_morethanonestep");
  auto h_neutron_steel_x = hc.book1D("Neutron x step distribution",binsx,xmin,-xmin,"neutron_x_step");
  auto h_neutron_steel_y = hc.book1D("Neutron y step distibution",binsx,xmin,-xmin,"neutron_y_step");
 
  while (dr.loopEvents()) { 
    
    //NB: G4McStas.MonNDGun is no longer supported, use mcpl input instead!
    //if(gen.getName()=="G4McStas.MonNDGun")
    //if(dr.eventNumber()%3==0) continue; // only scattered neutrons
    //if(dr.eventNumber()%3!=0) continue; // only non-scattered neutrons
    
    double edep = 0;
    double w = 1.;
    while (auto segment = segments_gas.next()) {
      edep += segment->eDep();
      //assert((segment->getTrack()->weight()==neutron->weight())&&"different weights within segments of the same track");
      w = segment->getTrack()->weight();
    }
    
    if(edep) h_edep->fill(edep/Units::keV,w);
    double edep_daughters = 0.;
    
    // w and neutron->weight() are the same
    while(auto neutron = primary_neutrons.next()) {
      h_neutron_nsegments->fill(neutron->nSegments(),neutron->weight());
      
      // get the first step of the first segment of the primary neutron
      // in the first volume it enters
      // the particle properties have to be the same as for the generator
      auto seg0 = neutron->segmentBegin();
      auto step0 = seg0->firstStep();

      int countSteel = 0;
      
      auto dir_true = step0->preMomentumArray();
      const double phi_true = Utils::phi(dir_true);
      const double theta_true = Utils::theta(dir_true);
      const double ekin_true = step0->preEKin();

      // Q doesn't make sense for the FlexGen
      const double Q_true = 4*M_PI*sin(0.5*theta_true)/Utils::neutronEKinToWavelength(ekin_true);
      
      h_neutron_lambda->fill(Utils::neutronEKinToWavelength(ekin_true)/Units::angstrom, neutron->weight());
      h_neutron_ekin->fill(ekin_true/Units::meV, neutron->weight());
      h_neutron_Q->fill(Q_true*Units::angstrom, neutron->weight());

      h_neutron_theta->fill(theta_true/Units::degree, neutron->weight());
      h_neutron_phi->fill(phi_true/Units::degree, neutron->weight());
      h_neutron_tof->fill(seg0->startTime()/Units::ms, neutron->weight()); //step0->preTime()
      h_neutron_xy->fill(step0->preGlobalX()/Units::cm, step0->preGlobalY()/Units::cm, neutron->weight());
      h_neutron_x->fill(step0->preGlobalX()/Units::cm,neutron->weight());
      h_neutron_y->fill(step0->preGlobalY()/Units::cm,neutron->weight());
     
      
      // loop over neutron segments, until you hit the first
      // segment in the counting gas (first pass=FP)
      auto segE = neutron->segmentEnd();
      auto seg1 = seg0;
      for (++seg1;seg1<segE;++seg1) {
        if (seg1->volumeName()=="CountingGas")
          break;
      }
      if (seg1==segE) {
        //Neutron never reaches gas
        continue;
      }
      
      auto step1 = seg1->firstStep();
      assert(step0&&step1&&"step info required");
      
      double dir_FP[3];
      Utils::subtract(step1->preGlobalArray(),step0->preGlobalArray(),dir_FP);

      const double theta_FP = Utils::theta(dir_FP); 
      //const double theta_FP = Utils::theta(step1->preGlobalArray());
      const double ekin_FP = step1->preEKin(); 
      const double Q_FP = 4*M_PI*sin(0.5*theta_FP)/Utils::neutronEKinToWavelength(ekin_FP);

      h_neutron_Q_FP->fill(Q_FP*Units::angstrom, neutron->weight());
      h_neutron_theta_FP->fill(theta_FP/Units::degree, neutron->weight());
      h_neutron_ekin_FP->fill(ekin_FP/Units::meV, neutron->weight());  
      h_neutron_xy_FP->fill(step1->preGlobalX()/Units::cm, step1->preGlobalY()/Units::cm, neutron->weight());
      h_neutron_x_FP->fill(step1->preGlobalX()/Units::cm, neutron->weight());
      h_neutron_y_FP->fill(step1->preGlobalY()/Units::cm, neutron->weight());

      if(theta_FP/Units::degree>0.1) {
        h_neutron_theta_FP_cut->fill(theta_FP/Units::degree, neutron->weight());
        h_neutron_x_FP_cut->fill(step1->postGlobalX()/Units::cm,neutron->weight());
        h_neutron_y_FP_cut->fill(step1->postGlobalY()/Units::cm,neutron->weight());
      }
       
      auto segL = neutron->lastSegment();
      if(segL->volumeName()=="Converter") {
        
        for (auto seg = neutron->segmentBegin(); seg!=neutron->segmentEnd(); ++seg) {
          if (seg->volumeName()=="TubeWall") {
            countSteel++;
            h_neutron_steel->fill(countSteel, neutron->weight());
          }
          //fills the histograms with those no=eutrons that crossed the steel more than once.
          if (countSteel > 1) {
            h_neutron_steel_morethanonestep->fill(countSteel, neutron->weight());
            h_neutron_steel_x->fill(step1->postGlobalX()/Units::cm,neutron->weight());
            h_neutron_steel_y->fill(step1->postGlobalY()/Units::cm,neutron->weight());
          }
          
          // if (countSteel > 2) {
          //   std::cout<<" seed " << dr.seed() << std::endl;
          //   std::cout<< "volumeName = " << seg->volumeName() << std::endl;
          //   std::cout<< dr.seed() << std::endl;
          //   printf("counts=%d",countSteel);
          //   if (seg->volumeName()=="Steel") {
          //     std::cout<< "nOfVolume = " << seg->volumeCopyNumber() << std::endl;
          //   }
          // } 
        }
        
        auto stepL = segL->lastStep();
        assert(step0&&stepL&&"step info required");
        
        double dir_conv[3];
        Utils::subtract(stepL->postGlobalArray(),step0->preGlobalArray(),dir_conv);

        const double theta_conv = Utils::theta(dir_conv);
        //const double theta_conv = Utils::theta(stepL->postGlobalArray());
        const double ekin_conv = stepL->preEKin(); // postEKin() returns 0 since the neutron stops
        const double Q_conv = 4*M_PI*sin(0.5*theta_conv)/Utils::neutronEKinToWavelength(ekin_conv);       
    
        h_neutron_theta_conv->fill(theta_conv/Units::degree, neutron->weight());       
        h_neutron_Q_conv->fill(Q_conv*Units::angstrom, neutron->weight());        
        h_neutron_ekin_conv->fill(ekin_conv/Units::meV, neutron->weight());   
        h_neutron_xy_conv->fill(stepL->postGlobalX()/Units::cm,stepL->postGlobalY()/Units::cm,neutron->weight());
        h_neutron_x_conv->fill(stepL->postGlobalX()/Units::cm,neutron->weight());
        h_neutron_y_conv->fill(stepL->postGlobalY()/Units::cm,neutron->weight());

        if(theta_conv/Units::degree>0.1) {
          h_neutron_theta_conv_cut->fill(theta_conv/Units::degree, neutron->weight());
          h_neutron_x_conv_cut->fill(stepL->postGlobalX()/Units::cm,neutron->weight());
          h_neutron_y_conv_cut->fill(stepL->postGlobalY()/Units::cm,neutron->weight());
        }

        if(hit.eventHasHit()) {
          const double theta_hit = Utils::theta(hit.eventHitPosition());
          
          const double phi_hit = Utils::phi(hit.eventHitPosition());
          const double tof_hit = hit.eventHitTime();      
          const double sample_to_detector_distance = sqrt( (hit.eventHitPositionX()-step0->preGlobalArray()[0])*(hit.eventHitPositionX()-step0->preGlobalArray()[0]) +
                                                           (hit.eventHitPositionY()-step0->preGlobalArray()[1])*(hit.eventHitPositionY()-step0->preGlobalArray()[1]) +
                                                           (hit.eventHitPositionZ()-step0->preGlobalArray()[2])*(hit.eventHitPositionZ()-step0->preGlobalArray()[2]) );

          // unsigned volumeDepthStored(6);//Number of "generations" of volumes which can be querried.
          const std::string& physicalVolumeName(unsigned idepth=1);
          int volumeCopyNumber(unsigned idepth=1);



          
          double velocity=-1;
          if(tof_hit) 
            velocity = ((sample_to_detector_distance + source_to_sample_distance)/Units::m)/(tof_hit/Units::s);
          else {
            printf("Error in hit tof value, tof zero or negative \n");
            return 1;
          }
          
          const double lambda = Utils::neutron_meters_per_second_to_angstrom(velocity);
          const double Q_hit = 4*M_PI*sin(0.5*theta_hit)/lambda; // already in correct units



          /// added to try and see the copynumbers 




          
          h_hit_theta->fill(theta_hit/Units::degree,hit.eventHitWeight());
          h_hit_theta_true->fill(theta_true/Units::degree,hit.eventHitWeight());
          h_neutron_ekin_eff->fill(Utils::neutron_angstrom_to_meV(lambda),hit.eventHitWeight());
          if(lambda)         
            h_hit_dll->fill((lambda-Utils::neutron_meV_to_angstrom(ekin_true/Units::meV))/Utils::neutron_meV_to_angstrom(ekin_true/Units::meV), hit.eventHitWeight());
          if(theta_true)
            h_hit_dthth->fill((theta_hit-theta_true)/theta_true,hit.eventHitWeight());

          h_hit_phi->fill(phi_hit/Units::degree,hit.eventHitWeight());
          h_hit_tof->fill(hit.eventHitTime()/Units::ms,hit.eventHitWeight());
          h_hit_phivstheta->fill(theta_hit/Units::degree,phi_hit/Units::degree,hit.eventHitWeight());
          
          h_hit_xy->fill(hit.eventHitPositionX()/Units::cm,hit.eventHitPositionY()/Units::cm,hit.eventHitWeight());
          h_hit_x->fill(hit.eventHitPositionX()/Units::cm,hit.eventHitWeight());
          h_hit_y->fill(hit.eventHitPositionY()/Units::cm,hit.eventHitWeight());
          h_hit_x_true->fill(step0->preGlobalArray()[0]/Units::cm,hit.eventHitWeight());
          h_hit_y_true->fill(step0->preGlobalArray()[1]/Units::cm,hit.eventHitWeight());

          //h_hit_z->fill(hit.eventHitPositionZ()/Units::cm,hit.eventHitWeight());
          h_hit_Q->fill(Q_hit,hit.eventHitWeight());
          h_hit_Q_true->fill(Q_true*Units::angstrom,hit.eventHitWeight());
          if(Q_true)
            h_hit_dQQ->fill((Q_hit-Q_true*Units::angstrom)/(Q_true*Units::angstrom));
        
          //double threshold = 120.;
          //printf("Number of daughters: %d\n", neutron->nDaughters());
         
          for(uint32_t i=0; i<neutron->nDaughters(); ++i) {
            auto daughter = neutron->getDaughter(i);
            //volumeCopyNumber(unsigned idepth=1);
            if(daughter->pdgCode()==1000020040 || daughter->pdgCode()==1000030070) {// alpha or Li7
              for(auto seg = daughter->segmentBegin(); seg!=daughter->segmentEnd(); ++seg) {
                if(seg->volumeName()=="CountingGas") { // && seg->eDep()/Units::keV>=threshold) {
                  printf("volume name, copyNr, parent volume name, copyNr: %s, %s, %d, %d \n", seg->volumeNameCStr(), seg->volumeNameCStr(6), seg->volumeCopyNumber(), seg->volumeCopyNumber(6));
                  //printf("daughter PDG, volume name, individual edep (keV) : %s %s %f\n", neutron->getDaughter(i)->pdgNameCStr(), seg->volumeNameCStr(), seg->eDep()/Units::keV);

                  edep_daughters += seg->eDep();
                  p->position[0] = hit.eventHitPositionX()/Units::cm;
                  p->position[1] = hit.eventHitPositionY()/Units::cm;
                  p->position[2] = hit.eventHitPositionZ()/Units::cm;
                  p->direction[0] = 0;
                  p->direction[1] = 0;
                  p->direction[2] = 1;
                  p->ekin = 0;
                  p->time = hit.eventHitTime()/Units::ms;
                  p->weight = hit.eventHitWeight();
                  p->pdgcode = 0;
                  /// adding together the copy number for the different components to make the "detector ID".
                  /// volumeCopyNumber() = gas pixel ; volumeCopyNumber(1) = convertor (don't include) ; volumeCopyNumber(2) = straw wall ; volumeCopyNumber(3) = emptytube (don't include) ;
                  /// volumeCopyNumber(4) = TubeWall ; volumeCopyNumber(5) = EmptyPackBox ; volumeCopyNumber(6) = EmptyPanelBox ; volumeCopyNumber(7) = world
                  p->userflags = seg->volumeCopyNumber()+seg->volumeCopyNumber(2)+seg->volumeCopyNumber(4)+seg->volumeCopyNumber(5)+seg->volumeCopyNumber(6);   /// userflags saved as hexidecimal 
                  mcpl_add_particle(f,p);
                } // end if for segments inside the CountingGas volume
              } // end for loop over segments of daughter
            } // end if for condition on alpha or Li7 daughter
          } // end for over neutron daughters
        } // end hit in event
      } // end if conversion condition
    }//end of loop over primary neutrons
  } //end of event loop
  mcpl_close_outfile(f);

  hc.saveToFile("bcsloki_sans",true);
    
  return 0;
}


