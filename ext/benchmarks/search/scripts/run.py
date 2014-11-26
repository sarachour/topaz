#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Version: 0.1
#
# Example:
# 	run.py [--repeat n] water-o param random
# 	run.py --threads m [--repeat n] water-p.v1 param random

import os, os.path
import runnerbase
import re

BENCHMARK = 'search'
APP_SEQ='./search-seq'
APP_PAR='./search-par'

# Pattern is following:
#
#
#End of fitting process
#
#final parameter set: 
# 6.291456e-01  1.056723e+01  4.480000e+00  1.700000e+00 
#
# Corresponding rmse: 0.332626 
#
def output_abstraction(status, stdout, stderr):

    outlines = stdout.split('\n')
    isEnd = False
    for l in outlines:
        if l.startswith("final parameter set:"): isEnd = True; continue
        if isEnd: r = l; isEnd = False
        if l.startswith(" Corresponding rmse:"): r += " " + l.split(':')[1]
    return r
    
def get_sequential_suffix(argumentsList, prefix = None):
    if len(argumentsList) == 1:
        inputStr=argumentsList[0]
    else:
        inputStr = os.path.basename(argumentsList[2]).replace('param.', '').replace('.dat', '')
    return (prefix + '.' if prefix else '') + inputStr

def get_parallel_suffix(argumentsList, threads, prefix = None):
    if len(argumentsList) == 1:
        inputStr=argumentsList[0]
    else:
        inputStr = os.path.basename(argumentsList[2]).replace('param.', '').replace('.dat', '')
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
                                   measureExperimentTime = measureExperimentTime, saveOutput = saveOutput
                                   )

def analyze(progArgs, options):
    seqTimeFile = runnerbase.getSequentialTimeFilename(get_sequential_suffix(progArgs))
    seqOutFile = runnerbase.getSequentialOutputFilename(get_sequential_suffix(progArgs))
    parTimeFile = runnerbase.getParallelTimeFilename(get_parallel_suffix(progArgs, options.threads))
    parOutFile = runnerbase.getParallelOutputFilename(get_parallel_suffix(progArgs, options.threads))   
    return runnerbase.analyze_performance_distortion_extra(seqTimeFile, seqOutFile, parTimeFile, parOutFile, options.threads, doPrint = True)

if __name__ == '__main__':
    runnerbase.runner_main(BENCHMARK, minProgramParameters = 3, maxProgramParameters = 3, 
                           seq_run_handler_f = lambda progArgs, options: run_sequential(APP_SEQ, progArgs, options.repeat, append = options.appendexp, options = options) , 
                           par_run_handler_f = lambda progArgs, options: run_parallel(APP_PAR, progArgs, options.repeat, options.threads, append = options.appendexp, options = options),  
                           analyze_handler_f = analyze, 
                           process_handler_f = runnerbase.runner_main_handler_none, 
                           sanity_check_f = lambda progArgs, options: runnerbase.sanity_check(APP_SEQ, APP_PAR, progArgs, outputAbstractionFunction = output_abstraction, options = options)
                           )    
