#!/usr/bin/env python

import os,sys
#import Core.System as Sys
import subprocess

## TEST
baseFolder = '/mnt/groupdata/detector/workspaces/mklausz/centos7/larmorSimulation'

njobs, nevtsperjob = (200000, 100000) 
outputdir= baseFolder + '/rearBankGeant4/realSetupFloodSource'
#outputdir= baseFolder + '/rearBankGeant4/problematic_realSetupFloodSource'

# for submitting only a part of the jobs                                                                                                          
job_nr_min = 155000
job_nr_max = 156000
if job_nr_max > njobs:
    job_nr_max = njobs

queue='newlong'
#queue='short'
#queue='quark'

cmdbase = [ 'ess_lokisim_simanachain_rear_bank_larmor'
            ,'results'
            ,'rear_detector_distance_m=4.099'
            ,'straw_pixel_number=512'
            ,'rear_detector_only=True'
            ,'with_beamstop=False'
            ,'larmor_2022_experiment=True'
            ,'event_gen=flood'
            ,'x_offset_meters=0.005'
            ,'--cleanup'
            ]

#jobList = [*range(82,98, 1)]+[*range(137,153, 1)]+[*range(210,250, 1)]

for i in range(njobs): 
    #if i not in [145111]:
    #    continue
    if  i < job_nr_min:
        continue
    if  i >= job_nr_max:
        continue

    cmd = cmdbase+['-n%i'%nevtsperjob,'-s%i'%(7999954+483*i)]
    cmd = ['ess_dmscutils_submit',queue,os.path.join(outputdir,'job%06i'%i)]+cmd
    MyOut = subprocess.Popen(cmd,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT)
    stdout,stderr = MyOut.communicate()
    print(stdout)
    print(stderr)        
