/**
 * This header implements the ondemand governor with DTM.
 * The ondemand governor implementation is based on
 *     Pallipadi, Venkatesh, and Alexey Starikovskiy.
 *     "The ondemand governor."
 *     Proceedings of the Linux Symposium. Vol. 2. No. 00216. 2006.
 */

#ifndef __DVFS_DATE15_H
#define __DVFS_DATE15_H

#include <vector>
#include "dvfspolicy.h"
#include "performance_counters.h"

class DVFSDATE15 : public DVFSPolicy {
public:
    DVFSDATE15(
        const PerformanceCounters *performanceCounters,
        int numberOfCores,
        int minFrequency,
        int maxFrequency,
        int frequencyStepSize,
        float upThreshold,
        float downThreshold,
        float dtmCriticalTemperature,
        float dtmRecoveredTemperature);
    virtual std::vector<int> getFrequencies(const std::vector<int> &oldFrequencies, const std::vector<bool> &activeCores);

private:
    const PerformanceCounters *performanceCounters;

    unsigned int numberOfCores;
    int minFrequency;
    int maxFrequency;
    int frequencyStepSize;
    float upThreshold;
    float downThreshold;
    float dtmCriticalTemperature;
    float dtmRecoveredTemperature;

    bool in_throttle_mode = false;
    bool throttle();
};

#endif
