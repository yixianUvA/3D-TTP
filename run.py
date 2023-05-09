import runlib
import sys, os

def example2():
    for benchmark in (
                      'parsec-blackscholes',
                      #'parsec-bodytrack',
                      #'parsec-canneal',
                      #'parsec-dedup',
                      #'parsec-fluidanimate',
                      #'parsec-streamcluster',
                      #'parsec-swaptions',
                      #'parsec-x264',
                      #'splash2-barnes',
                      #'splash2-fmm',
                      #'splash2-ocean.cont',
                      #'splash2-ocean.ncont',
                      #'splash2-radiosity',
                      #'splash2-raytrace',
                      #'splash2-water.nsq',
                      #'splash2-water.sp',
                      #'splash2-cholesky',
                      #'splash2-fft',
                      #'splash2-lu.cont',
                      #'splash2-lu.ncont',
                      #'splash2-radix'
                      ):
        min_parallelism = runlib.get_feasible_parallelisms(benchmark)[0]
        max_parallelism = runlib.get_feasible_parallelisms(benchmark)[-1]
        for freq in (1, 2, 3, 4):
            for parallelism in (min_parallelism, max_parallelism):
                # you can also use try_run instead
                runlib.run(['open', '{:.1f}GHz'.format(freq), 'constFreq'], runlib.get_instance(benchmark, parallelism, input_set='simsmall'))


def example():
    for freq in (1, 2, 3, 4):  # when adding a new frequency level, make sure that it is also added in base.cfg
        runlib.run(['open', '{:.1f}GHz'.format(freq), 'constFreq'], 'parsec-blackscholes-simmedium-15')

def eval_seg():
    freq = '20000000'
    for benchmark in (
                      'parsec-blackscholes',
                      'parsec-bodytrack',
                      'parsec-canneal',
                      'parsec-dedup',
                      'parsec-fluidanimate',
                      'parsec-streamcluster',
                      'parsec-swaptions',
                      'parsec-x264',
                      'splash2-barnes',
                      'splash2-fmm',
                      'splash2-ocean.cont',
                      'splash2-ocean.ncont',
                      'splash2-radiosity',
                      'splash2-raytrace',
                      'splash2-water.nsq',
                      'splash2-water.sp',
                      'splash2-cholesky',
                      'splash2-fft',
                      'splash2-lu.cont',
                      'splash2-lu.ncont',
                      'splash2-radix'
                      ):
        runlib.run(['open', 'tsp','exact', freq], runlib.get_instance(benchmark, parallelism=4, input_set='simsmall')
                   +','+runlib.get_instance(benchmark, parallelism=4, input_set='simsmall')
                   +','+runlib.get_instance(benchmark, parallelism=4, input_set='simsmall')
                   +','+runlib.get_instance(benchmark, parallelism=4, input_set='simsmall')) 
        runlib.run(['open', 'ttsp','exact', freq], runlib.get_instance(benchmark, parallelism=4, input_set='simsmall')
                   +','+runlib.get_instance(benchmark, parallelism=4, input_set='simsmall')
                   +','+runlib.get_instance(benchmark, parallelism=4, input_set='simsmall')
                   +','+runlib.get_instance(benchmark, parallelism=4, input_set='simsmall')) 
    runlib.run(['open', 'tsp','exact',freq], runlib.get_instance( 'parsec-fluidanimate', parallelism=5, input_set='simsmall')
                +','+runlib.get_instance( 'parsec-fluidanimate', parallelism=5, input_set='simsmall')
                +','+runlib.get_instance( 'parsec-fluidanimate', parallelism=5, input_set='simsmall')) 
    runlib.run(['open', 'ttsp','exact', freq], runlib.get_instance('parsec-fluidanimate', parallelism=5, input_set='simsmall')
                +','+runlib.get_instance('parsec-fluidanimate', parallelism=5, input_set='simsmall')
                +','+runlib.get_instance('parsec-fluidanimate', parallelism=5, input_set='simsmall'))
 
def eval_seg():
    freq = '2000000'
    for benchmark in (
                      'parsec-blackscholes',
                      'parsec-bodytrack',
                      'parsec-canneal',
                      'parsec-dedup',
                      'parsec-fluidanimate',
                      'parsec-streamcluster',
                      'parsec-swaptions',
                      'parsec-x264',
                      'splash2-barnes',
                      'splash2-fmm',
                      'splash2-ocean.cont',
                      'splash2-ocean.ncont',
                      'splash2-radiosity',
                      'splash2-raytrace',
                      'splash2-water.nsq',
                      'splash2-water.sp',
                      'splash2-cholesky',
                      'splash2-fft',
                      'splash2-lu.cont',
                      'splash2-lu.ncont',
                      'splash2-radix'
                      ):
        runlib.run(['open', 'tsp','exact', freq], runlib.get_instance(benchmark, parallelism=4, input_set='simsmall')
                  ) 
        runlib.run(['open', 'ttsp','exact', freq], runlib.get_instance(benchmark, parallelism=4, input_set='simsmall')
                ) 
    runlib.run(['open', 'tsp','exact',freq], runlib.get_instance('parsec-fluidanimate', parallelism=5, input_set='simsmall')) 
    runlib.run(['open', 'ttsp','exact', freq], runlib.get_instance('parsec-fluidanimate', parallelism=5, input_set='simsmall'))


def eval_seg1():
    freq = '5000000'
    for benchmark in (
                      'parsec-blackscholes',
                      'parsec-bodytrack',
                      'parsec-canneal'
                      ):
        runlib.run(['open', 'ondemand','exact', freq], runlib.get_instance(benchmark, parallelism=4, input_set='simsmall')
                   +','+runlib.get_instance(benchmark, parallelism=4, input_set='simsmall')
                   +','+runlib.get_instance(benchmark, parallelism=4, input_set='simsmall')
                   +','+runlib.get_instance(benchmark, parallelism=4, input_set='simsmall')
                   )
        runlib.run(['open', 'date15','exact', freq], runlib.get_instance(benchmark, parallelism=4, input_set='simsmall')
                   +','+runlib.get_instance(benchmark, parallelism=4, input_set='simsmall')
                   +','+runlib.get_instance(benchmark, parallelism=4, input_set='simsmall')
                   +','+runlib.get_instance(benchmark, parallelism=4, input_set='simsmall')
                   ) 
    runlib.run(['open', 'date15','exact', freq], runlib.get_instance(benchmark, parallelism=5, input_set='simsmall')
                   +','+runlib.get_instance(benchmark, parallelism=5, input_set='simsmall')
                   +','+runlib.get_instance(benchmark, parallelism=3, input_set='simsmall')
                   +','+runlib.get_instance(benchmark, parallelism=3, input_set='simsmall')
                   ) 
    runlib.run(['open', 'ondemand','exact', freq], runlib.get_instance(benchmark, parallelism=5, input_set='simsmall')
                   +','+runlib.get_instance(benchmark, parallelism=5, input_set='simsmall')
                   +','+runlib.get_instance(benchmark, parallelism=3, input_set='simsmall')
                   +','+runlib.get_instance(benchmark, parallelism=3, input_set='simsmall')
                   ) 
         
def eval_seg2():
    freq = '5000000'
    for benchmark in (
                      'parsec-streamcluster',
                      'parsec-swaptions',
                      'parsec-x264',
                      'parsec-dedup',
                      ):
        runlib.run(['open', 'ondemand','exact', freq], runlib.get_instance(benchmark, parallelism=4, input_set='simsmall')
                   +','+runlib.get_instance(benchmark, parallelism=4, input_set='simsmall')
                   +','+runlib.get_instance(benchmark, parallelism=4, input_set='simsmall')
                   +','+runlib.get_instance(benchmark, parallelism=4, input_set='simsmall')
                   )
        runlib.run(['open', 'date15','exact', freq], runlib.get_instance(benchmark, parallelism=4, input_set='simsmall')
                   +','+runlib.get_instance(benchmark, parallelism=4, input_set='simsmall')
                   +','+runlib.get_instance(benchmark, parallelism=4, input_set='simsmall')
                   +','+runlib.get_instance(benchmark, parallelism=4, input_set='simsmall')
                   )  
def eval_seg3():
    freq = '5000000'
    for benchmark in (
                      'splash2-ocean.cont',
                      'splash2-ocean.ncont',
                      'splash2-radiosity',
                      'splash2-raytrace'
                      ):
        runlib.run(['open', 'ondemand','exact', freq], runlib.get_instance(benchmark, parallelism=4, input_set='small')
                   +','+runlib.get_instance(benchmark, parallelism=4, input_set='small')
                   +','+runlib.get_instance(benchmark, parallelism=4, input_set='small')
                   +','+runlib.get_instance(benchmark, parallelism=4, input_set='small')
                   )
        runlib.run(['open', 'date15','exact', freq], runlib.get_instance(benchmark, parallelism=4, input_set='small')
                   +','+runlib.get_instance(benchmark, parallelism=4, input_set='small')
                   +','+runlib.get_instance(benchmark, parallelism=4, input_set='small')
                   +','+runlib.get_instance(benchmark, parallelism=4, input_set='small')
                   )  


def eval_seg4():
    freq = '5000000'
    for benchmark in (
                      'splash2-lu.cont',
                      'splash2-lu.ncont',
                      'splash2-radix',
                      'splash2-barnes',
                      
                      ):
        runlib.run(['open', 'ondemand','exact', freq], runlib.get_instance(benchmark, parallelism=4, input_set='small')
                   +','+runlib.get_instance(benchmark, parallelism=4, input_set='small')
                   +','+runlib.get_instance(benchmark, parallelism=4, input_set='small')
                   +','+runlib.get_instance(benchmark, parallelism=4, input_set='small')
                   )
        runlib.run(['open', 'date15','exact', freq], runlib.get_instance(benchmark, parallelism=4, input_set='small')
                   +','+runlib.get_instance(benchmark, parallelism=4, input_set='small')
                   +','+runlib.get_instance(benchmark, parallelism=4, input_set='small')
                   +','+runlib.get_instance(benchmark, parallelism=4, input_set='small')
                   )     




def eval_seg5():
    freq = '5000000'
    for benchmark in (
                      'splash2-water.nsq',
                      'splash2-water.sp',
                      'splash2-cholesky',
                      'splash2-fft',
                      'splash2-fmm',
                      ):
        runlib.run(['open', 'ondemand','exact', freq], runlib.get_instance(benchmark, parallelism=4, input_set='small')
                   +','+runlib.get_instance(benchmark, parallelism=4, input_set='small')
                   +','+runlib.get_instance(benchmark, parallelism=4, input_set='small')
                   +','+runlib.get_instance(benchmark, parallelism=4, input_set='small')
                   )
        runlib.run(['open', 'date15','exact', freq], runlib.get_instance(benchmark, parallelism=4, input_set='small')
                   +','+runlib.get_instance(benchmark, parallelism=4, input_set='small')
                   +','+runlib.get_instance(benchmark, parallelism=4, input_set='small')
                   +','+runlib.get_instance(benchmark, parallelism=4, input_set='small')
                   )     
    
    

def eval_seg6():
    freq = '5000000'
    for benchmark in (
                      'splash2-lu.cont',
                      'splash2-lu.ncont',
                      'splash2-radix'
                      ):
        #runlib.run(['open', 'tsp','exact', freq], runlib.get_instance(benchmark, parallelism=4, input_set='small')) 
        runlib.run(['open', 'ttsp','exact', freq], runlib.get_instance(benchmark, parallelism=4, input_set='small')) 

def eval_seg7():
    #runlib.run(['open', 'ttsp','exact', '5000000'], runlib.get_instance('splash2-radix', parallelism=4, input_set='small')) 
    runlib.run(['open', 'date15','5000000'], runlib.get_instance('splash2-radix', parallelism=16, input_set='small')) 
    runlib.run(['open', 'ondemand','exact', '5000000'], runlib.get_instance('splash2-radix', parallelism=16, input_set='small')) 

def eval_seg7():
    #runlib.run(['open', 'ondemand','exact', '20000000'], runlib.get_instance('parsec-blackscholes', parallelism=16, input_set='small')) 
    # runlib.run(['open', 'date15','5000000'], runlib.get_instance('parsec-blackscholes', parallelism=4, input_set='small')
    #            +','+runlib.get_instance('parsec-blackscholes', parallelism=4, input_set='small')
    #            +','+runlib.get_instance('parsec-blackscholes', parallelism=4, input_set='small')
    #            +','+runlib.get_instance('parsec-blackscholes', parallelism=4, input_set='small')) 
    # runlib.run(['open', 'ondemand','exact', '5000000'], runlib.get_instance('parsec-blackscholes', parallelism=4, input_set='small')
    #            +','+runlib.get_instance('parsec-blackscholes', parallelism=4, input_set='small')
    #            +','+runlib.get_instance('parsec-blackscholes', parallelism=4, input_set='small')
    #            +','+runlib.get_instance('parsec-blackscholes', parallelism=4, input_set='small')) 
    freq = '5000000'
    for benchmark in (
                      'parsec-blackscholes',
                      ):
        runlib.run(['open', 'ondemand','exact', freq], runlib.get_instance(benchmark, parallelism=4, input_set='simsmall')
                   +','+runlib.get_instance(benchmark, parallelism=4, input_set='simsmall')
                   +','+runlib.get_instance(benchmark, parallelism=4, input_set='simsmall')
                   +','+runlib.get_instance(benchmark, parallelism=4, input_set='simsmall')
                   )
        runlib.run(['open', 'date15','exact', freq], runlib.get_instance(benchmark, parallelism=4, input_set='simsmall')
                   +','+runlib.get_instance(benchmark, parallelism=4, input_set='simsmall')
                   +','+runlib.get_instance(benchmark, parallelism=4, input_set='simsmall')
                   +','+runlib.get_instance(benchmark, parallelism=4, input_set='simsmall')
                   )
def case_study():
    runlib.run(['open', 'tsp','exact', '1000000'], runlib.get_instance('parsec-blackscholes', parallelism=2, input_set='simsmall')) 
    #runlib.run(['open', 'tsp','exact', '20000000'], runlib.get_instance('splash2-radix', parallelism=1, input_set='small')) 
    #runlib.run(['open', 'ttsp','exact', '20000000'], runlib.get_instance('splash2-radix', parallelism=1, input_set='small')) 
    #runlib.run(['open', 'ondemand','exact', '20000000'], runlib.get_instance('splash2-radix', parallelism=1, input_set='small')) 
    #runlib.run(['open', 'tsp','exact', '20000000'], runlib.get_instance('splash2-barnes', parallelism=1, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=1, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=1, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=1, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=1, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=1, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=1, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=1, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=1, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=1, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=1, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=1, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=1, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=1, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=1, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=1, input_set='large')) 
    #runlib.run(['open', 'ttsp','exact', '20000000'], runlib.get_instance('splash2-barnes', parallelism=1, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=1, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=1, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=1, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=1, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=1, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=1, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=1, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=1, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=1, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=1, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=1, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=1, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=1, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=1, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=1, input_set='large')) 
    #runlib.run(['open', 'ondemand', '20000000'], runlib.get_instance('splash2-barnes', parallelism=1, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=1, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=1, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=1, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=1, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=1, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=1, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=1, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=1, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=1, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=1, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=1, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=1, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=1, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=1, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=1, input_set='large')) 
     
    # runlib.run(['open', 'tsp','exact', '20000000'], runlib.get_instance('parsec-blackscholes', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-blackscholes', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-blackscholes', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-blackscholes', parallelism=4, input_set='simmedium')) 
    # runlib.run(['open', 'ttsp','exact', '20000000'], runlib.get_instance('parsec-blackscholes', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-blackscholes', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-blackscholes', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-blackscholes', parallelism=4, input_set='simmedium'))
    # runlib.run(['open', 'ondemand', '20000000'], runlib.get_instance('parsec-blackscholes', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-blackscholes', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-blackscholes', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-blackscholes', parallelism=4, input_set='simmedium'))
    # runlib.run(['open', 'ttsp','exact', '20000000'], runlib.get_instance('parsec-bodytrack', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-bodytrack', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-bodytrack', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-bodytrack', parallelism=4, input_set='simmedium'))
    # runlib.run(['open', 'tsp','exact', '20000000'], runlib.get_instance('parsec-bodytrack', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-bodytrack', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-bodytrack', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-bodytrack', parallelism=4, input_set='simmedium'))
    # runlib.run(['open', 'ondemand', '20000000'], runlib.get_instance('parsec-bodytrack', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-bodytrack', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-bodytrack', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-bodytrack', parallelism=4, input_set='simmedium'))
    # runlib.run(['open', 'ttsp','exact', '20000000'], runlib.get_instance('parsec-fluidanimate', parallelism=5, input_set='simmedium')+','+runlib.get_instance('parsec-fluidanimate', parallelism=5, input_set='simmedium')+','+runlib.get_instance('parsec-fluidanimate', parallelism=5, input_set='simmedium'))
    # runlib.run(['open', 'tsp','exact', '20000000'], runlib.get_instance('parsec-fluidanimate', parallelism=5, input_set='simmedium')+','+runlib.get_instance('parsec-fluidanimate', parallelism=5, input_set='simmedium')+','+runlib.get_instance('parsec-fluidanimate', parallelism=5, input_set='simmedium'))
    # runlib.run(['open', 'ondemand', '20000000'], runlib.get_instance('parsec-fluidanimate', parallelism=5, input_set='simmedium')+','+runlib.get_instance('parsec-fluidanimate', parallelism=5, input_set='simmedium')+','+runlib.get_instance('parsec-fluidanimate', parallelism=5, input_set='simmedium'))
   
    #runlib.run(['open', 'tsp','exact', '20000000'], runlib.get_instance('parsec-dedup', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-dedup', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-dedup', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-dedup', parallelism=4, input_set='simmedium')) 
    #runlib.run(['open', 'ttsp','exact', '20000000'], runlib.get_instance('parsec-dedup', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-dedup', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-dedup', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-dedup', parallelism=4, input_set='simmedium'))
    #runlib.run(['open', 'ondemand', '20000000'], runlib.get_instance('parsec-dedup', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-dedup', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-dedup', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-dedup', parallelism=4, input_set='simmedium')) 
    #runlib.run(['open', 'ttsp','exact', '20000000'], runlib.get_instance('parsec-swaptions', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-swaptions', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-swaptions', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-swaptions', parallelism=4, input_set='simmedium'))
    #runlib.run(['open', 'tsp','exact', '20000000'], runlib.get_instance('parsec-swaptions', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-swaptions', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-swaptions', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-swaptions', parallelism=4, input_set='simmedium'))
    #runlib.run(['open', 'ondemand', '20000000'], runlib.get_instance('parsec-swaptions', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-swaptions', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-swaptions', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-swaptions', parallelism=4, input_set='simmedium'))
    #runlib.run(['open', 'ttsp','exact', '20000000'], runlib.get_instance('parsec-streamcluster', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-streamcluster', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-streamcluster', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-streamcluster', parallelism=4, input_set='simmedium'))
    #runlib.run(['open', 'tsp','exact', '20000000'], runlib.get_instance('parsec-streamcluster', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-streamcluster', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-streamcluster', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-streamcluster', parallelism=4, input_set='simmedium'))
    # runlib.run(['open', 'ondemand', '20000000'], runlib.get_instance('parsec-streamcluster', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-streamcluster', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-streamcluster', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-streamcluster', parallelism=4, input_set='simmedium'))
    # #runlib.run(['open', 'ttsp','exact', '20000000'], runlib.get_instance('parsec-x264', parallelism=8, input_set='simmedium')+','+runlib.get_instance('parsec-x264', parallelism=8, input_set='simmedium'))
    # #runlib.run(['open', 'tsp','exact', '20000000'], runlib.get_instance('parsec-x264', parallelism=8, input_set='simmedium')+','+runlib.get_instance('parsec-x264', parallelism=8, input_set='simmedium'))
    # #runlib.run(['open', 'ondemand', '20000000'], runlib.get_instance('parsec-x264', parallelism=8, input_set='simmedium')+','+runlib.get_instance('parsec-x264', parallelism=8, input_set='simmedium'))
    # runlib.run(['open', 'ttsp','exact', '20000000'], runlib.get_instance('splash2-barnes', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=4, input_set='large'))
    # runlib.run(['open', 'tsp','exact', '20000000'], runlib.get_instance('splash2-barnes', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=4, input_set='large'))
    # runlib.run(['open', 'ondemand', '20000000'], runlib.get_instance('splash2-barnes', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=4, input_set='large'))
    # runlib.run(['open', 'ttsp','exact', '20000000'], runlib.get_instance('splash2-cholesky', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-cholesky', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-cholesky', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-cholesky', parallelism=4, input_set='large'))
    # runlib.run(['open', 'tsp','exact', '20000000'], runlib.get_instance('splash2-cholesky', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-cholesky', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-cholesky', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-cholesky', parallelism=4, input_set='large'))
    # runlib.run(['open', 'ondemand', '20000000'], runlib.get_instance('splash2-cholesky', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-cholesky', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-cholesky', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-cholesky', parallelism=4, input_set='large'))
    # runlib.run(['open', 'ttsp','exact', '20000000'], runlib.get_instance('splash2-fft', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-fft', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-fft', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-fft', parallelism=4, input_set='large'))
    # runlib.run(['open', 'tsp','exact', '20000000'], runlib.get_instance('splash2-fft', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-fft', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-fft', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-fft', parallelism=4, input_set='large'))
    # runlib.run(['open', 'ondemand', '20000000'], runlib.get_instance('splash2-fft', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-fft', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-fft', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-fft', parallelism=4, input_set='large'))
    # #runlib.run(['open', 'ttsp','exact', '20000000'], runlib.get_instance('splash2-fmm', parallelism=16, input_set='large'))
    # #runlib.run(['open', 'tsp','exact', '20000000'], runlib.get_instance('splash2-fmm', parallelism=16, input_set='large'))
    # #runlib.run(['open', 'ondemand', '20000000'], runlib.get_instance('splash2-fmm', parallelism=16, input_set='large'))
    # runlib.run(['open', 'ttsp','exact', '20000000'], runlib.get_instance('splash2-lu.cont', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-lu.cont', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-lu.cont', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-lu.cont', parallelism=4, input_set='large'))
    # runlib.run(['open', 'tsp','exact', '20000000'], runlib.get_instance('splash2-lu.cont', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-lu.cont', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-lu.cont', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-lu.cont', parallelism=4, input_set='large'))
    # runlib.run(['open', 'ondemand', '20000000'], runlib.get_instance('splash2-lu.cont', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-lu.cont', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-lu.cont', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-lu.cont', parallelism=4, input_set='large'))
    # runlib.run(['open', 'ttsp','exact', '20000000'], runlib.get_instance('splash2-ocean.cont', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-ocean.cont', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-ocean.cont', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-ocean.cont', parallelism=4, input_set='large'))
    # runlib.run(['open', 'tsp','exact', '20000000'], runlib.get_instance('splash2-ocean.cont', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-ocean.cont', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-ocean.cont', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-ocean.cont', parallelism=4, input_set='large'))
    # runlib.run(['open', 'ondemand', '20000000'], runlib.get_instance('splash2-ocean.cont', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-ocean.cont', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-ocean.cont', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-ocean.cont', parallelism=4, input_set='large'))
    # runlib.run(['open', 'ttsp','exact', '20000000'], runlib.get_instance('splash2-radiosity', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-radiosity', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-radiosity', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-radiosity', parallelism=4, input_set='large'))
    # runlib.run(['open', 'tsp','exact', '20000000'], runlib.get_instance('splash2-radiosity', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-radiosity', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-radiosity', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-radiosity', parallelism=4, input_set='large'))
    # runlib.run(['open', 'ondemand', '20000000'], runlib.get_instance('splash2-radiosity', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-radiosity', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-radiosity', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-radiosity', parallelism=4, input_set='large'))
    # runlib.run(['open', 'ttsp','exact', '20000000'], runlib.get_instance('splash2-radix', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-radix', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-radix', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-radix', parallelism=4, input_set='large'))
    # runlib.run(['open', 'tsp','exact', '20000000'], runlib.get_instance('splash2-radix', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-radix', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-radix', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-radix', parallelism=4, input_set='large'))
    # runlib.run(['open', 'ondemand', '20000000'], runlib.get_instance('splash2-radix', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-radix', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-radix', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-radix', parallelism=4, input_set='large'))
    # runlib.run(['open', 'ttsp','exact', '20000000'], runlib.get_instance('splash2-water.nsq', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-water.nsq', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-water.nsq', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-water.nsq', parallelism=4, input_set='large'))
    # runlib.run(['open', 'tsp','exact', '20000000'], runlib.get_instance('splash2-water.nsq', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-water.nsq', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-water.nsq', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-water.nsq', parallelism=4, input_set='large'))
    # runlib.run(['open', 'ondemand', '20000000'], runlib.get_instance('splash2-water.nsq', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-water.nsq', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-water.nsq', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-water.nsq', parallelism=4, input_set='large'))

    # runlib.run(['open', 'ttsp','exact', '10000000'], runlib.get_instance('parsec-blackscholes', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-blackscholes', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-blackscholes', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-blackscholes', parallelism=4, input_set='simmedium'))
    # runlib.run(['open', 'ttsp','exact', '10000000'], runlib.get_instance('parsec-bodytrack', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-bodytrack', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-bodytrack', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-bodytrack', parallelism=4, input_set='simmedium'))
    # runlib.run(['open', 'ttsp','exact', '10000000'], runlib.get_instance('parsec-fluidanimate', parallelism=5, input_set='simmedium')+','+runlib.get_instance('parsec-fluidanimate', parallelism=5, input_set='simmedium')+','+runlib.get_instance('parsec-fluidanimate', parallelism=5, input_set='simmedium'))
    
    ##runlib.run(['open', 'ttsp','exact', '1000000'], runlib.get_instance('parsec-dedup', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-dedup', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-dedup', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-dedup', parallelism=4, input_set='simmedium'))
    #runlib.run(['open', 'ttsp','exact', '10000000'], runlib.get_instance('parsec-swaptions', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-swaptions', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-swaptions', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-swaptions', parallelism=4, input_set='simmedium'))
    #runlib.run(['open', 'ttsp','exact', '10000000'], runlib.get_instance('parsec-streamcluster', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-streamcluster', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-streamcluster', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-streamcluster', parallelism=4, input_set='simmedium'))
    #runlib.run(['open', 'ttsp','exact', '10000000'], runlib.get_instance('splash2-barnes', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=4, input_set='large'))
    #runlib.run(['open', 'ttsp','exact', '10000000'], runlib.get_instance('splash2-cholesky', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-cholesky', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-cholesky', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-cholesky', parallelism=4, input_set='large'))
    #runlib.run(['open', 'ttsp','exact', '10000000'], runlib.get_instance('splash2-fft', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-fft', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-fft', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-fft', parallelism=4, input_set='large'))
    #runlib.run(['open', 'ttsp','exact', '10000000'], runlib.get_instance('splash2-lu.cont', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-lu.cont', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-lu.cont', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-lu.cont', parallelism=4, input_set='large'))
    #runlib.run(['open', 'ttsp','exact', '10000000'], runlib.get_instance('splash2-ocean.cont', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-ocean.cont', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-ocean.cont', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-ocean.cont', parallelism=4, input_set='large'))
    #runlib.run(['open', 'ttsp','exact', '20000000'], runlib.get_instance('splash2-lu.cont', parallelism=2, input_set='large')+','+runlib.get_instance('splash2-lu.cont', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-lu.cont', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-lu.cont', parallelism=6, input_set='large'))
    #runlib.run(['open', 'ondemand','exact', '20000000'], runlib.get_instance('splash2-lu.cont', parallelism=2, input_set='large')+','+runlib.get_instance('splash2-lu.cont', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-lu.cont', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-lu.cont', parallelism=6, input_set='large'))
    #runlib.run(['open', 'tsp','exact', '20000000'], runlib.get_instance('splash2-lu.cont', parallelism=2, input_set='large')+','+runlib.get_instance('splash2-lu.cont', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-lu.cont', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-lu.cont', parallelism=6, input_set='large'))

    #runlib.run(['open', 'ttsp','exact', '10000000'], runlib.get_instance('splash2-lu.cont', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-fft', parallelism=4, input_set='large')+','+runlib.get_instance('parsec-swaptions', parallelism=4, input_set='simmedium')+','+runlib.get_instance('splash2-radiosity', parallelism=4, input_set='large'))
    #runlib.run(['open', 'ondemand','exact', '10000000'], runlib.get_instance('splash2-lu.cont', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-fft', parallelism=4, input_set='large')+','+runlib.get_instance('parsec-swaptions', parallelism=4, input_set='simmedium')+','+runlib.get_instance('splash2-radiosity', parallelism=4, input_set='large'))
 
    # runlib.run(['open', 'tsp','exact', '50000'], runlib.get_instance('parsec-blackscholes', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-blackscholes', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-blackscholes', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-blackscholes', parallelism=4, input_set='simmedium'))
    # runlib.run(['open', 'ttsp','exact', '50000'], runlib.get_instance('parsec-blackscholes', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-blackscholes', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-blackscholes', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-blackscholes', parallelism=4, input_set='simmedium'))
    # runlib.run(['open', 'ondemand', '50000'], runlib.get_instance('parsec-blackscholes', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-blackscholes', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-blackscholes', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-blackscholes', parallelism=4, input_set='simmedium'))
    # runlib.run(['open', 'tsp','exact', '50000'], runlib.get_instance('parsec-bodytrack', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-bodytrack', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-bodytrack', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-bodytrack', parallelism=4, input_set='simmedium'))
    # runlib.run(['open', 'ttsp','exact', '50000'], runlib.get_instance('parsec-bodytrack', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-bodytrack', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-bodytrack', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-bodytrack', parallelism=4, input_set='simmedium'))
    # runlib.run(['open', 'ondemand', '50000'], runlib.get_instance('parsec-bodytrack', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-bodytrack', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-bodytrack', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-bodytrack', parallelism=4, input_set='simmedium'))
    # runlib.run(['open', 'tsp','exact', '50000'], runlib.get_instance('parsec-fluidanimate', parallelism=5, input_set='simmedium')+','+runlib.get_instance('parsec-fluidanimate', parallelism=5, input_set='simmedium')+','+runlib.get_instance('parsec-fluidanimate', parallelism=5, input_set='simmedium'))
    # runlib.run(['open', 'ttsp','exact', '50000'], runlib.get_instance('parsec-fluidanimate', parallelism=5, input_set='simmedium')+','+runlib.get_instance('parsec-fluidanimate', parallelism=5, input_set='simmedium')+','+runlib.get_instance('parsec-fluidanimate', parallelism=5, input_set='simmedium'))
    # runlib.run(['open', 'ondemand', '50000'], runlib.get_instance('parsec-fluidanimate', parallelism=5, input_set='simmedium')+','+runlib.get_instance('parsec-fluidanimate', parallelism=5, input_set='simmedium')+','+runlib.get_instance('parsec-fluidanimate', parallelism=5, input_set='simmedium'))
    
    # #runlib.run(['open', 'ttsp','exact', '50000'], runlib.get_instance('parsec-dedup', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-dedup', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-dedup', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-dedup', parallelism=4, input_set='simmedium'))
    # runlib.run(['open', 'ttsp','exact', '50000'], runlib.get_instance('parsec-swaptions', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-swaptions', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-swaptions', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-swaptions', parallelism=4, input_set='simmedium'))
    # runlib.run(['open', 'ttsp','exact', '50000'], runlib.get_instance('parsec-streamcluster', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-streamcluster', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-streamcluster', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-streamcluster', parallelism=4, input_set='simmedium'))
    # runlib.run(['open', 'ttsp','exact', '50000'], runlib.get_instance('splash2-barnes', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=4, input_set='large'))
    # runlib.run(['open', 'ttsp','exact', '50000'], runlib.get_instance('splash2-cholesky', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-cholesky', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-cholesky', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-cholesky', parallelism=4, input_set='large'))
    # runlib.run(['open', 'ttsp','exact', '50000'], runlib.get_instance('splash2-fft', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-fft', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-fft', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-fft', parallelism=4, input_set='large'))
    # runlib.run(['open', 'ttsp','exact', '50000'], runlib.get_instance('splash2-lu.cont', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-lu.cont', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-lu.cont', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-lu.cont', parallelism=4, input_set='large'))
    # runlib.run(['open', 'ttsp','exact', '50000'], runlib.get_instance('splash2-ocean.cont', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-ocean.cont', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-ocean.cont', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-ocean.cont', parallelism=4, input_set='large'))
    # runlib.run(['open', 'ttsp','exact', '50000'], runlib.get_instance('splash2-radiosity', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-radiosity', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-radiosity', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-radiosity', parallelism=4, input_set='large'))
    # runlib.run(['open', 'ttsp','exact', '50000'], runlib.get_instance('splash2-radix', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-radix', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-radix', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-radix', parallelism=4, input_set='large'))
    # runlib.run(['open', 'ttsp','exact', '50000'], runlib.get_instance('splash2-water.nsq', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-water.nsq', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-water.nsq', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-water.nsq', parallelism=4, input_set='large'))

    # runlib.run(['open', 'ttsp','exact', '0'], runlib.get_instance('parsec-blackscholes', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-blackscholes', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-blackscholes', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-blackscholes', parallelism=4, input_set='simmedium'))
    # runlib.run(['open', 'ttsp','exact', '0'], runlib.get_instance('parsec-bodytrack', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-bodytrack', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-bodytrack', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-bodytrack', parallelism=4, input_set='simmedium'))
    # runlib.run(['open', 'ttsp','exact', '0'], runlib.get_instance('parsec-fluidanimate', parallelism=5, input_set='simmedium')+','+runlib.get_instance('parsec-fluidanimate', parallelism=5, input_set='simmedium')+','+runlib.get_instance('parsec-fluidanimate', parallelism=5, input_set='simmedium'))
    # #runlib.run(['open', 'ttsp','exact', '0'], runlib.get_instance('parsec-dedup', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-dedup', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-dedup', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-dedup', parallelism=4, input_set='simmedium'))
    # runlib.run(['open', 'ttsp','exact', '0'], runlib.get_instance('parsec-swaptions', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-swaptions', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-swaptions', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-swaptions', parallelism=4, input_set='simmedium'))
    # runlib.run(['open', 'ttsp','exact', '0'], runlib.get_instance('parsec-streamcluster', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-streamcluster', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-streamcluster', parallelism=4, input_set='simmedium')+','+runlib.get_instance('parsec-streamcluster', parallelism=4, input_set='simmedium'))
    # runlib.run(['open', 'ttsp','exact', '0'], runlib.get_instance('splash2-barnes', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-barnes', parallelism=4, input_set='large'))
    # runlib.run(['open', 'ttsp','exact', '0'], runlib.get_instance('splash2-cholesky', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-cholesky', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-cholesky', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-cholesky', parallelism=4, input_set='large'))
    # runlib.run(['open', 'ttsp','exact', '0'], runlib.get_instance('splash2-fft', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-fft', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-fft', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-fft', parallelism=4, input_set='large'))
    # runlib.run(['open', 'ttsp','exact', '0'], runlib.get_instance('splash2-lu.cont', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-lu.cont', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-lu.cont', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-lu.cont', parallelism=4, input_set='large'))
    # runlib.run(['open', 'ttsp','exact', '0'], runlib.get_instance('splash2-ocean.cont', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-ocean.cont', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-ocean.cont', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-ocean.cont', parallelism=4, input_set='large'))
    # runlib.run(['open', 'ttsp','exact', '0'], runlib.get_instance('splash2-radiosity', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-radiosity', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-radiosity', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-radiosity', parallelism=4, input_set='large'))
    # runlib.run(['open', 'ttsp','exact', '0'], runlib.get_instance('splash2-radix', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-radix', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-radix', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-radix', parallelism=4, input_set='large'))
    # runlib.run(['open', 'ttsp','exact', '0'], runlib.get_instance('splash2-water.nsq', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-water.nsq', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-water.nsq', parallelism=4, input_set='large')+','+runlib.get_instance('splash2-water.nsq', parallelism=4, input_set='large'))

def main():
    eval_seg7()

    #example()
    #eval_seg()


if __name__ == '__main__':
    main()
