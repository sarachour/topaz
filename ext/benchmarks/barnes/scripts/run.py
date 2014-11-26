#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Version: 0.1
#
# Example:
# 	water.py [--repeat n] water-o param random
# 	water.py --threads m [--repeat n] water-p.v1 param random

import os, os.path
import runnerbase
import re

BENCHMARK = 'barnes'
APP_SEQ='./barnes-seq'
APP_PAR='./barnes-par'

def output_abstraction(status, stdout, stderr):
##ENERGY TOTALS
## MASS:        8192
## ENERGY:     (5848927.9466 12043418.1883 -6194490.2417) 
## KIN ENERGY: [(4.04028e+06 830183 936380 )(830183 3.95108e+06 714322 )(936380 714322 4.05205e+06 )]
## POT ENERGY: [(-2.07494e+06 -231136 -320490 )(-224751 -2.11078e+06 -263713 )(-322490 -244520 -2.08395e+06 )]
## AGGREGATE CM POS: ( -4.0001  -1.9947   0.0021) 
## AGGREGATE CM VEL: (  0.0109   0.0460   0.0091) 
## ANGULAR MOMENTUM: (-1162.2472 666.7073 -801.6823)

    outlines = stdout.split('\n')
    isEnd = False
    for l in outlines:
        if not isEnd: 
            if l.startswith("RESTTIME"): isEnd = True
            continue
        
        lr = l.replace('[', ' ').replace('(', ' ').replace(')', ' ').replace(']', ' ')
        ls = lr.split() 
        if l.startswith(" ENERGY:"):
            energy = ( ls[1], ls[2], ls[3] )
        elif l.startswith(" KIN ENERGY:"):
            kinenergy = ( ls[2], ls[3], ls[4], ls[5], ls[6], ls[7], ls[8], ls[9], ls[10] )
        elif l.startswith(" POT ENERGY:"):
            potenergy = ( ls[2], ls[3], ls[4], ls[5], ls[6], ls[7], ls[8], ls[9], ls[10] )
        elif l.startswith(" AGGREGATE CM POS:"):
            cmpos = ( ls[3], ls[4], ls[5] )
        elif l.startswith(" AGGREGATE CM VEL:"):
            cmvel = ( ls[3], ls[4], ls[5] )
        elif l.startswith(" ANGULAR MOMENTUM:"):
            angmom = ( ls[2], ls[3], ls[4] )
            
    
    return " ".join(energy) + "\t" + " ".join(kinenergy) + "\t" + " ".join(potenergy) + "\t" + " ".join(cmpos) + "\t" + " ".join(cmvel) + "\t" + " ".join(angmom) 
    
def get_sequential_suffix(argumentsList, prefix = None):
    inputStr = os.path.basename(argumentsList[0]).replace('input', '')
    return (prefix + '.' if prefix else '') + inputStr

def get_parallel_suffix(argumentsList, threads, prefix = None):
    inputStr = os.path.basename(argumentsList[0]).replace('input', '')
    return (prefix + '.' if prefix else '') + inputStr + '.' + str(threads)

def run_sequential(program, argumentsList, repetitions, reportSuffix = None, saveOutput = True, measureExperimentTime = True, append = False, options = None):
    suffix = get_sequential_suffix(argumentsList, reportSuffix)
    return runnerbase.run_sequential(program, argumentsList, repetitions, reportSuffix = suffix,
                                     outputAbstractionFunction = output_abstraction, 
                                     saveOutput = saveOutput, measureExperimentTime = measureExperimentTime, append=append
                                     )

def run_parallel(program, argumentsList, repetitions, threads, reportSuffix = None, maxExecTime=runnerbase.MAX_EXEC_TIME_SEC, saveOutput = True, measureExperimentTime = True, append = False, options = None):
    suffix = get_parallel_suffix(argumentsList, threads, reportSuffix) 
    return runnerbase.run_parallel(program, argumentsList, repetitions, threads, outputAbstractionFunction = output_abstraction, 
                                   reportSuffix = suffix, append = append, 
                                   measureExperimentTime = measureExperimentTime, saveOutput = saveOutput, options = options
                                   )

def analyze(progArgs, options):
    seqTimeFile = runnerbase.getSequentialTimeFilename(get_sequential_suffix(progArgs))
    seqOutFile = runnerbase.getSequentialOutputFilename(get_sequential_suffix(progArgs))
    parTimeFile = runnerbase.getParallelTimeFilename(get_parallel_suffix(progArgs, options.threads))
    parOutFile = runnerbase.getParallelOutputFilename(get_parallel_suffix(progArgs, options.threads))   
    return runnerbase.analyze_performance_distortion_extra(seqTimeFile, seqOutFile, parTimeFile, parOutFile, options.threads, doPrint = True)

if __name__ == '__main__':
    runnerbase.runner_main(BENCHMARK, minProgramParameters = 1, maxProgramParameters = 2, 
                           seq_run_handler_f = lambda progArgs, options: run_sequential(APP_SEQ, progArgs, options.repeat, append = options.appendexp, options = options) , 
                           par_run_handler_f = lambda progArgs, options: run_parallel(APP_PAR, progArgs, options.repeat, options.threads, append = options.appendexp, options = options),  
                           analyze_handler_f = analyze, 
                           process_handler_f = runnerbase.runner_main_handler_none, 
                           sanity_check_f = lambda progArgs, options: runnerbase.sanity_check(APP_SEQ, APP_PAR, progArgs, outputAbstractionFunction = output_abstraction, options = options)
                           )    
