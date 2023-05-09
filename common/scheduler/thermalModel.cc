#include "thermalModel.h"
#include <algorithm> 
#include <sstream> 

ThermalModel::ThermalModel(const String thermalModelFilename, const String transTempeFilename, double ambientTemperature, double maxTemperature, double inactivePower, double tdp, double time)
    : transTempeFilename(transTempeFilename), ambientTemperature(ambientTemperature), maxTemperature(maxTemperature), inactivePower(inactivePower), tdp(tdp) {
    // std::cout << "thermalModelFilename is " << thermalModelFilename << std::endl;
    // std::cout << "transTempeFilename is " << transTempeFilename << std::endl; 
    // std::cout << "ambientTemperature is " << ambientTemperature << std::endl; 
    // std::cout << "maxTemperature is " << maxTemperature << std::endl; 
    // std::cout << "inactivePower is " << inactivePower << std::endl; 
    // std::cout << "tdp is " << tdp << std::endl; 
    // std::cout << "time is " << time << std::endl;
    model = new grid_model;
    std::ifstream f;
    f.open(thermalModelFilename.c_str());
    max_mem_power =0;
    model->numberLayers = readValue<int>(f);
    model->layers = new layer[model->numberLayers];
    int total_block = 0;
    int id = 0;
    for(unsigned int i = 0; i < model->numberLayers; i++){
        model->layers[i].starting_id = id;
        model->layers[i].n_blocks = readValue<int>(f);
        model->layers[i].blocks = new block[model->layers[i].n_blocks];
        total_block += model->layers[i].n_blocks;
        id += model->layers[i].n_blocks;
        model->layers[i].has_power = readValue<int>(f);
        for(unsigned int j = 0; j < model->layers[i].n_blocks; j++){
            model->layers[i].blocks[j].name = readValue<string>(f);
            model->layers[i].blocks[j].i1 = readValue<int>(f);
            model->layers[i].blocks[j].j1 = readValue<int>(f);
            model->layers[i].blocks[j].i2 = readValue<int>(f);
            model->layers[i].blocks[j].j2 = readValue<int>(f);
        }
    } 
    model->numberPrimaryPathLayers = readValue<unsigned int>(f);
    model->numberSecondaryPathLayers = readValue<unsigned int>(f);
    model->gridRows = readValue<unsigned int>(f); 
    model->gridColumns = readValue<unsigned int>(f);
    model->numberBlocks = readValue<unsigned int>(f);
    model->numberGridThermalNodes = readValue<unsigned int>(f);
    model->extraThermalNodes = readValue<unsigned int>(f);
    model->numberBlockThermalNodes = model->numberBlocks + model->extraThermalNodes;
    if (model->numberBlocks != total_block) {
        std::cout << "Assertion error in thermal model file, wrong numberBlocks" << std::endl;
		exit (1);
    }
    if (model->numberGridThermalNodes != model->numberLayers*model->gridRows*model->gridColumns+model->extraThermalNodes) {
        std::cout << "Assertion error in thermal model file: wrong number of total grid thermal nodes" << std::endl;
		exit (1);
    }

    readDoubleMatrix(f, &model->B, model->numberGridThermalNodes, model->numberGridThermalNodes);
    readDoubleMatrix(f, &model->AInv, model->numberGridThermalNodes); 
    readDoubleMatrix(f, &model->C, model->numberGridThermalNodes, model->numberGridThermalNodes);
    model->BInv.resize(model->numberGridThermalNodes, model->numberBlockThermalNodes);   
    model->BInv = model->B.inverse(); 

    readDoubleMatrix(f, &model->Gamb, model->numberGridThermalNodes);
    for(unsigned int i=0; i<model->numberGridThermalNodes; i++)
        model->Gamb(i) = model->Gamb(i)*(ambientTemperature+273.15);
    readDoubleMatrix(f, &model->g2b_power, model->numberGridThermalNodes, model->numberBlockThermalNodes);
    readDoubleMatrix(f, &model->g2b_temp, model->numberGridThermalNodes, model->numberBlockThermalNodes);
    model->map_mode = readValue<int>(f); 
    
    model->x.resize(model->numberGridThermalNodes, model->numberBlockThermalNodes);
    model->x = model->BInv*model->g2b_power;
    model->y.resize(model->numberGridThermalNodes);
    model->y = model->BInv*model->Gamb;
    model->P.resize(model->numberBlockThermalNodes);
     
    t_interval = time; //20000000;
    model->Tinit.resize(model->numberBlockThermalNodes);
    model->Tthreshold.resize(model->numberGridThermalNodes);
 
    Matrix<double, Dynamic, Dynamic> aux;
    aux.resize(model->numberGridThermalNodes,model->numberGridThermalNodes);
	for(unsigned int i = 0; i < model->numberGridThermalNodes; i++){
        model->Tthreshold(i) = maxTemperature + 273.15;
		for(unsigned int j = 0; j < model->numberGridThermalNodes; j++){
			aux(i,j) = (model->C(i,j) != 0) ? -model->C(i,j) : 0;
        }
    }

    EigenSolver< Matrix<double, Dynamic, Dynamic> > es(aux);
	Matrix<double, Dynamic, Dynamic> eigenValuesAux = es.pseudoEigenvalueMatrix();
	Matrix<double, Dynamic, Dynamic> eigenVectorsAux = es.pseudoEigenvectors();
	Matrix<double, Dynamic, Dynamic> eigenVectorsInvAux = eigenVectorsAux.inverse();
    // std::cout << "&&&&&&&&&&&&&&&&" << std::endl;
    // std::cout << "The eigensolver can work " << std::endl;
    // std::cout << "&&&&&&&&&&&&&&&&" << std::endl;

	
    Matrix<double, Dynamic, Dynamic> exponentials; 
    exponentials.resize(model->numberGridThermalNodes,model->numberGridThermalNodes);

    for(int k = 0; k < model->numberGridThermalNodes; k++){
        for(int j = 0; j < model->numberGridThermalNodes; j++)
            exponentials(k,j) = 0;
    }

    int step = 0;
    for(int k = 0; k < model->numberGridThermalNodes-1;){
            if(eigenValuesAux(k,k+1) != 0){
                exponentials(k,k) = pow((double)M_E, eigenValuesAux(k,k) * t_interval * 1e-9)*cos(eigenValuesAux(k,k+1) * t_interval * 1e-9);
                exponentials(k,k+1) = pow((double)M_E, eigenValuesAux(k,k) * t_interval * 1e-9)*sin(eigenValuesAux(k,k+1) * t_interval * 1e-9);
                exponentials(k+1,k) = - pow((double)M_E, eigenValuesAux(k,k) * t_interval * 1e-9)*sin(abs(eigenValuesAux(k,k+1)) * t_interval * 1e-9); 
                exponentials(k+1,k+1) = pow((double)M_E, eigenValuesAux(k,k) * t_interval * 1e-9)*cos(abs(eigenValuesAux(k,k+1)) * t_interval * 1e-9);
                step = 2;
            }else{
                exponentials(k,k) = pow((double)M_E, eigenValuesAux(k,k) * t_interval * 1e-9);
                if(k == model->numberGridThermalNodes-2)
                   exponentials(k+1,k+1) = pow((double)M_E, eigenValuesAux(k+1,k+1) * t_interval * 1e-9); 
                step = 1;
            }
            k+=step;
    }
    model->psi.resize(model->numberGridThermalNodes,model->numberGridThermalNodes);
    model->phi.resize(model->numberGridThermalNodes,model->numberGridThermalNodes);

    model->psi = eigenVectorsAux * exponentials * eigenVectorsInvAux;
    for(int k = 0; k < model->numberGridThermalNodes; k++){
        for(int j = 0; j < model->numberGridThermalNodes; j++){
            aux(k,j) = (model->psi(k,j) !=0) ? -model->psi(k,j): 0;
            if(k==j) aux(k,j) += double(1.0);
        }
    } 
    model->phi = model->C.inverse() * aux * model->AInv.asDiagonal(); 
    

    /*EigenSolver< Matrix<double, Dynamic, Dynamic> > es(aux);
    Matrix<double, Dynamic, 1> eigenValuesAux = es.eigenvalues().real();
	Matrix<double, Dynamic, Dynamic> eigenVectorsAux = es.eigenvectors().real();
	Matrix<double, Dynamic, Dynamic> eigenVectorsInvAux = eigenVectorsAux.inverse();

    model->psi.resize(model->numberGridThermalNodes,model->numberGridThermalNodes);
    model->phi.resize(model->numberGridThermalNodes,model->numberGridThermalNodes);

    Matrix<double, Dynamic, 1> exponentials;
    exponentials.resize(model->numberGridThermalNodes);
    for(unsigned int i = 0; i < model->numberGridThermalNodes; i++){
        exponentials(i) = pow((double)M_E, eigenValuesAux(i) * double(1000000) * 1e-9);
    }
    model->psi = eigenVectorsAux * exponentials.asDiagonal() * eigenVectorsInvAux;
    for(int k = 0; k < model->numberGridThermalNodes; k++){
        for(int j = 0; j < model->numberGridThermalNodes; j++){
            aux(k,j) = (model->psi(k,j) !=0) ? -model->psi(k,j): 0;
            if(k==j) aux(k,j) += double(1.0);
        }
    }
    model->phi = model->C.inverse() * aux * model->AInv.asDiagonal(); */

    /*
    //Matrix<double, Dynamic, Dynamic> aux = Iexponentials.asDiagonal();
    //model->psi = eigenVectorsAux * exponentials.asDiagonal() * eigenVectorsInvAux;
    //model->phi = eigenVectorsAux * aux.inverse() * eigenVectorsInvAux;
    //cout<<aux;
    /*model->B_CT.resize(model->numberGridThermalNodes, model->numberGridThermalNodes);
    model->CT.resize(model->numberGridThermalNodes, 1);
    for(int i=0; i<model->numberGridThermalNodes; i++){
        model->CT(i,0) = 0;
        for(int j=0; j<model->numberGridThermalNodes; j++){
            if(j>model->numberSecondaryPathLayers*model->gridRows*model->gridColumns-1 && j < (model->numberSecondaryPathLayers+1)*model->gridRows*model->gridColumns){
                model->B_CT(i,j) = 0;
                model->CT(i,0) += model->B(i,j);
            }
            else
                model->B_CT(i,j) = model->B(i,j);
            //cout<<"("<<i<<","<<j<<") "<<model->B_CT(i,j)<<endl;
        }
        model->CT(i,0) = model->CT(i,0)*(maxTemperature+273.15);
    }*/

    f.close();   
}

template<typename T>
T ThermalModel::readValue(std::ifstream &file) const {
    T value;
    file >> value;
    //std::getline(file, value);
    if(file.rdstate() != std::stringstream::goodbit){
        std::cout << "Assertion error in thermal model file: file ended too early." << std::endl;
        file.close();
        exit(1);
    }
    return value;
}

std::string ThermalModel::readLine(std::ifstream &file) const {
    std::string value;
    std::getline(file, value);
    if(file.rdstate() != std::stringstream::goodbit){
        std::cout << "Assertion error in thermal model file: file ended too early." << std::endl;
        file.close();
        exit(1);
    }
    return value;
}

void ThermalModel::readDoubleMatrix(std::ifstream &file, Matrix<double, Dynamic, Dynamic>* matrix, unsigned int rows, unsigned int columns) const {
    
    (*matrix).resize(rows, columns);
    for (unsigned int r = 0; r < rows; r++)
        for (unsigned int c = 0; c < columns; c++)
            (*matrix)(r,c) = readValue<double>(file);
}

void ThermalModel::readDoubleMatrix(std::ifstream &file, Matrix<double, Dynamic, 1>* matrix, unsigned int rows) const {
    
    (*matrix).resize(rows);
    for (unsigned int r = 0; r < rows; r++)
        (*matrix)(r) = readValue<double>(file);
}

void inplaceGauss(std::vector<std::vector<float>> &A, std::vector<float> &b) {
    unsigned int n = b.size();
    for (unsigned int row = 0; row < n; row++) {
        // divide
        float pivot = A.at(row).at(row);
        b.at(row) /= pivot;
        for (unsigned int col = 0; col < n; col++) {
            A.at(row).at(col) /= pivot;
        }

        // add
        for (unsigned int row2 = 0; row2 < n; row2++) {
            if (row != row2) {
                float factor = A.at(row2).at(row);
                b.at(row2) -= factor * b.at(row);
                for (unsigned int col = 0; col < n; col++) {
                    A.at(row2).at(col) -= factor * A.at(row).at(col);
                }
            }
        }
    }
}

std::vector<double> ThermalModel::tsp(const std::vector<bool> &activeCores, std::map<std::string, double> &powerOfInactiveCores, string method) {
    std::vector<int> activeIndices; 
    std::vector<int> activeGridIndices;
    std::vector<int> activeCoreIndices;
    std::vector<int> activeCoreLayers;

    //Matrix<double, model->numberBlockThermalNodes, 1> inactivePowers;
    //inactivePowers.resize(model->numberBlocks+model->extraThermalNodes,1);

    std::vector<double> powers(activeCores.size(), 0);
    
    for(unsigned int coreCounter = 0; coreCounter < activeCores.size(); coreCounter++)
		for (auto & iter : powerOfInactiveCores){
			if(iter.first == "C_" + std::to_string(coreCounter) && activeCores.at(coreCounter))
				iter.second = inactivePower;
		}

    for (auto & iter : powerOfInactiveCores){
		if(iter.first.find("B") != string::npos) 
        {
            //max_mem_power = (max_mem_power > iter.second) ? max_mem_power : iter.second;
            iter.second = 0.219;
        }
    }
    int i, j, base;
    for(j=0; j< model->numberBlockThermalNodes; j++)
        model->P(j) = 0;
    for(i=0; i< model->numberLayers; i++) {
        if(model->layers[i].has_power) {
            //cout<<model->layers[i].starting_id<<", "<<model->layers[i].n_blocks<<"; ";
            for(j=0; j< model->layers[i].n_blocks; j++) {
                for (auto & iter : powerOfInactiveCores){
			        if(iter.first == model->layers[i].blocks[j].name) 
                    {
                        model->P(model->layers[i].starting_id+j) = iter.second;
                        //cout<<model->layers[i].blocks[j].name<<" ";
                    }
		        }
            }
        }
    }
 
    Matrix<double, Dynamic, 1> temperature;
    temperature.resize(model->numberGridThermalNodes);
    temperature = model->x*model->P+model->y;

    std::vector<double> tInactive(activeCores.size(), 0);
    int i1, j1, i2, j2, ci1, cj1, ci2, cj2, numCoreLayer;
    numCoreLayer = 0;
    for(i=0, base = 0; i < model->numberLayers; i++) {
        if(model->layers[i].blocks[0].name.find('C') != string::npos && model->layers[i].has_power)
        {   
            numCoreLayer ++;
            for(j=0; j < model->layers[i].n_blocks; j++) {
                /* extent of this unit in grid cell units	*/
                i1 = model->layers[i].blocks[j].i1;
                j1 = model->layers[i].blocks[j].j1;
                i2 = model->layers[i].blocks[j].i2;
                j2 = model->layers[i].blocks[j].j2;

                /* map the center grid cell's temperature to the block	*/
                if (model->map_mode == GRID_CENTER) {
                    /* center co-ordinates	*/	
                    ci1 = (i1 + i2) / 2;
                    cj1 = (j1 + j2) / 2; 
                    /* in case of even no. of cells, center 
                    * is the average of two central cells
                    */
                    /* ci2 = ci1-1 when even, ci1 otherwise	*/  
                    ci2 = ci1 - !((i2-i1) % 2);
                    /* cj2 = cj1-1 when even, cj1 otherwise	*/  
                    cj2 = cj1 - !((j2-j1) % 2);
                    tInactive[base+j] = (temperature(i * model->gridRows * model->gridColumns + ci1 * model->gridColumns + cj1) +
                                        temperature(i * model->gridRows * model->gridColumns + ci2 * model->gridColumns + cj1) +
                                        temperature(i * model->gridRows * model->gridColumns + ci1 * model->gridColumns + cj2) +
                                        temperature(i * model->gridRows * model->gridColumns + ci2 * model->gridColumns + cj2)) / 4;
                    //cout<<"T_"<<model->layers[i].blocks[j].name<<": "<<tInactive[base+j]-273.15<<"\n";
                }
                /* find the min/max/avg temperatures of the 
                * grid cells in this block
                */
            
                double avg = 0.0;
                double min, max;
                int count = 0;
                min = max = temperature(i * model->gridRows * model->gridColumns + i1 * model->gridColumns + j1);
                for(int k=i1; k < i2; k++)
                    for(int l=j1; l < j2; l++) {
                        avg += temperature(i * model->gridRows * model->gridColumns + k * model->gridColumns + l);
                        if (temperature(i * model->gridRows * model->gridColumns + k * model->gridColumns + l) < min)
                            min = temperature(i * model->gridRows * model->gridColumns + k * model->gridColumns + l);
                        if (temperature(i * model->gridRows * model->gridColumns + k * model->gridColumns + l) > max)
                            max = temperature(i * model->gridRows * model->gridColumns + k * model->gridColumns + l);
                        count++;
                }

                switch (model->map_mode)
                {
                    case GRID_AVG:
                        tInactive[base+j] = avg / count;
                        break;
                    case GRID_MIN:
                        tInactive[base+j] = min;
                        break;
                    case GRID_MAX:
                        tInactive[base+j] = max;
                        break;
                    case GRID_CENTER:
                        break;
                    default:
                        assert("unknown mapping mode\n");
                        break;
                }
            
                if(activeCores.at(base+j)){
                    activeCoreIndices.push_back(model->layers[i].starting_id+j);
                    activeIndices.push_back(base+j);
                    for(int x=i1; x<i2; x++)
                        for(int y=j1; y<j2; y++){
                            //activeGridIndices.push_back(i * model->gridRows * model->gridColumns + x * model->gridColumns + y);
                            activeGridIndices.push_back(x * model->gridColumns + y);
                            activeCoreLayers.push_back(i);
                        }
                }
            }
            base += model->layers[i].n_blocks;
        }
    }

    std::vector<float> headroomGridTrunc(activeGridIndices.size());
    std::vector<std::vector<float>> BInvGridTrunc;

    for (i = 0; i < activeGridIndices.size(); i+=1) {
        headroomGridTrunc.at(i) = maxTemperature - temperature(model->numberSecondaryPathLayers * model->gridRows * model->gridColumns + activeGridIndices.at(i)) + 273.15;
    }

   for (i = 0; i < activeGridIndices.size(); i++) {
        std::vector<float> row;
        for(j=0; j<activeGridIndices.size(); j++)
            row.push_back(model->BInv(model->numberSecondaryPathLayers * model->gridRows * model->gridColumns + activeGridIndices.at(i), activeCoreLayers.at(j) * model->gridRows * model->gridColumns + activeGridIndices.at(j)));
        BInvGridTrunc.push_back(row);
    }
    int numGridPerCore = activeGridIndices.size()/activeIndices.size();
    cout<<"numGrid: "<<numGridPerCore<<" "<<activeGridIndices.size()<<" "<<activeIndices.size()<<endl;

    std::vector<float> powersGridTrunc = headroomGridTrunc;
    
    inplaceGauss(BInvGridTrunc, powersGridTrunc);

 /* Deriving equal power budget for all grids belonged to a certain block */
    Matrix<double, Dynamic, Dynamic> A;
    Matrix<double, Dynamic, 1> B;
    A.resize(activeGridIndices.size(),activeIndices.size());
    B.resize(activeGridIndices.size());
    for (i=0; i<activeGridIndices.size(); i++){
        B(i) = maxTemperature - temperature(model->numberSecondaryPathLayers * model->gridRows * model->gridColumns + activeGridIndices.at(i)) + 273.15;
        for(j=0; j<activeIndices.size(); j++){
            A(i,j) = 0;
            for(int k=0; k<numGridPerCore; k++)
                A(i,j) += model->BInv(model->numberSecondaryPathLayers * model->gridRows * model->gridColumns + activeGridIndices.at(i), activeCoreLayers.at(j*numGridPerCore+k) * model->gridRows * model->gridColumns + activeGridIndices.at(j*numGridPerCore+k));
        }
    }
    Matrix<double, Dynamic, 1> power2 = (A.transpose() * A).ldlt().solve(A.transpose() * B);
    /*  */

    for(i=0, base = 0; i< activeGridIndices.size(); i+=numGridPerCore) {
        double minPower = powersGridTrunc.at(i);
        double average = 0;
        double maxPower = 0;
        for(j=0; j<numGridPerCore; j++){
            //cout<< powersGridTrunc.at(i+j) <<" ";
            average +=  powersGridTrunc.at(i+j) / model->g2b_power(activeCoreLayers.at(i) * model->gridRows * model->gridColumns + activeGridIndices.at(i+j), activeCoreIndices.at(base));
            if(minPower > powersGridTrunc.at(i+j))
                minPower = powersGridTrunc.at(i+j);
            if(maxPower < powersGridTrunc.at(i+j))
                maxPower = powersGridTrunc.at(i+j);
        }
        if(method == "min"){
            model->P(activeCoreIndices.at(base),0) = (minPower*numGridPerCore >= 0) ? minPower*numGridPerCore : 0;
            powers.at(activeIndices.at(base)) = (minPower*numGridPerCore >= 0) ? minPower*numGridPerCore : 0;
        }else if(method == "max"){
            model->P(activeCoreIndices.at(base),0) = (maxPower*numGridPerCore >= 0) ? maxPower*numGridPerCore : 0;
            powers.at(activeIndices.at(base)) = (maxPower*numGridPerCore >= 0) ? maxPower*numGridPerCore : 0;
        }else if(method == "average"){
            model->P(activeCoreIndices.at(base),0) = (average/numGridPerCore >= 0) ? average/numGridPerCore : 0;
            powers.at(activeIndices.at(base)) = (average/numGridPerCore >= 0) ? average/numGridPerCore : 0;
        }else if(method == "exact"){
            model->P(activeCoreIndices.at(base)) = (power2(base)*numGridPerCore >= 0) ? power2(base)*numGridPerCore : 0;
            powers.at(activeIndices.at(base)) = (power2(base)*numGridPerCore >= 0) ? power2(base)*numGridPerCore : 0;
        }
        cout<<"Core"<<activeIndices.at(base)<<" "<<maxTemperature <<" "<< tInactive[activeIndices.at(base)] - 273.15<<" Power Budget:"<<powers.at(base)<<" (min:"<<minPower*numGridPerCore<<", avg:"<<average/numGridPerCore<<", max:"<<maxPower*numGridPerCore<<", exact:"<<power2(base)*numGridPerCore<<")"<<endl;
        base++;
    }

    temperature = model->x*model->P+model->y; 
    /**
     * @brief visualize the temperature of each component
     * 
     */
    // for(i=0; i < model->numberLayers; i++) {
    //     cout<<i<<": ";
    //     for(j=0; j < model->layers[i].n_blocks; j++) {
    //       i1 = model->layers[i].blocks[j].i1;
    //       j1 = model->layers[i].blocks[j].j1;
    //       i2 = model->layers[i].blocks[j].i2;
    //       j2 = model->layers[i].blocks[j].j2;
    //       for(int k=i1; k<i2; k++)
    //         for(int l=j1; l<j2; l++)
    //             //if(temperature(i * model->gridRows * model->gridColumns + k * model->gridColumns + l, 0) > maxTemperature+273.15)
    //             //    cout<<"Temperature of grid cell: "<<i * model->gridRows * model->gridColumns + k * model->gridColumns + l<<" for block "<< model->layers[i].blocks[j].name<<" exceeds critical temperature "<<temperature(i * model->gridRows * model->gridColumns + k * model->gridColumns + l, 0)<<endl;
    //             cout<<temperature(i * model->gridRows * model->gridColumns + k * model->gridColumns + l) - 273.15<<" ";
    //     }
    //     cout<<endl;
    // }


    return powers;  
}


/*double ThermalModel::tsp(const std::vector<bool> &activeCores) const {
    std::vector<double> powerOfInactiveCores(activeCores.size(), inactivePower);

    return tsp(activeCores, powerOfInactiveCores);
}

double ThermalModel::tsp(const std::vector<bool> &activeCores, const std::vector<double> &powerOfInactiveCores) const {
    if (activeCores.size() != coreRows * coreColumns) {
        std::cout << "\n[Scheduler][TSP][Error]: Invalid system size: " << activeCores.size() << ", expected " << (coreRows * coreColumns) << "cores." << std::endl;
		exit (1);
    }

    int amtActiveCores = 0;
    double idlePower = 0;
    for (unsigned int i = 0; i < activeCores.size(); i++) {
        if (activeCores.at(i)) {
            amtActiveCores++;
        } else {
            idlePower += powerOfInactiveCores.at(i);
        }
    }

    double minTSP = (tdp - idlePower) / amtActiveCores; // TDP constraint

    if (amtActiveCores > 0) {
        for (unsigned int core = 0; core < activeCores.size(); core++) {
            double activeSum = 0;
            double inactiveSum = 0;
            for (unsigned int i = 0; i < activeCores.size(); i++) {
                if (activeCores.at(i)) {
                    activeSum += BInv[core][i];
                } else {
                    inactiveSum += powerOfInactiveCores.at(i) * BInv[core][i];
                }
            }
            double coreSafePower = (maxTemperature - ambientTemperature - inactiveSum) / activeSum;
            minTSP = std::min(minTSP, coreSafePower);
        }
    }

    return minTSP;
}

std::vector<double> ThermalModel::tspForManyCandidates(const std::vector<bool> &activeCores, const std::vector<int> &candidates) const {
    if (activeCores.size() != coreRows * coreColumns) {
        std::cout << "\n[Scheduler][TSP][Error]: Invalid system size: " << activeCores.size() << ", expected " << (coreRows * coreColumns) << "cores." << std::endl;
		exit (1);
    }

    int amtActiveCores = count(activeCores.begin(), activeCores.end(), true) + 1; // start at one

    double idlePower = (coreRows * coreColumns - amtActiveCores) * inactivePower;
    double tdpConstraint = (tdp - idlePower) / amtActiveCores;
    std::vector<double> tsps(candidates.size(), tdpConstraint);

    if (amtActiveCores > 0) {
        for (unsigned int core = 0; core < activeCores.size(); core++) {
            double activeSum = 0;
            double inactiveSum = 0;
            for (unsigned int i = 0; i < activeCores.size(); i++) {
                if (activeCores.at(i)) {
                    activeSum += BInv[core][i];
                } else {
                    inactiveSum += BInv[core][i];
                }
            }

            for (unsigned int candidateIdx = 0; candidateIdx < candidates.size(); candidateIdx++) {
                int candidate = candidates.at(candidateIdx);
                double candActiveSum = activeSum + BInv[core][candidate];
                double candInactiveSum = inactiveSum - BInv[core][candidate];
                double coreSafePower = (maxTemperature - ambientTemperature - inactivePower * candInactiveSum) / candActiveSum;
                tsps.at(candidateIdx) = std::min(tsps.at(candidateIdx), coreSafePower);
            }
        }
    }

    return tsps;
}

double ThermalModel::worstCaseTSP(int amtActiveCores) const {
    double amtIdleCores = coreRows * coreColumns - amtActiveCores;
    double minTSP = (tdp - amtIdleCores * inactivePower) / amtActiveCores; // TDP constraint

    if (amtActiveCores > 0) {
        for (unsigned int core = 0; core < (unsigned int)(coreRows * coreColumns); core++) {
            std::vector<double> BInvRow(coreRows * coreColumns);
            for (unsigned int i = 0; i < (unsigned int)(coreRows * coreColumns); i++) {
                BInvRow.at(i) = BInv[core][i];
            }
            std::sort(BInvRow.begin(), BInvRow.end(), std::greater<double>()); // sort descending

            double activeSum = 0;
            double inactiveSum = 0;
            for (unsigned int i = 0; i < (unsigned int)(coreRows * coreColumns); i++) {
                if (i < (unsigned int)amtActiveCores) {
                    activeSum += BInvRow.at(i);
                } else {
                    inactiveSum += BInvRow.at(i) * inactivePower;
                }
            }

            double coreSafePower = (maxTemperature - ambientTemperature - inactiveSum) / activeSum;
            minTSP = std::min(minTSP, coreSafePower);
        }
    }

    return minTSP;
}
*/

/** powerBudgetMaxSteadyState
 * Return a per-core power budget that (if matched by the power consumption) heats every core exactly to the critical temperature.
 */
/*std::vector<double> ThermalModel::powerBudgetMaxSteadyState(const std::vector<bool> &activeCores) const {
    std::vector<int> activeIndices;
    std::vector<double> inactivePowers(coreRows * coreColumns, 0);
    for (unsigned int i = 0; i < coreRows * coreColumns; i++) {
        if (activeCores.at(i)) {
            activeIndices.push_back(i);
        } else {
            inactivePowers.at(i) = inactivePower;
        }
    }
    std::vector<float> tInactive = getSteadyState(inactivePowers);
    std::vector<float> headroomTrunc(activeIndices.size());
    for (unsigned int i = 0; i < activeIndices.size(); i++) {
        int index = activeIndices.at(i);
        headroomTrunc.at(i) = maxTemperature - tInactive.at(index);
    }

    std::vector<float> powersTrunc = headroomTrunc;
    std::vector<std::vector<float>> BInvTrunc;
    for (unsigned int i = 0; i < activeIndices.size(); i++) {
        std::vector<float> row;
        for (unsigned int j = 0; j < activeIndices.size(); j++) {
            row.push_back(BInv[activeIndices.at(i)][activeIndices.at(j)]);
        }
        BInvTrunc.push_back(row);
    }
    // now solve BInvTrunc * powersTrunc = headroomTrunc
    inplaceGauss(BInvTrunc, powersTrunc);

    std::vector<double> powers(coreRows * coreColumns, inactivePower);
    for (unsigned int i = 0; i < activeIndices.size(); i++) {
        powers.at(activeIndices.at(i)) = powersTrunc.at(i);
    }

    return powers;
}*/

std::vector<double> ThermalModel::ttsp(const std::vector<bool> &activeCores, std::map<std::string, double> &powerOfInactiveCores, string method) {
	
    std::ifstream f;
    f.open(transTempeFilename.c_str());
	std::string header;
    double temp;
    //std::cout << "********************************" << std::endl;
    //std::cout << "The size of active cores is " << activeCores.size() << std::endl;
    //std::cout << "powerOfInactiveCores.size() is " << powerOfInactiveCores.size() << std::endl;
    //for(auto & iter : powerOfInactiveCores){
    //    std::cout << "{\"" << iter.first << "\"," << iter.second << "}," << std::endl;
    //}
    //std::cout << "********************************" << std::endl;
    
 
    int component = 0;
    while(f >> header >> temp){
        model->Tinit(component) = temp;
        // print the component message

        // if(component < model->numberBlocks){
        //     cout<<"("<<header<<","<<model->Tinit(component) - 273.15<<") ";
        //     if(component%8 == 7)
        //         cout<<endl;
        // }
        component++;
    }

    f.close();
    
    std::vector<int> activeIndices; 
    std::vector<int> activeGridIndices;
    std::vector<int> activeCoreIndices;
    std::vector<int> activeCoreLayers;

    std::vector<double> powers(activeCores.size(), 0);
    
    //for(unsigned int coreCounter = 0; coreCounter < activeCores.size(); coreCounter++)
	//	for (auto & iter : powerOfInactiveCores){
	//		if(iter.first == "C_" + std::to_string(coreCounter) && activeCores.at(coreCounter))
	//			iter.second = 0;
	//	}
        
    int i, j, base;
    for(j=0; j< model->numberBlockThermalNodes; j++)
        model->P(j) = 0;
    for(i=0; i< model->numberLayers; i++) {
        if(model->layers[i].has_power) {
            //cout<<model->layers[i].starting_id<<", "<<model->layers[i].n_blocks<<"; ";
            for(int j=0; j< model->layers[i].n_blocks; j++) {
                for (auto & iter : powerOfInactiveCores){
			        if(iter.first == model->layers[i].blocks[j].name) 
                    {
                        model->P(model->layers[i].starting_id+j) = iter.second;
                        //cout<<model->layers[i].blocks[j].name<<" ";
                    }
		        } 
            }
        }
    }

    int i1, j1, i2, j2, numCoreLayer;
    numCoreLayer = 0;
    for(i=0, base = 0; i < model->numberLayers; i++) {
        if(model->layers[i].blocks[0].name.find('C') != string::npos && model->layers[i].has_power)
        {   
            numCoreLayer ++;
            for(j=0; j < model->layers[i].n_blocks; j++) {
                /* extent of this unit in grid cell units	*/
                i1 = model->layers[i].blocks[j].i1;
                j1 = model->layers[i].blocks[j].j1;
                i2 = model->layers[i].blocks[j].i2;
                j2 = model->layers[i].blocks[j].j2;

                if(activeCores.at(base+j)){
                    activeCoreIndices.push_back(model->layers[i].starting_id+j);
                    activeIndices.push_back(base+j);
                    for(int x=i1; x<i2; x++)
                        for(int y=j1; y<j2; y++){
                            //activeGridIndices.push_back(i * model->gridRows * model->gridColumns + x * model->gridColumns + y);
                            activeGridIndices.push_back(x * model->gridColumns + y);
                            activeCoreLayers.push_back(i);
                        }
                }
            }
            base += model->layers[i].n_blocks;
        }
    }
    
    Matrix<double, Dynamic,1> power = model->g2b_power * model->P;
    cout<<endl<<power.rows()<<endl;
    for (i = 0; i < activeGridIndices.size(); i++)
        power(activeCoreLayers.at(i) * model->gridRows * model->gridColumns + activeGridIndices.at(i)) = 0;
    Matrix<double, Dynamic,1> Toffset = model->Tthreshold - model->psi * model->g2b_temp * model->Tinit - model->phi * power;

    Matrix<double, Dynamic,Dynamic> ExpGrid;
    Matrix<double, Dynamic,1> HeadroomGrid;
    ExpGrid.resize(activeGridIndices.size(),activeGridIndices.size());
    HeadroomGrid.resize(activeGridIndices.size());
    for (i = 0; i < activeGridIndices.size(); i++) {
        HeadroomGrid(i) = Toffset(model->numberSecondaryPathLayers * model->gridRows * model->gridColumns + activeGridIndices.at(i));
        for(j=0; j<activeGridIndices.size(); j++)
            ExpGrid(i,j) = model->phi(model->numberSecondaryPathLayers * model->gridRows * model->gridColumns + activeGridIndices.at(i), activeCoreLayers.at(j) * model->gridRows * model->gridColumns + activeGridIndices.at(j));
    }
    Matrix<double, Dynamic,Dynamic> res = ExpGrid.inverse() * HeadroomGrid;

    int numGridPerCore = activeGridIndices.size()/activeIndices.size();

    /* Deriving equal power budget for all grids belonged to a certain block */
    Matrix<double, Dynamic, Dynamic> A;
    Matrix<double, Dynamic, 1> B;
    A.resize(activeGridIndices.size(),activeIndices.size());
    B.resize(activeGridIndices.size());
    for (i=0; i<activeGridIndices.size(); i++){
        B(i) = Toffset(model->numberSecondaryPathLayers * model->gridRows * model->gridColumns + activeGridIndices.at(i));
        for(j=0; j<activeIndices.size(); j++){
            A(i,j) = 0;
            for(int k=0; k<numGridPerCore; k++)
                A(i,j) += model->phi(model->numberSecondaryPathLayers * model->gridRows * model->gridColumns + activeGridIndices.at(i), activeCoreLayers.at(j*numGridPerCore+k) * model->gridRows * model->gridColumns + activeGridIndices.at(j*numGridPerCore+k));
        }
    }
    Matrix<double, Dynamic, 1> power2 = (A.transpose() * A).ldlt().solve(A.transpose() * B);
    /*  */


    for(i=0, base = 0; i< activeGridIndices.size(); i+=numGridPerCore, base++) {
        double minPower = res(i);
        double average = 0;
        double maxPower = 0;
        for(j=0; j<numGridPerCore; j++){
            //cout<< powersGridTrunc.at(i+j) <<" ";
            average +=  res(i+j) / model->g2b_power(activeCoreLayers.at(i) * model->gridRows * model->gridColumns + activeGridIndices.at(i+j), activeCoreIndices.at(base));
            if(minPower > res(i+j))
                minPower = res(i+j);
            if(maxPower < res(i+j))
                maxPower = res(i+j);
        }
        if(method == "min"){
            model->P(activeCoreIndices.at(base),0) = (minPower*numGridPerCore >= 0) ? minPower*numGridPerCore : 0;
            powers.at(activeIndices.at(base)) = (minPower*numGridPerCore >= 0) ? minPower*numGridPerCore : 0;
        }else if(method == "max"){
            model->P(activeCoreIndices.at(base),0) = (maxPower*numGridPerCore >= 0) ? maxPower*numGridPerCore : 0;
            powers.at(activeIndices.at(base)) = (maxPower*numGridPerCore >= 0) ? maxPower*numGridPerCore : 0;
        }else if(method == "average"){
            model->P(activeCoreIndices.at(base),0) = (average/numGridPerCore >= 0) ? average/numGridPerCore : 0;
            powers.at(activeIndices.at(base)) = (average/numGridPerCore >= 0) ? average/numGridPerCore : 0;
        }else if(method == "exact"){
            model->P(activeCoreIndices.at(base)) = (power2(base)*numGridPerCore >= 0) ? power2(base)*numGridPerCore : 0;
            powers.at(activeIndices.at(base)) = (power2(base)*numGridPerCore >= 0) ? power2(base)*numGridPerCore : 0;
        }
        cout<<"Core"<<activeIndices.at(base)<<" Power Budget:"<<powers.at(base)<<" (min:"<<minPower*numGridPerCore<<", avg:"<<average/numGridPerCore<<", max:"<<maxPower*numGridPerCore<<", exact:"<<power2(base)*numGridPerCore<<")"<<endl;
    }

    Matrix<double, Dynamic,Dynamic> temperature = model->psi * model->g2b_temp * model->Tinit + model->phi * model->g2b_power * model->P;
    
    /**
     * @brief Print the temperature of all layers ---T-TSP
     * 
     */
    // for(i=0; i < model->numberLayers; i++) {
    //     cout<<i<<": ";
    //     for(j=0; j < model->layers[i].n_blocks; j++) {
    //       i1 = model->layers[i].blocks[j].i1;
    //       j1 = model->layers[i].blocks[j].j1;
    //       i2 = model->layers[i].blocks[j].i2;
    //       j2 = model->layers[i].blocks[j].j2;
    //       for(int k=i1; k<i2; k++)
    //         for(int l=j1; l<j2; l++)
    //             //if(temperature(i * model->gridRows * model->gridColumns + k * model->gridColumns + l, 0) > maxTemperature+273.15)
    //             //    cout<<"Temperature of grid cell: "<<i * model->gridRows * model->gridColumns + k * model->gridColumns + l<<" for block "<< model->layers[i].blocks[j].name<<" exceeds critical temperature "<<temperature(i * model->gridRows * model->gridColumns + k * model->gridColumns + l, 0)<<endl;
    //             cout<<temperature(i * model->gridRows * model->gridColumns + k * model->gridColumns + l, 0) - 273.15<<" ";
    //     }
    //     cout<<endl;
    // }
    
    /* Computing Thermal Transmission Delay Between Top and Bottom Layers */
    /*Matrix<double, Dynamic,1> Tsteady = model->x*model->P+model->y;

    Matrix<double, Dynamic, Dynamic> aux;
    aux.resize(model->numberGridThermalNodes,model->numberGridThermalNodes);
	for(unsigned int i = 0; i < model->numberGridThermalNodes; i++){
        model->Tthreshold(i) = maxTemperature + 273.15;
		for(unsigned int j = 0; j < model->numberGridThermalNodes; j++){
			aux(i,j) = (model->C(i,j) != 0) ? -model->C(i,j) : 0;
        }
    }

    EigenSolver< Matrix<double, Dynamic, Dynamic> > es(aux);
	Matrix<double, Dynamic, Dynamic> eigenValuesAux = es.pseudoEigenvalueMatrix();
	Matrix<double, Dynamic, Dynamic> eigenVectorsAux = es.pseudoEigenvectors();
	Matrix<double, Dynamic, Dynamic> eigenVectorsInvAux = eigenVectorsAux.inverse();
	Matrix<double, Dynamic, Dynamic> exponentials; 
    exponentials.resize(model->numberGridThermalNodes,model->numberGridThermalNodes);
    Matrix<double, Dynamic, 1> t_max; 
    t_max.resize(model->gridColumns*model->gridRows);

    for(i=0; i<1; i++){
        t_max(i) = 0;
        for(j=0; j<100; j++){
            for(int k = 0; k < model->numberGridThermalNodes; k++){
                for(int l = 0; l < model->numberGridThermalNodes; l++)
                    exponentials(k,l) = 0;
            }
            int step = 0;
            for(int k = 0; k < model->numberGridThermalNodes-1;){
                if(eigenValuesAux(k,k+1) != 0){
                    exponentials(k,k) = pow((double)M_E, eigenValuesAux(k,k) * t_max(i))*cos(eigenValuesAux(k,k+1) * t_max(i));
                    exponentials(k,k+1) = pow((double)M_E, eigenValuesAux(k,k) * t_max(i))*sin(eigenValuesAux(k,k+1) * t_max(i));
                    exponentials(k+1,k) = - pow((double)M_E, eigenValuesAux(k,k) * t_max(i))*sin(abs(eigenValuesAux(k,k+1)) * t_max(i)); 
                    exponentials(k+1,k+1) = pow((double)M_E, eigenValuesAux(k,k) * t_max(i))*cos(abs(eigenValuesAux(k,k+1)) * t_max(i));
                    step = 2;
                }else{
                    exponentials(k,k) = pow((double)M_E, eigenValuesAux(k,k) * t_max(i));
                    if(k == model->numberGridThermalNodes-2)
                        exponentials(k+1,k+1) = pow((double)M_E, eigenValuesAux(k+1,k+1) * t_max(i)); 
                    step = 1;
                }
                k+=step;
            }

            Matrix<double, Dynamic, 1> auxtemp = eigenVectorsAux * eigenValuesAux * exponentials * eigenVectorsInvAux * (model->g2b_temp* model->Tinit - Tsteady);
            Matrix<double, Dynamic, 1> auxsecondtemp = eigenVectorsAux * eigenValuesAux * eigenValuesAux * exponentials * eigenVectorsInvAux * (model->g2b_temp * model->Tinit - Tsteady);
            double numerator = auxtemp(19 * model->gridRows * model->gridColumns + i);
            double denominator = auxsecondtemp(19 * model->gridRows * model->gridColumns + i);
            Matrix<double, Dynamic, 1> T = Tsteady + eigenVectorsAux * exponentials * eigenVectorsInvAux * (model->g2b_temp* model->Tinit - Tsteady);
            cout<<t_max(i)<<","<< T(19 * model->gridRows * model->gridColumns + i)<<","<<Tsteady(19 * model->gridRows * model->gridColumns + i)<<endl;
            t_max(i) = t_max(i) - numerator/denominator;
            if((Tsteady(19 * model->gridRows * model->gridColumns + i) - T(19 * model->gridRows * model->gridColumns + i))/Tsteady(19 * model->gridRows * model->gridColumns + i) < 0.001)
                break;
        }
    }

    Matrix<double, Dynamic, 1> t_max2; 
    t_max2.resize(model->gridColumns*model->gridRows);
    for(i=0; i<1; i++){
        t_max2(i) = 0;
        for(j=0; j<100; j++){
            for(int k = 0; k < model->numberGridThermalNodes; k++){
                for(int l = 0; l < model->numberGridThermalNodes; l++)
                    exponentials(k,l) = 0;
            }
            int step = 0;
            for(int k = 0; k < model->numberGridThermalNodes-1;){
                if(eigenValuesAux(k,k+1) != 0){
                    exponentials(k,k) = pow((double)M_E, eigenValuesAux(k,k) * t_max2(i))*cos(eigenValuesAux(k,k+1) * t_max2(i));
                    exponentials(k,k+1) = pow((double)M_E, eigenValuesAux(k,k) * t_max2(i))*sin(eigenValuesAux(k,k+1) * t_max2(i));
                    exponentials(k+1,k) = - pow((double)M_E, eigenValuesAux(k,k) * t_max2(i))*sin(abs(eigenValuesAux(k,k+1)) * t_max2(i)); 
                    exponentials(k+1,k+1) = pow((double)M_E, eigenValuesAux(k,k) * t_max2(i))*cos(abs(eigenValuesAux(k,k+1)) * t_max2(i));
                    step = 2;
                }else{
                    exponentials(k,k) = pow((double)M_E, eigenValuesAux(k,k) * t_max2(i));
                    if(k == model->numberGridThermalNodes-2)
                        exponentials(k+1,k+1) = pow((double)M_E, eigenValuesAux(k+1,k+1) * t_max2(i)); 
                    step = 1;
                }
                k+=step;
            }

            Matrix<double, Dynamic, 1> auxtemp = eigenVectorsAux * eigenValuesAux * exponentials * eigenVectorsInvAux * (model->g2b_temp* model->Tinit - Tsteady);
            Matrix<double, Dynamic, 1> auxsecondtemp = eigenVectorsAux * eigenValuesAux * eigenValuesAux * exponentials * eigenVectorsInvAux * (model->g2b_temp * model->Tinit - Tsteady);
            double numerator = auxtemp(3 * model->gridRows * model->gridColumns + i);
            double denominator = auxsecondtemp(3 * model->gridRows * model->gridColumns + i);
            Matrix<double, Dynamic, 1> T = Tsteady + eigenVectorsAux * exponentials * eigenVectorsInvAux * (model->g2b_temp* model->Tinit - Tsteady);
            cout<<t_max2(i)<<","<< T(3 * model->gridRows * model->gridColumns + i)<<","<<Tsteady(3 * model->gridRows * model->gridColumns + i)<<endl;
            t_max2(i) = t_max2(i) - numerator/denominator;
            if((Tsteady(3 * model->gridRows * model->gridColumns + i) - T(3 * model->gridRows * model->gridColumns + i))/Tsteady(3 * model->gridRows * model->gridColumns + i) < 0.001)
                break;
        }
    }
    cout<<"time difference:"<< t_max2(0) - t_max(0)<<endl;*/
    /* */  



    return powers;  
}
// int main(){
//     std::cout << "This is a test " << std::endl;
//     return 0;
// }
