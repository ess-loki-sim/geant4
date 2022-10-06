from __future__ import print_function
import G4CustomPyGen
import Core.Units as Units
#import Utils.NeutronMath
#import math
import G4GeoLoki.LokiMaskingHelper as Mask

class CalibrationSlitSourceGen(G4CustomPyGen.GenBase):
    def declare_parameters(self):
        self.addParameterDouble("gen_x_offset_millimeters", 0.0)
        self.addParameterDouble("gen_x_width_meters", 0.0)
        self.addParameterDouble("gen_y_width_meters", 0.0)
        self.addParameterInt("aiming_straw_pixel_number", 512) #used only for aiming, NOT for analysis
        #self.addParameterDouble("z_width_meters", 0.001)

    def init_generator(self,gun):
        gun.set_type('geantino') #neutron

        self._i = -1 #count events to shoot neutrons at each pixel
        #self._i = 16384 * 160 -1 #skip most of the pixels over the vacuum tank endig top
        #self._i = 128*7 * (8 * 3 -4) - 1 #just for a figure
        #self._i = 512*7 * (8 * 3 -4  + 15*8) - 1 # 512*7 * (8 * 3 -4) - 1 #just for a figure

        #self._i = 16384 * (160+820) -1 #skip most of the pixels over the vacuum tank endig top
        #self._i = 980000 #below the vacuum tank endig bottom

        self.larmor2022experiment = self.getParameterBoolean('geo_larmor_2022_experiment')
        self.old_tube_numbering = self.getParameterBoolean('geo_old_tube_numbering')

        rearDetectorDistance = self.getParameterDouble('geo_rear_detector_distance_m') *Units.m
        self.aimHelper = Mask.MaskingHelper(rearDetectorDistance, self.aiming_straw_pixel_number)
        self.totalNumberOfPixels = self.aimHelper.getTotalNumberOfPixels()

    def generate_event(self,gun):
        self._i += 1        
        # Source position - 
        sourcePositionX = self.gen_x_offset_millimeters *Units.mm
        #sourcePositionX = -(self.gen_x_width_meters * 0.5) + self.rand() * self.gen_x_width_meters
        sourcePositionY = 0.0 *Units.mm
        #sourcePositionY = -(self.gen_y_width_meters * 0.5) + self.rand() * self.gen_y_width_meters
        sourcePositionZ = 0.0 *Units.mm
        gun.set_position(sourcePositionX , sourcePositionY , sourcePositionZ )

        # Direction - toward the centre of a pixel
        #bank pixel limits: 0, 401408, 516096, 602112, 716800, 802816, 1003520, 1232896, 1376256, 1605632
        pixelId = ((self._i + 0) % self.totalNumberOfPixels) 

        pixelCentreX, pixelCentreY, pixelCentreZ = self.aimHelper.getPixelCentrePositionsForMasking(pixelId, self.old_tube_numbering, self.larmor2022experiment)

        gun.set_direction(pixelCentreX - sourcePositionX, pixelCentreY - sourcePositionY, pixelCentreZ - sourcePositionZ)
        

