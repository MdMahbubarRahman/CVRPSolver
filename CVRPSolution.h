#include <iostream>
#include <map>
#include <vector>
#include <set>
#include <list>
#include <queue>

#include"Geneticalgorithm.h"

#ifndef CVRPSOLUTION_H
#define CVRPSOLUTION_H

//class cvrp solution 
class CVRPSolution {
private:
	std::vector<int> solution;
	std::set<int> customers;
	std::map<int, int> customerTodemand;
	double solCost = 0;
	int satelliteNode = 0;
	int maxRouteCapacity = 0;
	int totalDemandSatisfied = 0;
	int numberOfRoutes = 0;
public:
	CVRPSolution();
	CVRPSolution(const CVRPSolution& sol); 
	CVRPSolution(std::vector<int> sol, std::set<int> customers, std::map<int, int> customerTodemand, double cost, int satelliteNode, int maxRouteCapacity, int totalDemandSatisfied, int numberOfRoutes);//constructor
	CVRPSolution(Chromosome& chromSol, std::set<int> customers, std::map<int, int> customerTodemand); //constructor
	void showSolution() const;
	void showCustomers();
	void showCustomerTodemandMap();
	void showSolCost();
	void showSatelliteNode();
	void showMaxRouteCapacity();
	void showTotalDemandSatisfied();
	void showNumberOfRoutes();
	std::vector<int> getSolution();
	int getSatelliteNode();
	double getCost();
	int getMaxRouteCapacity();
	std::set<int> getCustomers();
	std::map<int, int> getCustomerTodemandMap();
	int getTotalDemandSatisfied();
	int getNumberOfRoutes();
};

#endif //CVRPSOLUTION_H
