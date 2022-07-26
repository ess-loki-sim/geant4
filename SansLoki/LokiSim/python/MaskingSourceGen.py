from __future__ import print_function
import G4CustomPyGen
import Core.Units as Units
#import Utils.NeutronMath
#import math
import G4GeoLoki.LokiMaskingHelper as Mask

class MaskingSourceGen(G4CustomPyGen.GenBase):
    #def __init__(self, rear_detector_distance_m):
    #    G4CustomPyGen.GenBase.__init__(self)
    #    self.rear_detector_distance_m = rear_detector_distance_m

    def declare_parameters(self):
        self.addParameterDouble("x_width_meters", 0.0)
        self.addParameterDouble("y_width_meters", 0.0)
        self.addParameterInt("aiming_rear_bank_pixel_number", 512) #used only for aiming, NOT for analysis
        #self.addParameterDouble("z_width_meters", 0.001)

    def init_generator(self,gun):
        gun.set_type('geantino') #neutron

        self._i = -1 #count events to shoot neutrons at each pixel
        ##self.aimHelper = Mask.bcsBanks(self.rear_detector_distance_m *Units.m) #doesn't work
        #self.aimHelper = Mask.bcsBanks(5.0 *Units.m)
        self.aimHelper = Mask.MaskingHelper(4.099 *Units.m, self.aiming_rear_bank_pixel_number)
        self.totalNumberOfPixels = self.aimHelper.getTotalNumberOfPixels()

    def generate_event(self,gun):
        self._i += 1        

        # Source position - 
        sourcePositionX = self.x_width_meters * 0.5
        sourcePositionY = self.y_width_meters * 0.5
        sourcePositionZ = 0.0
        gun.set_position(sourcePositionX, sourcePositionY, sourcePositionZ)

        # Direction - toward the centre of a pixel
        #bank pixel limits: 0, 401408, 516096, 602112, 716800, 802816, 1003520, 1232896, 1376256, 1605632
        pixelId = ((self._i + 0) % self.totalNumberOfPixels) 
        self.aimHelper.calcPixelCentrePositionForMasking(pixelId, True) # self._i + math.floor(self.rand()*256) + 401408  # 17*7*256 + 256
        
        pixelCentreX = self.aimHelper.getPixelCentrePosition(0)
        pixelCentreY = self.aimHelper.getPixelCentrePosition(1) -207.94792 #-214.975 #-244.94792 #magical offset of the bank...
        pixelCentreZ = self.aimHelper.getPixelCentrePosition(2) 

        gun.set_direction(pixelCentreX - sourcePositionX, pixelCentreY - sourcePositionY, pixelCentreZ - sourcePositionZ)

