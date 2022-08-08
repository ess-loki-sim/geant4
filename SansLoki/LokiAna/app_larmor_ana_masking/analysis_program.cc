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

// #include "G4GeoLoki/BcsBanks.hh"
//Griff analysis. See https://confluence.esss.lu.se/display/DG/Griff for more info.

#ifndef M_PI
#define M_PI 3.14159265358979323846 //  pi
#endif

const int strawPixelNumber = 512;

int getPositionPixelId(const int tubeId, const double positionX){
  const double strawLength = tubeId < 16 ? 1.5*Units::m : 1.2 *Units::m; //0-15 1.5m ; 16-31 1.2 m
  const double pixelLength = strawLength / strawPixelNumber;

  const double strawBegin = - 0.5* strawLength;
  const int invertedPixelId = std::floor((positionX - strawBegin) / pixelLength);

  return (strawPixelNumber - 1) - invertedPixelId; //pixels are numbered in minus x direction
}

int getPixelId(const int tubeId, const int strawId, const double positionX) {
  const int strawPixelOffset = (tubeId * 7 + strawId) * strawPixelNumber;
  const int positionPixelId = getPositionPixelId(tubeId, positionX);
  return strawPixelOffset + positionPixelId;
}

int main(int argc, char **argv)
{

  Core::catch_fpe();
  GriffDataReader dr(argc, argv);

  auto setup = dr.setup();
  auto &geo = setup->geo();
  //printf("QQQ=============  %s \n", geo.getName().c_str());
  if (geo.getName() != "G4GeoLoki/GeoBCSBanks" && geo.getName() != "G4GeoLoki/GeoLarmorBCSExperiment")
  {
    printf("Error: Wrong setup for this analysis\n");
    return 1;
  }

  setup->dump();

  GriffAnaUtils::TrackIterator primary_geantinos(&dr);
  primary_geantinos.addFilter(new GriffAnaUtils::TrackFilter_Primary());
  primary_geantinos.addFilter(new GriffAnaUtils::TrackFilter_PDGCode(999));

  SimpleHists::HistCollection hc;

  //const double sampleDetectorDistance = setup->geo().getParameterDouble("rear_detector_distance_m") * Units::m;

  const int numberOfPixels = 4 * 8 * 7 * strawPixelNumber; //hardcoded: 4(pack) * 8(tube) * 7(straw) * 512(pixel)

  auto h_neutron_pixel_geantino = hc.book2D("Show pixels the geantinos entered", strawPixelNumber, 0, strawPixelNumber, numberOfPixels / strawPixelNumber, 0, numberOfPixels / strawPixelNumber, "h_neutron_pixel_geantino");
  h_neutron_pixel_geantino->setXLabel("Pixel ID along straw");
  h_neutron_pixel_geantino->setYLabel("Straw ID");

  auto h_neutron_pixel_geantino_masking = hc.book2D("Show pixels the geantinos entered (no mask transmission)", strawPixelNumber, 0, strawPixelNumber, numberOfPixels / strawPixelNumber, 0, numberOfPixels / strawPixelNumber, "h_neutron_pixel_geantino_masking");
  h_neutron_pixel_geantino_masking->setXLabel("Pixel ID along straw");
  h_neutron_pixel_geantino_masking->setYLabel("Straw ID");

  auto h_counters = hc.bookCounts("General neutron counters", "neutron_counters");
  auto countTestGeantino = h_counters->addCounter("all_geantino");
  auto countTestGeantinoAbsInMask = h_counters->addCounter("geantino_in_Mask");

  bool enteredArrayGeantino[numberOfPixels];  //array to find pixels the geantinos enter
  bool enteredArrayGeantino_masking[numberOfPixels];  //array to find pixels the geantinos enter
  memset(enteredArrayGeantino, false, sizeof(enteredArrayGeantino[0]) * numberOfPixels);  //set all values zero
  memset(enteredArrayGeantino_masking, false, sizeof(enteredArrayGeantino_masking[0]) * numberOfPixels); //set all values zero

  while (dr.loopEvents()) {
    while (auto trk_geantino = primary_geantinos.next()) {
      countTestGeantino += 1;

      bool geantinoAbsorbed = false;
      for (auto seg = trk_geantino->segmentBegin(); seg != trk_geantino->segmentEnd(); ++seg) {

        if (!geantinoAbsorbed && (seg->volumeName().find("BoronMask-") != std::string::npos || seg->volumeName() == "B4CPanel" || seg->volumeName() == "AlPanel")) {
          countTestGeantinoAbsInMask += 1;
          geantinoAbsorbed = true;
          //break;
        }
        else if (seg->volumeName() == "Converter") {
          const int strawId_conv = seg->volumeCopyNumber(1);
          const int tubeId_conv = seg->volumeCopyNumber(3);

          auto step = seg->lastStep();
          const int pixelId = getPixelId(tubeId_conv, strawId_conv, step->postGlobalX());

          if (enteredArrayGeantino[pixelId] == false) { //only the first time
            h_neutron_pixel_geantino->fill(pixelId % strawPixelNumber, std::floor(pixelId / strawPixelNumber), 1);
            enteredArrayGeantino[pixelId] = true;
          }
          if (!geantinoAbsorbed && enteredArrayGeantino_masking[pixelId] == false) {
            h_neutron_pixel_geantino_masking->fill(pixelId % strawPixelNumber, std::floor(pixelId / strawPixelNumber), 1);
            enteredArrayGeantino_masking[pixelId] = true;
          }
        }
      }
    }
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
