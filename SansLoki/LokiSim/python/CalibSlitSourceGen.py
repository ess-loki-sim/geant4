from __future__ import print_function
import G4CustomPyGen
import Core.Units as Units
#import Utils.NeutronMath
#import math
import G4GeoLoki.LokiMaskingHelper as Mask

class CalibrationSlitSourceGen(G4CustomPyGen.GenBase):
    #def __init__(self, rear_detector_distance_m):
    #    G4CustomPyGen.GenBase.__init__(self)
    #    self.rear_detector_distance_m = rear_detector_distance_m

    def declare_parameters(self):
        self.addParameterDouble("x_offset_millimeters", 0.0)
        self.addParameterDouble("x_width_meters", 0.0)
        self.addParameterDouble("y_width_meters", 0.0)
        #self.addParameterDouble("z_width_meters", 0.001)

    def init_generator(self,gun):
        gun.set_type('geantino') #neutron

        self._i = -1 #count events to shoot neutrons at each pixel
        #self._i = 16384 * 160 -1 #skip most of the pixels over the vacuum tank endig top
        #self._i = 128*7 * (8 * 3 -4) - 1 #just for a figure
        #self._i = 512*7 * (8 * 3 -4  + 15*8) - 1 # 512*7 * (8 * 3 -4) - 1 #just for a figure

        #self._i = 16384 * (160+820) -1 #skip most of the pixels over the vacuum tank endig top
        #self._i = 980000 #below the vacuum tank endig bottom
        ##self.aimHelper = Mask.bcsBanks(self.rear_detector_distance_m *Units.m) #doesn't work
        #self.aimHelper = Mask.bcsBanks(4.420 *Units.m)
        #self.aimHelper = Mask.bcsBanks(4.068 *Units.m)
        self.aimHelper = Mask.bcsBanks(4.099 *Units.m)
        self.totalNumberOfPixels = self.aimHelper.getTotalNumberOfPixels()

    def generate_event(self,gun):
        self._i += 1        

        # Source position - 
        sourcePositionX = self.x_offset_millimeters *Units.mm
        #sourcePositionX = -(self.x_width_meters * 0.5) + self.rand() * self.x_width_meters
        sourcePositionY = 0.0 *Units.mm
        #sourcePositionY = -(self.y_width_meters * 0.5) + self.rand() * self.y_width_meters
        sourcePositionZ = 0.0 *Units.mm
        gun.set_position(sourcePositionX , sourcePositionY , sourcePositionZ )

        # Direction - toward the centre of a pixel
        #bank pixel limits: 0, 401408, 516096, 602112, 716800, 802816, 1003520, 1232896, 1376256, 1605632
        pixelId = ((self._i + 0) % self.totalNumberOfPixels) 
        self.aimHelper.calcPixelCentrePositionForMasking(pixelId) # self._i + math.floor(self.rand()*256) + 401408  # 17*7*256 + 256
        
        pixelCentreX = self.aimHelper.getPixelCentrePosition(0)
        pixelCentreY = self.aimHelper.getPixelCentrePosition(1) -207.94792 #-214.975 # - 211.94792  #magical offset of the bank...
        pixelCentreZ = self.aimHelper.getPixelCentrePosition(2) 

        gun.set_direction(pixelCentreX - sourcePositionX, pixelCentreY - sourcePositionY, pixelCentreZ - sourcePositionZ)
        #gun.set_direction(pixelCentreX - sourcePositionX +3*(2*self.rand()-1), pixelCentreY - sourcePositionY+3*(2*self.rand()-1), pixelCentreZ - sourcePositionZ+3*(2*self.rand()-1))

