#ifndef __TSP_H
#define __TSP_H


#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include "fixed_types.h"
#include <Eigen/Dense>
#include <Eigen/Eigenvalues>
#include <unsupported/Eigen/MatrixFunctions>
// #include "eigen-3.4.0/Eigen/Dense"
// #include "eigen-3.4.0/Eigen/Eigenvalues"
// #include "eigen-3.4.0/unsupported/Eigen/MatrixFunctions"


using namespace Eigen; 
using namespace std;

/* grid to block mapping mode	*/
#define	GRID_AVG	0
#define	GRID_MIN	1
#define	GRID_MAX	2
#define	GRID_CENTER	3

/* Block data structure	*/
struct block
{
	string name;
    int i1, j1, i2, j2; //ID of grid cells in each block
};

/* Layer data structure	*/
struct layer
{
	block *blocks;
    int starting_id = 0;
	int n_blocks;
  	int has_power;
};

struct grid_model
{
	layer *layers;
  	bool has_power;
    int numberLayers;
    int numberBlocks;
    int extraThermalNodes; /* spreader nodes, central sink nodes (directly under the spreader), and peripheral sink nodes */
    int numberGridThermalNodes;
    int numberBlockThermalNodes; 
    int numberPrimaryPathLayers;
    int numberSecondaryPathLayers;
    int gridRows;
    int gridColumns;
    int map_mode;
    Matrix<double, Dynamic, Dynamic> BInv;
    Matrix<double, Dynamic, Dynamic> B;
    //Matrix<double, Dynamic, Dynamic> B_CT;
    //Matrix<double, Dynamic, Dynamic> CT;
    Matrix<double, Dynamic, 1> AInv;
    Matrix<double, Dynamic, Dynamic> C;
    Matrix<double, Dynamic, 1> Gamb;
    Matrix<double, Dynamic, Dynamic> g2b_temp;
    Matrix<double, Dynamic, Dynamic> g2b_power;
    Matrix<double, Dynamic, Dynamic> x;
    Matrix<double, Dynamic, 1> y;
    Matrix<double, Dynamic, 1> P;
    Matrix<double, Dynamic, Dynamic> psi; 
    Matrix<double, Dynamic, Dynamic> phi; 
    Matrix<double, Dynamic, 1> Tinit;
    Matrix<double, Dynamic, 1> Tthreshold;
};   

class ThermalModel {
public:
    ThermalModel(const String thermalModelFilename, const String transTempeFilename, double ambientTemperature, double maxTemperature, double inactivePower, double tdp, double time);

    std::vector<double> tsp(const std::vector<bool> &activeCores, std::map<std::string, double> &powerOfInactiveCores, string method);
    std::vector<double> ttsp(const std::vector<bool> &activeCores, std::map<std::string, double> &powerOfInactiveCores, string method);
    //double tsp(const std::vector<bool> &activeCores, const std::vector<double> &powerOfInactiveCores) const;
    //double tsp(const std::vector<bool> &activeCores) const;
    //std::vector<double> tspForManyCandidates(const std::vector<bool> &activeCores, const std::vector<int> &candidates) const;
    //double worstCaseTSP(int amtActiveCores) const;
    //std::vector<double> powerBudgetMaxSteadyState(const std::vector<bool> &activeCores) const;
    std::vector<double> getSteadyState(const Matrix<double, Dynamic, Dynamic> powers) const;
    double max_mem_power;

private:
    double ambientTemperature;
    double maxTemperature;
    double inactivePower;
    String transTempeFilename;
    long double t_interval;
    double tdp;
    template<typename T> T readValue(std::ifstream &file) const;
    std::string readLine(std::ifstream &file) const;
    void readDoubleMatrix(std::ifstream &file, Matrix<double, Dynamic, Dynamic> *matrix, unsigned int rows, unsigned int columns) const;
    void readDoubleMatrix(std::ifstream &file, Matrix<double, Dynamic, 1> *matrix, unsigned int rows) const;

    long power_budget_epoch;
    std::vector<double> powerbudgets;

    grid_model *model;
    /*double *Tsteady;
    double *Tinit;
    double *Tdifference;
	double *multiplierH;
	double **HfunctionWithoutLambda;
    double *exponentials;
    double **matrix_exponentials;*/
};

#endif
