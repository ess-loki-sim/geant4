#!/usr/bin/env python

import os,sys
import subprocess

baseFolder = '/mnt/groupdata/detector/workspaces/mklausz/centos7/larmorSimulation'

## FLAT SAMPLE
#njobs, nevtsperjob, additionalNeutrons = (20000, 100000, 1) # 10736 00000  - 1670418672  withBS      
#mcplFile = baseFolder + '/mcstas/rearBankRealSetup_flat_20A_split1000_1e11_BS_newMonPos/larmor_postsample.mcpl.gz'
#outputdir= baseFolder + '/rearBankGeant4/rearBankRealSetup_flat_20A_split1000_1e11_BS' 

## SAMP 10 
#njobs, nevtsperjob, additionalNeutrons = (21126, 100000, 70927) # 2112670927   
#mcplFile = baseFolder + '/mcstas/rearBankRealSetup_samp10_2kA_split1000_1e11_BS_new/larmor_postsample.mcpl.gz'
#outputdir= baseFolder + '/rearBankGeant4/rearBankRealSetup_samp10_2kA_split1000_1e11_BS_new'    

## SAMP 18
#njobs, nevtsperjob, additionalNeutrons = (20000, 100000, 0) # 2e9 (first part)
#mcplFile = baseFolder + '/mcstas/rearBankRealSetup_samp18_20A_split1000_1e11_BS_new/larmor_postsample.mcpl.gz' 
#outputdir= baseFolder + '/rearBankGeant4/rearBankRealSetup_samp18_20A_split1000_1e11_BS_new'

#njobs, nevtsperjob, additionalNeutrons = (19315, 100000, 23679) # 1931523679 (second part)
#mcplFile = baseFolder + '/mcstas/rearBankRealSetup_samp18_20A_split1000_1e11_BS_new/larmor_postsample_after2e9.mcpl'
#outputdir= baseFolder + '/rearBankGeant4/rearBankRealSetup_samp18_20A_split1000_1e11_BS_new_after2e9'

## SANS-50


## EMPTY (Slit)
njobs, nevtsperjob, additionalNeutrons = (4799, 100000, 48055) # 479948055 - 4.8e8  
mcplFile = baseFolder + '/mcstas/rearBankRealSetup_empty_20mA_split1000_1e11_new/larmor_postsample.mcpl.gz'
outputdir= baseFolder + '/rearBankGeant4/rearBankRealSetup_empty_20mA_split1000_1e11_BS_new'

# for submitting only a part of the jobs
job_nr_min = 4000 
job_nr_max = 5000
if job_nr_max > njobs:
    job_nr_max = njobs

#queue='verylong'
queue='newlong'
#queue='short'
#queue='quark'

#cmdbase = [ 'ess_lokisim_simanachain_bcs_larmor'
cmdbase = [ 'ess_lokisim_simanachain_rear_bank_larmor'
            ,'results'
            ,'event_gen=mcpl'
            ,'allow_zero_weight=y'
            ,'rear_detector_distance_m=4.099'
            ,'rear_bank_pixel_number=512'
            ,'sample_generator_distance_meters=4.049'
            ,'larmor_2022_experiment=True'
            ,'rear_detector_only=True'
            ,'with_beamstop=False'
            ,'with_calibration_slits=False'
            ,'x_offset_meters=0.005'
            ,('input_file=' + mcplFile)
            ,'--cleanup'
            ]

#jobList = [*range(82,98, 1)]+[*range(137,153, 1)]+[*range(210,250, 1)]

for i in rang(enjobs-1): # for seeded events                                                                    
    #if i not in [3296]:
    #continue
    if  i < job_nr_min:
        continue
    if  i >= job_nr_max:
        continue

    cmd = cmdbase+['skip_events=%i'%(i*nevtsperjob),'-n%i'%nevtsperjob,'-s%i'%(7999954+483*i)]
    cmd = ['ess_dmscutils_submit',queue,os.path.join(outputdir,'job%05i'%i)]+cmd
    MyOut = subprocess.Popen(cmd,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT)
    stdout,stderr = MyOut.communicate()
    print(stdout)
    print(stderr)        

#####################################################
## LAST JOB with possible additional Neutron number##
#####################################################

if job_nr_max == njobs:
    i = njobs-1
    cmd = cmdbase+['skip_events=%i'%(i*nevtsperjob),'-n%i'%(nevtsperjob + additionalNeutrons),'-s%i'%(7999954+483*i)]
    cmd = ['ess_dmscutils_submit',queue,os.path.join(outputdir,'job%05i'%i)]+cmd
    
    MyOut = subprocess.Popen(cmd,
                             stdout=subprocess.PIPE,
                             stderr=subprocess.STDOUT)
    stdout,stderr = MyOut.communicate()
    print(stdout)
    print(stderr)
