#include <iostream>
#include <vector>
#include <list>
#include <set>
#include <Queue>
#include <map>

#ifndef HUNGARIANALG_H
#define HUNGARIANALG_H

//The Hungarian algorithm solves assignment problem which is a relaxation of the traveling salesman problem.
//Hungarian algorithm generates either a TSP tour or a number of subtours.

enum HungarianAlgStatus {
	HungarianAlgTerminatedSuccessfully, HungarianAlgTerminatedWithErrors
};

struct IntersectionPoint {
	int row;
	int column;
	IntersectionPoint(int a, int b) {
		row = a;
		column = b;
	}
};


class HungarianAlg {
private:
	double reducedCost;
	double baseValue;
	double initSolCost;
	double finalSolCost;
	bool tspOptimal;
	int reducedCostMatrixSize;
	std::map<int, int> boxPoints;
	bool assignmentOptimal;
	std::list<std::vector<int>> listOfRoutes;
	std::vector<int> initialTsp;
	std::vector<std::vector<double>> costMatrix;
	std::vector<std::vector<double>> reducedCostMatrix;
	HungarianAlgStatus hStatus;
public:
	HungarianAlg();
	HungarianAlg(const HungarianAlg & hunAlg);
	HungarianAlg(std::vector<int> initialTsp, std::vector<std::vector<double>> costMatrix);
	void performRowColumnReduction();
	void rowScanning(std::set<int>& coveredRows, std::set<int>& coveredColumns, std::map<int, int>& boxPoints);
	void columnScanning(std::set<int>& coveredRows, std::set<int>& coveredColumns, std::map<int, int>& boxPoints);
	void diagonalSelectionAdv(std::set<int>& coveredRows, std::set<int>& coveredColumns, std::map<int, int>& boxPoints);
	int uncoveredZeroScanning(std::set<int>& coveredRows, std::set<int>& coveredColumns);
	void coverMinimumValueAssignmentsAndCheckOptimality(bool& assignmentOptimal, std::set<int>& coveredRows, std::set<int>& coveredColumns, std::map<int, int>& boxPoints, std::list<IntersectionPoint>& twiceCoveredPoints);
	void performRowColumnReductionForUncoveredCells(bool& assignmentOptimal, std::set<int>& coveredRows, std::set<int>& coveredColumns, std::map<int, int>& boxPoints, std::list<IntersectionPoint>& twiceCoveredPoints);
	void populateListOfRoutes(std::map<int, int> boxPoints);
	void showAssignmentSolution();
	std::map<int, int> getAssignmentSolution();
	std::list<std::vector<int>> getListOfRoutes();
	void runHungarianAlg();
};

#endif