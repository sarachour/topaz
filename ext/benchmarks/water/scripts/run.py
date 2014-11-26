#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Version: 0.1
#
# Example:
# 	water.py [--repeat n] water-o param random
# 	water.py --threads m [--repeat n] water-p.v1 param random

import os, os.path
import runnerbase

BENCHMARK = 'water'
APP_SEQ='./water-seq'
APP_PAR='./water-par'

def output_abstraction(status, stdout, stderr):
    outlines = stdout.split('\n')
    resultLines = [l for l in outlines if l.startswith('     ')]
    if len(resultLines) == 0: return ""
    lastline = resultLines.pop().strip().split()
    return " ".join(lastline[1:])

def get_sequential_suffix(argumentsList, prefix = None):
    inputStr = os.path.basename(argumentsList[0]).replace('LWI5.', '')
    return (prefix + '.' if prefix else '') + inputStr

def get_parallel_suffix(argumentsList, threads, prefix = None):
    inputStr = os.path.basename(argumentsList[0]).replace('LWI5.', '')
    return (prefix + '.' if prefix else '') + inputStr + '.' + str(threads)

def run_sequential(program, argumentsList, repetitions, reportSuffix = None, saveOutput = True, measureExperimentTime = True, append = False, options = None):
    suffix = get_sequential_suffix(argumentsList, reportSuffix)
    return runnerbase.run_sequential(program, argumentsList, repetitions, reportSuffix = suffix,
                                     outputAbstractionFunction = output_abstraction, 
                                     saveOutput = saveOutput, measureExperimentTime = measureExperimentTime, append=append, options = options
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
    runnerbase.runner_main(BENCHMARK, minProgramParameters = 2, maxProgramParameters = 2, 
                           seq_run_handler_f = lambda progArgs, options: run_sequential(APP_SEQ, progArgs, options.repeat, append = options.appendexp, options = options) , 
                           par_run_handler_f = lambda progArgs, options: run_parallel(APP_PAR, progArgs, options.repeat, options.threads, append = options.appendexp, options = options),  
                           analyze_handler_f = analyze, 
                           process_handler_f = runnerbase.runner_main_handler_none, 
                           sanity_check_f = lambda progArgs, options: runnerbase.sanity_check(APP_SEQ, APP_PAR, progArgs, outputAbstractionFunction = output_abstraction, options = options)
                           )    
