#include <iostream>
#include <iterator>
#include <cmath>
#include <vector>
#include <list>
#include <map>
#include <fstream>
#include <string>
#include <sstream>

#include "Tabusearch.h"
#include "Geneticalgorithm.h"
#include "BranchAndBound.h"
#include "OperatorTheory.h"
#include "HungarianAlg.h"
#include "CVRPSolution.h"

const int POPULATION_SIZE = 50;
const int NUMBER_OF_NODES = 60;//cus+sats
const int DEPOT_NODE = 0;
const int CAPACITY_LIMIT = 10;
const int KCHAIN_LENGTH = 5;
const int SWAPCHAIN_LENGTH = 5;
const int TRUCK_CAP_LIMIT = 20;
const int UAV_CAP_LIMIT = 3;
const int MAX_NO_TRUCK = 5;
const int MAX_NO_UAV = 3;

int main() {

    std::fstream myFile;
    //open raod distance data file
    myFile.open("DistanceMatrix.txt", std::ios::in);//read
    std::vector<std::vector<double>> roadDistance;
    if (myFile.is_open()) {
        std::string line, word;
        std::istringstream iss;
        int rowNum = 0;
        while (std::getline(myFile, line)) {
            if (rowNum > 0) {
                std::vector<double> dist;
                iss.clear();
                iss.str(line);
                int colNum = 0;
                while (iss.good()) {
                    iss >> word;
                    //&& colNum <= NUMBER_OF_NODES
                    if (colNum > 0) {
                        double abc = std::stod(word);//abc == road distance
                        //std::cout << abc << std::endl;
                        dist.push_back(abc);
                    }
                    colNum += 1;
                }
                roadDistance.push_back(dist);
            }
            rowNum += 1;
        }
        myFile.close();
    }

    std::cout << "Populate a vector containing customers." << std::endl;
    std::vector<int> customerCluster;
    for (int i = 5; i <= 25; i++) {
        customerCluster.push_back(i);
    }

    std::cout << "Populate customer to demand map." << std::endl;
    std::map<int, int> cusToDemand;
    for (int i = 5; i <= 25; i++) {
        cusToDemand.insert(std::pair<int, int>(i, 1));//unit demand
    }

    Geneticalgorithm genAlg;
    genAlg = Geneticalgorithm(DEPOT_NODE, TRUCK_CAP_LIMIT, cusToDemand, roadDistance, customerCluster);
    genAlg.runGeneticAlgorithm();
    Chromosome chrom = genAlg.getGASolution();
    std::set<int> customers;
    for (auto it : customerCluster) {
        customers.insert(it);
    }
    CVRPSolution cvrpSol = CVRPSolution(chrom, customers, cusToDemand);
    cvrpSol.showSolution();
    return 0;
}

