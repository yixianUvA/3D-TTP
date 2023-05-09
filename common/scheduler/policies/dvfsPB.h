/**
 * This header implements the power budgeting DVFS policy
 */

#ifndef __DVFS_PB_H
#define __DVFS_PB_H

#include <vector>
#include "dvfspolicy.h"
#include "thermalModel.h"
#include "performance_counters.h"

class DVFSPB : public DVFSPolicy {
public:
    DVFSPB(std::string model, ThermalModel* thermalModel, const PerformanceCounters *performanceCounters, int numberOfCores, int minFrequency, int maxFrequency, int frequencyStepSize, string method);
    virtual std::vector<int> getFrequencies(const std::vector<int> &oldFrequencies, const std::vector<bool> &activeCores);

private: 
    ThermalModel* thermalModel;
    std::string model;
    std::vector<double> powerBudget;
    const PerformanceCounters *performanceCounters;
    unsigned int coreRows;
    unsigned int coreColumns;
    unsigned int numberOfCores;
    int minFrequency;
    int maxFrequency;
    int frequencyStepSize;
    std::string method;
};

#endif
