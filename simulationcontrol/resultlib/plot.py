import collections
import os
import sys
HERE = os.path.dirname(os.path.abspath(__file__))
SIMULATIONCONTROL = os.path.dirname(HERE)
sys.path.append(SIMULATIONCONTROL)

import matplotlib as mpl
mpl.use('Agg')
import math
import matplotlib.pyplot as plt
import numpy as np
import seaborn as sns
from matplotlib.ticker import MaxNLocator
from matplotlib.lines import Line2D
import matplotlib
import resultlib


def smoothen(data, k):
    return list(np.convolve(data, np.ones((k,))/k, mode='same'))


def interleave(list, k):
    stride = math.ceil(len(list) / k)
    for start in range(stride):
        for i in range(k):
            ix = start + stride * i
            if ix < len(list):
                yield list[ix]
        

def set_color_palette(num_colors):
    if num_colors > 0:
        pal = list(interleave(sns.color_palette("hls", num_colors), 1))
        sns.set_palette(pal)


def plot_core_trace(run, name, title, ylabel, traces_function, active_cores, yMin=None, yMax=None, smooth=None, force_recreate=False):
    def f():
        try:
            traces = traces_function()
        except KeyboardInterrupt:
            raise
        # print(active_cores)
        return collections.OrderedDict(('Core {}'.format(core), traces[core]) for core in active_cores)
    plot_named_traces(run, name, title, ylabel, f, yMin=yMin, yMax=yMax, smooth=smooth, force_recreate=force_recreate)


def plot_named_traces(run, name, title, ylabel, traces_function, yMin=None, yMax=None, smooth=None, force_recreate=False):
    filename = os.path.join(resultlib.find_run(run), '{}.png'.format(name))

    if not os.path.exists(filename) or force_recreate:
        try:
            traces = traces_function()
        except KeyboardInterrupt:
            raise

        set_color_palette(len(traces))

        plt.figure(figsize=(14,10))
        tracelen = 0
        for name, trace in traces.items():
            valid_trace = [value for value in trace if value is not None]
            if len(valid_trace) > 0:
                #if yMin is not None:
                #    yMin = min(yMin, min(valid_trace) * 1.1)
                #if yMax is not None:
                #    yMax = max(yMax, max(valid_trace) * 1.1)
                tracelen = len(trace)
                if smooth is not None:
                    trace = smoothen(trace, smooth)
                plt.plot(trace, label=name)
        if yMin is not None:
            plt.ylim(bottom=yMin)
        if yMax is not None:
            plt.ylim(top=yMax)

        plt.title('{} {}'.format(title, run))
        plt.legend()
        plt.grid()
        plt.grid(which='minor', linestyle=':')
        plt.minorticks_on()
        plt.savefig(filename, bbox_inches='tight')
        plt.close()


def plot_named_combined(run, name, title, ylabel, smooth=None, force_recreate=False):
    filename = os.path.join(resultlib.find_run(run), '{}.pdf'.format(name))

    active_cores = resultlib.get_active_cores(run)
    name = 'combine'
    #filename = os.path.join(find_run(run), '{}.pdf'.format(name))
    #ips_trace = get_ips_traces(run)
    #power_trace = get_power_traces(run)
    #power_budget_trace = get_power_budget_traces(run)
    #temperature_trace = get_temperature_traces(run)
    set_color_palette(len(active_cores))
    plt.rcParams["font.family"] = "serif"
    font = {'family': 'serif',}
    matplotlib.rcParams['mathtext.fontset'] = 'stix'
    matplotlib.rcParams['font.family'] = 'STIXGeneral'
    fig, (ax1, ax2, ax3, ax4) = plt.subplots(4,figsize=(12,15))
    tracelen = 0
    width = 1

    #Peak Temperature
    if not os.path.exists(filename) or force_recreate:
        try:
            peak = []
            header = []
            for i in range(1,9):
                peak.append(resultlib.get_memory_peak_temperature_traces(run,i))
                header.append("Mem_"+str(i))
            peak.append(resultlib.get_core_peak_temperature_traces(run))
            header.append("Core_1")
        
            #for i in range(1,17):
            #    header.append("C_"+str(i))
            #    peak.append(resultlib.get_core_temperature_traces(run)[i-1])
            traces = collections.OrderedDict((h, t) for h, t in zip(header, peak))
        except KeyboardInterrupt:
            raise
        set_color_palette(len(traces))
        tracelen = 0
        set = 0
        for name, trace in traces.items():
            y_valid_trace = [value for value in trace if value is not None]
            x_valid_trace = [index for index in range(len(y_valid_trace))]
            ax1.set_xticks(np.arange(0, len(x_valid_trace)+1, 5))
            if len(y_valid_trace) > 0:
                tracelen = len(trace)
                if smooth is not None:
                    trace = smoothen(trace, smooth)
                if set<8:
                    ax1.plot(x_valid_trace,y_valid_trace, linewidth=width , color='orange', label=name)
                else:
                    ax1.plot(x_valid_trace,y_valid_trace, linewidth=width , color='blue', label=name)
            ax1.set_ylim(bottom=45)
            ax1.set_ylim(top=75)
            set += 1

        #Power Budget
        try:
            power = resultlib.get_core_power_budget_traces(run)
            traces = collections.OrderedDict(('Core {}'.format(core), power[core]) for core in active_cores)
        except KeyboardInterrupt:
            raise

        set_color_palette(len(traces))
        tracelen = 0
        for name, trace in traces.items():
            y_valid_trace = [value for value in trace if value is not None]
            x_valid_trace = [index for index in range(len(y_valid_trace))]
            ax2.set_xticks(np.arange(0, len(x_valid_trace)+1, 5))
            if len(y_valid_trace) > 0:
                tracelen = len(trace)
                if smooth is not None:
                    trace = smoothen(trace, smooth)
                ax2.plot(x_valid_trace,y_valid_trace, linewidth=width , label=name)
            ax2.set_ylim(bottom=0)
            ax2.set_ylim(top=30)

        #Power Consumption
        try:
            power = resultlib.get_core_power_traces(run)
            traces = collections.OrderedDict(('Core {}'.format(core), power[core]) for core in active_cores)
        except KeyboardInterrupt:
            raise

        set_color_palette(len(traces))
        tracelen = 0
        for name, trace in traces.items():
            y_valid_trace = [value for value in trace if value is not None]
            x_valid_trace = [index for index in range(len(y_valid_trace))]
            ax3.set_xticks(np.arange(0, len(x_valid_trace)+1, 5))
            if len(y_valid_trace) > 0:
                tracelen = len(trace)
                if smooth is not None:
                    trace = smoothen(trace, smooth)
                ax3.plot(x_valid_trace,y_valid_trace, linewidth=width , label=name)
            ax3.set_ylim(bottom=0)
            ax3.set_ylim(top=20)

        #Frequency
        try:
            power = resultlib.get_core_freq_traces(run)
            traces = collections.OrderedDict(('Core {}'.format(core), power[core]) for core in active_cores)
        except KeyboardInterrupt:
            raise

        set_color_palette(len(traces))
        tracelen = 0
        for name, trace in traces.items():
            y_valid_trace = [value for value in trace if value is not None]
            x_valid_trace = [index for index in range(len(y_valid_trace))]
            ax4.set_xticks(np.arange(0, len(x_valid_trace)+1, 5))
            if len(y_valid_trace) > 0:
                tracelen = len(trace)
                if smooth is not None:
                    trace = smoothen(trace, smooth)
                ax4.plot(x_valid_trace,y_valid_trace, linewidth=width , label=name)
            ax4.set_ylim(bottom=0.8e9)
            ax4.set_ylim(top=4.2e9)

        #ax1.title('{} {}'.format(title, run))
        #ax1.legend(bbox_to_anchor=(0.014,0.78,0.940,0.2),loc="upper center", mode="expand", ncol=9, fontsize=14)
        #ax1.legend(bbox_to_anchor=(0.014,0.78,0.540,0.2), fancybox=True, framealpha=0.6, loc="upper left", mode="expand", borderaxespad=0, ncol=2, fontsize=14)
        custom_lines = [Line2D([0], [0], color='orange', lw=4),
                Line2D([0], [0], color='blue', lw=4)]
        ax1.legend(custom_lines, ['Core Layer (C0)', 'Memory Layers L0 – L7 (8×)'],loc="upper left", fontsize=12)
        ax1.annotate(s='',xy=(99.5,47), xytext=(-0.5,47), arrowprops=dict(facecolor='black', lw=1,arrowstyle='<->, head_width=0.4'))
        ax1.text(40, 47.5, 'Total Execution Time: 99 ms', fontsize=12)
        
        ax1.text(-3.8, 47, "(a)", size=14, bbox=dict(boxstyle="round", facecolor=(1,1,1,0.25),edgecolor=(0,0,0,1))) #IPS
        ax2.text(-3.8, 2, "(b)", size=14, bbox=dict(boxstyle="round", facecolor=(1,1,1,0.25),edgecolor=(0,0,0,1))) #power budget
        ax3.text(-3.8, 1.4, "(c)", size=14, bbox=dict(boxstyle="round", facecolor=(1,1,1,0.25),edgecolor=(0,0,0,1))) #power consumption
        ax4.text(-3.8, 1.05e9, "(d)", size=14, bbox=dict(boxstyle="round", facecolor=(1,1,1,0.25),edgecolor=(0,0,0,1))) #power consumption
        ax1.text(80, 70.5, "L0", size=12, bbox=dict(boxstyle="round", facecolor=(1,1,1,0),edgecolor=(0,0,0,0))) #IPS
        ax1.text(80, 58.3, "L7", size=12, bbox=dict(boxstyle="round", facecolor=(1,1,1,0.8),edgecolor=(0,0,0,0))) #IPS
        ax1.text(80, 55, "C0", size=12, bbox=dict(boxstyle="round", facecolor=(1,1,1,0.8),edgecolor=(0,0,0,0))) #IPS


        ax1.grid()
        ax1.grid(which='minor', linestyle=':', linewidth=0.3)
        ax1.grid(which='major', linestyle='-', linewidth=0.5)
        ax1.minorticks_on()
    
        #ax2.legend(bbox_to_anchor=(0.014,0.78,0.940,0.2),loc="upper center", mode="expand", ncol=8)
        ax2.grid()
        ax2.grid(which='minor', linestyle=':', linewidth=0.3)
        ax2.grid(which='major', linestyle='-', linewidth=0.5)
        ax2.minorticks_on()

        ax3.grid()
        ax3.grid(which='minor', linestyle=':', linewidth=0.3)
        ax3.grid(which='major', linestyle='-', linewidth=0.5)
        ax3.minorticks_on()

        ax4.grid()
        ax4.grid(which='minor', linestyle=':', linewidth=0.3)
        ax4.grid(which='major', linestyle='-', linewidth=0.5)
        ax4.minorticks_on()

        ax4.set_xlabel('Time [ms]', fontsize=16)
        ax1.set_ylabel('Temperature [$^\circ$C]', fontsize=16)#Inst. per Sec.
        ax2.set_ylabel('Power Budget [W]', fontsize=16)
        ax3.set_ylabel('Power [W]', fontsize=16)
        ax4.set_ylabel('Frequency [GHz]', fontsize=16)

        ax1.xaxis.set_tick_params(labelsize=14)
        ax2.xaxis.set_tick_params(labelsize=14)
        ax3.xaxis.set_tick_params(labelsize=14)
        ax4.xaxis.set_tick_params(labelsize=14)
        ax1.yaxis.set_tick_params(labelsize=14)
        ax2.yaxis.set_tick_params(labelsize=14)
        ax3.yaxis.set_tick_params(labelsize=14)
        ax4.yaxis.set_tick_params(labelsize=14)

        fig.savefig(filename, bbox_inches='tight')
        plt.close()

def plot_named_combined_ondemand(run, name, title, ylabel, smooth=None, force_recreate=False):
    filename = os.path.join(resultlib.find_run(run), '{}.pdf'.format(name))

    active_cores = resultlib.get_active_cores(run)
    name = 'combine'
    #filename = os.path.join(find_run(run), '{}.pdf'.format(name))
    #ips_trace = get_ips_traces(run)
    #power_trace = get_power_traces(run)
    #power_budget_trace = get_power_budget_traces(run)
    #temperature_trace = get_temperature_traces(run)
    set_color_palette(len(active_cores))
    plt.rcParams["font.family"] = "serif"
    font = {'family': 'serif',}
    matplotlib.rcParams['mathtext.fontset'] = 'stix'
    matplotlib.rcParams['font.family'] = 'STIXGeneral'
    fig, (ax1, ax2, ax3) = plt.subplots(3,figsize=(12,11))
    tracelen = 0
    width = 1

    #Peak Temperature
    if not os.path.exists(filename) or force_recreate:
        try:
            peak = []
            header = []
            for i in range(1,9):
                peak.append(resultlib.get_memory_peak_temperature_traces(run,i))
                header.append("Mem_"+str(i))
            peak.append(resultlib.get_core_peak_temperature_traces(run))
            header.append("Core_1")  
            #for i in range(1,17):
            #    header.append("C_"+str(i))
            #    peak.append(resultlib.get_core_temperature_traces(run)[i-1])
    
            traces = collections.OrderedDict((h, t) for h, t in zip(header, peak))
        except KeyboardInterrupt:
            raise
        set_color_palette(len(traces))
        tracelen = 0
        set = 0
        for name, trace in traces.items():
            y_valid_trace = [value for value in trace if value is not None]
            x_valid_trace = [index for index in range(len(y_valid_trace))]
            ax1.set_xticks(np.arange(0, len(x_valid_trace)+1, 5))
            if len(y_valid_trace) > 0:
                tracelen = len(trace)
                if smooth is not None:
                    trace = smoothen(trace, smooth)
                if set<8:
                    ax1.plot(x_valid_trace,y_valid_trace, linewidth=width , color='orange', label=name)
                else:
                    ax1.plot(x_valid_trace,y_valid_trace, linewidth=width , color='blue', label=name)
            ax1.set_ylim(bottom=45)
            ax1.set_ylim(top=75)
            set += 1

        #Power Consumption
        try:
            power = resultlib.get_core_power_traces(run)
            traces = collections.OrderedDict(('Core {}'.format(core), power[core]) for core in active_cores)
        except KeyboardInterrupt:
            raise

        set_color_palette(len(traces))
        tracelen = 0
        for name, trace in traces.items():
            y_valid_trace = [value for value in trace if value is not None]
            x_valid_trace = [index for index in range(len(y_valid_trace))]
            ax2.set_xticks(np.arange(0, len(x_valid_trace)+1, 5))

            if len(y_valid_trace) > 0:
                tracelen = len(trace)
                if smooth is not None:
                    trace = smoothen(trace, smooth)
                ax2.plot(x_valid_trace,y_valid_trace, linewidth=width , label=name)
            ax2.set_ylim(bottom=0)
            ax2.set_ylim(top=20)

        #Frequency
        try:
            power = resultlib.get_core_freq_traces(run)
            traces = collections.OrderedDict(('Core {}'.format(core), power[core]) for core in active_cores)
        except KeyboardInterrupt:
            raise

        set_color_palette(len(traces))
        tracelen = 0
        for name, trace in traces.items():
            y_valid_trace = [value for value in trace if value is not None]
            x_valid_trace = [index for index in range(len(y_valid_trace))]
            ax3.set_xticks(np.arange(0, len(x_valid_trace)+1, 5))
            if len(y_valid_trace) > 0:
                tracelen = len(trace)
                if smooth is not None:
                    trace = smoothen(trace, smooth)
                ax3.plot(x_valid_trace,y_valid_trace, linewidth=width , label=name)
            ax3.set_ylim(bottom=0.8e9)
            ax3.set_ylim(top=4.2e9)

        #ax1.title('{} {}'.format(title, run))
        #ax1.legend(bbox_to_anchor=(0.014,0.78,0.940,0.2),loc="upper center", mode="expand", ncol=9, fontsize=14)
        #ax1.legend(bbox_to_anchor=(0.014,0.78,0.540,0.2), fancybox=True, framealpha=0.6, loc="upper left", mode="expand", borderaxespad=0, ncol=2, fontsize=14)
        
        custom_lines = [Line2D([0], [0], color='orange', lw=4),
                Line2D([0], [0], color='blue', lw=4)]
        ax1.legend(custom_lines, ['Core Layer (C0)', 'Memory Layers L0 – L7 (8×)'],loc="upper left", fontsize=12)

        ax1.annotate(s='',xy=(110.5,47), xytext=(-0.5,47), arrowprops=dict(facecolor='black', lw=1,arrowstyle='<->, head_width=0.4'))
        ax1.text(40, 47.5, 'Total Execution Time: 110 ms', fontsize=12)
        
        ax1.text(-4.2, 47, "(a)", size=14, bbox=dict(boxstyle="round", facecolor=(1,1,1,0.25),edgecolor=(0,0,0,1))) #IPS
        ax2.text(-4.2, 1.5, "(b)", size=14, bbox=dict(boxstyle="round", facecolor=(1,1,1,0.25),edgecolor=(0,0,0,1))) #power budget
        ax3.text(-4.2, 1.05e9, "(c)", size=14, bbox=dict(boxstyle="round", facecolor=(1,1,1,0.25),edgecolor=(0,0,0,1))) #power consumption
       
        ax1.text(77, 70, "L0", size=12, bbox=dict(boxstyle="round", facecolor=(1,1,1,0),edgecolor=(0,0,0,0))) #IPS
        ax1.text(77, 56, "L7", size=12, bbox=dict(boxstyle="round", facecolor=(1,1,1,0.8),edgecolor=(0,0,0,0))) #IPS
        ax1.text(77, 52, "C0", size=12, bbox=dict(boxstyle="round", facecolor=(1,1,1,0.8),edgecolor=(0,0,0,0))) #IPS

        ax1.text(88.5, 72.3, "1", size=12, bbox=dict(boxstyle="circle", facecolor=(1,1,1,0.25),edgecolor=(0,0,0,1))) #IPS
        ax1.annotate(s='',xy=(89,45), xytext=(89,71), arrowprops=dict(facecolor='black', lw=1,arrowstyle='-', linestyle="--"))
        ax2.annotate(s='',xy=(89,0), xytext=(89,20), arrowprops=dict(facecolor='black', lw=1,arrowstyle='-', linestyle="--"))
        ax3.annotate(s='',xy=(89,0.8e9), xytext=(89,4.2e9), arrowprops=dict(facecolor='black', lw=1,arrowstyle='-', linestyle="--"))

        ax1.grid()
        ax1.grid(which='minor', linestyle=':', linewidth=0.3)
        ax1.grid(which='major', linestyle='-', linewidth=0.5)
        ax1.minorticks_on()
    
        #ax2.legend(bbox_to_anchor=(0.014,0.78,0.940,0.2),loc="upper center", mode="expand", ncol=8)
        ax2.grid()
        ax2.grid(which='minor', linestyle=':', linewidth=0.3)
        ax2.grid(which='major', linestyle='-', linewidth=0.5)
        ax2.minorticks_on()

        ax3.grid()
        ax3.grid(which='minor', linestyle=':', linewidth=0.3)
        ax3.grid(which='major', linestyle='-', linewidth=0.5)
        ax3.minorticks_on()
                
        ax1.set_ylabel('Temperature [$^\circ$C]', fontsize=16)#Inst. per Sec.
        ax2.set_ylabel('Power [W]', fontsize=16)
        ax3.set_ylabel('Frequency [GHz]', fontsize=16)
        ax3.set_xlabel('Time [ms]', fontsize=16)

        ax1.xaxis.set_tick_params(labelsize=14)
        ax2.xaxis.set_tick_params(labelsize=14)
        ax3.xaxis.set_tick_params(labelsize=14)
        ax1.yaxis.set_tick_params(labelsize=14)
        ax2.yaxis.set_tick_params(labelsize=14)
        ax3.yaxis.set_tick_params(labelsize=14)

        fig.savefig(filename, bbox_inches='tight')
        plt.close()

def plot_cpi_stack_trace(run, active_cores, force_recreate=False):
    blacklist = ['imbalance', 'sync', 'total', 'rs_full', 'serial', 'smt', 'mem-l4', 'mem-dram-cache', 'dvfs-transition', 'other']
    parts = [part for part in resultlib.get_cpi_stack_trace_parts(run) if not any(b in part for b in blacklist)]
    traces = {part: resultlib.get_cpi_stack_part_trace(run, part) for part in parts}

    set_color_palette(len(parts))

    for core in active_cores:
        name = 'cpi-stack-trace-c{}'.format(core)
        title = 'cpi-stack-trace Core {}'.format(core)
        filename = os.path.join(resultlib.find_run(run), '{}.png'.format(name))
        if not os.path.exists(filename) or force_recreate:
            fig = plt.figure(figsize=(14,10))
            ax = fig.add_subplot(1, 1, 1)

            stacktrace = [traces[part][core] for part in parts]
            xs = range(len(stacktrace[0]))
            plt.stackplot(xs, stacktrace, labels=parts)
            plt.ylim(bottom=0, top=6)
            plt.title('{} {}'.format(title, run))

            handles, labels = ax.get_legend_handles_labels()
            ax.legend(handles[::-1], labels[::-1], loc='upper left', bbox_to_anchor=(1, 1))
            
            plt.grid() 
            plt.grid(which='minor', linestyle=':')
            plt.minorticks_on() 
            plt.savefig(filename, bbox_inches='tight')
            plt.close()


def create_plots(run, force_recreate=False):
    print('creating plots for {}'.format(run))
    active_cores = resultlib.get_active_cores(run)

    plot_core_trace(run, 'frequency', 'Frequency', 'Frequency (GHz)', lambda: resultlib.get_core_freq_traces(run), active_cores, yMin=0, yMax=4.1e9, force_recreate=force_recreate)
    plot_core_trace(run, 'core_temperature', 'Core temperature', 'Core Temperature (C)', lambda: resultlib.get_core_temperature_traces(run), active_cores, yMin=45, yMax=100, force_recreate=force_recreate)
    plot_named_traces(run, 'all_temperatures', 'All temperatures', 'Temperature (C)', lambda: resultlib.get_all_temperature_traces(run), yMin=45, yMax=100, force_recreate=force_recreate)
    plot_core_trace(run, 'core_power', 'Core power', 'Power (W)', lambda: resultlib.get_core_power_traces(run), active_cores, yMin=0, force_recreate=force_recreate)
    plot_core_trace(run, 'core_utilization', 'Core utilization', 'Core Utilization', lambda: resultlib.get_core_utilization_traces(run), active_cores, yMin=0, force_recreate=force_recreate)
    plot_core_trace(run, 'cpi', 'CPI', 'CPI', lambda: resultlib.get_cpi_traces(run), active_cores, yMin=0, force_recreate=force_recreate)
    plot_core_trace(run, 'ips', 'IPS', 'IPS', lambda: resultlib.get_ips_traces(run), active_cores, yMin=0, yMax=8e9, force_recreate=force_recreate)
    print(run)
    if "ondemand" not in run and "date15" not in run:
        plot_core_trace(run, 'core_power_budget', 'core_power_budget', 'Power Budget (W)', lambda: resultlib.get_core_power_budget_traces(run), active_cores, yMin=0, force_recreate=force_recreate)
    #if "ondemand" or "date15" in run:
    #    plot_named_combined(run, 'peak_memory', 'Peak Memory', 'Peak Memory', force_recreate=force_recreate)
    else:
        plot_named_combined_ondemand(run, 'peak_memory', 'Peak Memory', 'Peak Memory', force_recreate=force_recreate)

    # plot_core_trace(run, 'ipssmooth', 'Smoothed IPS', lambda: resultlib.get_ips_traces(run), active_cores, yMin=0, yMax=8e9, smooth=10, force_recreate=force_recreate)
    #plot_cpi_stack_trace(run, active_cores, force_recreate=force_recreate)


if __name__ == '__main__':
    for run in sorted(resultlib.get_runs())[::-1]:
        create_plots(run)
