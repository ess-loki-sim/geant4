#include "GriffAnaUtils/All.hh"
#include "Core/Units.hh"
#include "Core/FPE.hh"
#include "Utils/ArrayMath.hh"
#include "Utils/NeutronMath.hh"
#include "SimpleHists/HistCollection.hh"
#include <string>
#include <cmath>
#include <iostream>
#include <fstream>
#include "MCPL/mcpl.h"

#include "G4GeoLoki/PixelatedBanks.hh"
//Griff analysis. See https://confluence.esss.lu.se/display/DG/Griff for more info.

#ifndef M_PI
#define M_PI 3.14159265358979323846 //  pi
#endif

/// /// TODOGEANTINO
bool getPrimaryNeutronAndGeantino(GriffDataReader &dr,
                                  const GriffDataRead::Track *&trk_neutron,
                                  const GriffDataRead::Track *&trk_geantino)
{
  if (dr.nPrimaryTracks() != 2)
    return false;
  trk_neutron = dr.primaryTrackBegin();
  trk_geantino = trk_neutron + 1;
  if (trk_neutron->pdgCode() != 2112)
    std::swap(trk_neutron, trk_geantino);
  if (trk_neutron->pdgCode() != 2112)
    return false;
  if (trk_geantino->pdgCode() != 999)
    return false;
  return true;
}

int main(int argc, char **argv)
{

  Core::catch_fpe();
  GriffDataReader dr(argc, argv);

  auto setup = dr.setup();
  auto &geo = setup->geo();
  //printf("QQQ=============  %s \n", geo.getName().c_str());
  if (geo.getName() != "G4GeoLoki/GeoBCSBanks" && geo.getName() != "G4GeoBCS/GeoLarmorBCSExperiment")
  {
    printf("Error: Wrong setup for this analysis\n");
    return 1;
  }

  setup->dump();

  SimpleHists::HistCollection hc;

  const double sampleDetectorDistance = setup->geo().getParameterDouble("rear_detector_distance_m") * Units::m;
  const int rearBankPixelNumber = 256;
  printf("HARDCODED rear bank pixel number for analysis: %d\n", rearBankPixelNumber);

  PixelatedBanks banks = PixelatedBanks(sampleDetectorDistance, rearBankPixelNumber);
  const int numberOfPixels = banks.getTotalNumberOfPixels();

  auto h_neutron_pixel_geantino = hc.book2D("Show pixels the geantinos entered", 256, 0, 256, numberOfPixels / 256, 0, numberOfPixels / 256, "h_neutron_pixel_geantino");
  h_neutron_pixel_geantino->setXLabel("Pixel ID along straw");
  h_neutron_pixel_geantino->setYLabel("Straw ID");

  auto h_neutron_pixel_geantino_masking = hc.book2D("Show pixels the geantinos entered (no mask transmission)", 256, 0, 256, numberOfPixels / 256, 0, numberOfPixels / 256, "h_neutron_pixel_geantino_masking");
  h_neutron_pixel_geantino_masking->setXLabel("Pixel ID along straw");
  h_neutron_pixel_geantino_masking->setYLabel("Straw ID");

  auto h_neutron_counters = hc.bookCounts("General neutron counters", "neutron_counters");
  auto countTestGeantino = h_neutron_counters->addCounter("all_geantino");
  auto countTestGeantinoAbsInMask = h_neutron_counters->addCounter("geantino_in_Mask");

  if (numberOfPixels != 1605632) {
    printf("Error: Wrong pixel number for this analysis\n");
    return 1;
  } 

  bool enteredArrayGeantino[1605632];                                                                    //array to find pixels the geantinos enter
  bool enteredArrayGeantino_masking[1605632];                                                            //array to find pixels the geantinos enter
  memset(enteredArrayGeantino, false, sizeof(enteredArrayGeantino[0]) * numberOfPixels);                 //set all values zero
  memset(enteredArrayGeantino_masking, false, sizeof(enteredArrayGeantino_masking[0]) * numberOfPixels); //set all values zero

  while (dr.loopEvents())
  {

    /// TODOGEANTINO
    const GriffDataRead::Track *trk_neutron;
    const GriffDataRead::Track *trk_geantino;
    //if (!getPrimaryNeutronAndGeantino(dr,trk_neutron,trk_geantino)) {
    if (getPrimaryNeutronAndGeantino(dr, trk_neutron, trk_geantino))
    {
      countTestGeantino += 1;

      bool geantinoAbsorbed = false;
      for (auto seg = trk_geantino->segmentBegin(); seg != trk_geantino->segmentEnd(); ++seg)
      {

        if (!geantinoAbsorbed && (seg->volumeName().find("BoronMask-") != std::string::npos || seg->volumeName() == "B4CPanel" || seg->volumeName() == "AlPanel"))
        {
          countTestGeantinoAbsInMask += 1;
          geantinoAbsorbed = true;
          //break;
        }
        else if (seg->volumeName() == "Converter")
        {
          const int strawId_conv = seg->volumeCopyNumber(1);
          const int tubeId_conv = seg->volumeCopyNumber(3);
          const int bankId_conv = seg->volumeCopyNumber(5);

          auto step = seg->lastStep();
          const int pixelId = banks.getPixelId(bankId_conv, tubeId_conv, strawId_conv, step->postGlobalX(), step->postGlobalY());

          //TESTING

          if (enteredArrayGeantino[pixelId] == false)
          { //only the first time
            h_neutron_pixel_geantino->fill(pixelId % 256, std::floor(pixelId / 256), 1);
            enteredArrayGeantino[pixelId] = true;
          }
          if (!geantinoAbsorbed && enteredArrayGeantino_masking[pixelId] == false)
          {
            h_neutron_pixel_geantino_masking->fill(pixelId % 256, std::floor(pixelId / 256), 1);
            enteredArrayGeantino_masking[pixelId] = true;
          }
        }
      }
    }
    /// TODOGEANTINO END
  }   //end of event loop

  hc.saveToFile("bcsloki_masking", true);

  int indexOffset = 11;
  std::ofstream maskFile;
  maskFile.open("maskFile.xml");
  maskFile << "<?xml version=\"1.0\"?>\n";
  maskFile << "<detector-masking>\n";
  maskFile << "\t<group>\n";
  maskFile << "\t\t<detids> ";

  for (int i = 0; i < numberOfPixels; i++)
  {
    if (enteredArrayGeantino_masking[i] == false)
    {
      maskFile << i + indexOffset << ", "; //TODO OFFSET?
    }
  }
  maskFile.seekp(-2, std::ios_base::cur); //Go back with the write pointer to override the last coma and space ", "
  maskFile << " </detids>\n";
  maskFile << "\t</group>\n";
  maskFile << "</detector-masking>";
  maskFile.close();

  return 0;
}
