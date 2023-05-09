#include "dvfsPB.h"
#include "powermodel.h"
#include <iomanip>
#include <iostream>
#include <map>

using namespace std;

DVFSPB::DVFSPB(std::string model, ThermalModel *thermalModel, const PerformanceCounters *performanceCounters, int numberOfCores, int minFrequency, int maxFrequency, int frequencyStepSize, std::string method)
	: model(model), thermalModel(thermalModel), performanceCounters(performanceCounters), numberOfCores(numberOfCores), minFrequency(minFrequency), maxFrequency(maxFrequency), frequencyStepSize(frequencyStepSize), method(method){
} 

std::vector<int> DVFSPB::getFrequencies(const std::vector<int> &oldFrequencies, const std::vector<bool> &activeCores) {
	std::vector<int> frequencies(numberOfCores);

	map<string, double> instPower = performanceCounters->getPowerOfAllComponents();
	/*for(unsigned int coreCounter = 0; coreCounter < numberOfCores; coreCounter++)
		for (auto & iter : instPower){
			if(iter.first == "C_" + std::to_string(coreCounter) && activeCores.at(coreCounter))
				iter.second = thermalModel->inactivePower;
		}
*/
	cout<< model<<endl;
	if(model == "tsp")
		powerBudget = thermalModel->tsp(activeCores, instPower, method);
	else if(model == "ttsp")
		powerBudget = thermalModel->ttsp(activeCores, instPower, method);
	
	std::ofstream f;
	f.open("PeriodicPowerBudget.trace", std::ios_base::app);

	for (int coreCounter = 0; coreCounter < numberOfCores; coreCounter++) {
		if(coreCounter!=0)
			f<<"	";
		f<<powerBudget.at(coreCounter);
	} 
	f<<endl;

	for (unsigned int coreCounter = 0; coreCounter < numberOfCores; coreCounter++) {
		if (activeCores.at(coreCounter)) {
			float power = performanceCounters->getPowerOfCore(coreCounter);
			float temperature = performanceCounters->getTemperatureOfCore(coreCounter);
			int frequency = oldFrequencies.at(coreCounter);
			float utilization = performanceCounters->getUtilizationOfCore(coreCounter);

			cout << "[Scheduler][DVFSTSP]: Core " << setw(2) << coreCounter << ":";
			cout << " P=" << fixed << setprecision(3) << power << " W";
			cout << " (budget: " << fixed << setprecision(3) << powerBudget[coreCounter] << " W)";
			cout << " f=" << frequency << " MHz";
			cout << " T=" << fixed << setprecision(1) << temperature << " °C";
			cout << " utilization=" << fixed << setprecision(3) << utilization << endl;

			int expectedGoodFrequency = PowerModel::getExpectedGoodFrequency(frequency, power, powerBudget.at(coreCounter), minFrequency, maxFrequency, frequencyStepSize);
			frequencies.at(coreCounter) = expectedGoodFrequency;
		} else {
			frequencies.at(coreCounter) = minFrequency;
		}
	}
	cout<<"Maximum Memory Power=" <<thermalModel->max_mem_power<<endl;
	return frequencies;
}
