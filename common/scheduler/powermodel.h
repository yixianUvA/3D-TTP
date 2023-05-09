#ifndef __POWERMODEL_H
#define __POWERMODEL_H
#include <vector>

class PowerModel {
public:
    static int getExpectedGoodFrequency(int currentFrequency, float powerConsumption, float powerBudget, int minFrequency, int maxFrequency, int frequencyStepSize);
    static int getExpectedGoodFrequency(int currentFrequency, float powerConsumption, float powerBudget, const std::vector<int> &Frequency);
    static float estimatePower(int currentFrequency, float currentPowerConsumption, int newFrequency);
};

#endif
