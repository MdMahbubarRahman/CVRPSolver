#include <iostream>
#include <vector>
#include <list>
#include <set>
#include <Queue>
#include <map>

#include "HungarianAlg.h"
#include "OperatorTheory.h"

#ifndef BRANCHANDBOUND_H
#define BRANCHANDBOUND_H

enum BBSolverStatus {
	Optimal, SubOptimal, LocalOptimal, TerminatedWithErrors
};

struct TourSolution {
	double objValue;
	std::vector<int> tourSolution;
};

struct Incumbent {
	std::list<BasicCell> incumbentSolution;
	std::vector<int> tour;
	double objValue;
};

struct TSPSolution {
	double cost;
	std::vector<int> tour;
};

struct BBTree {
	int currentNumOfNodes;
	int numOfTourSolution;
	int numOfNodePrunedByBound;
	int numOfNodePrunedByIntegrality;
	int numOfNodePrunedWithErrorsOrInfeasibility;
	std::list<Node> branchNodes;
};

//Branch and Bound Solver for solving traveling salesman problem
class BranchAndBoundSolver{
private:
	double lowerBound;
	double weakerLowerBound;
	double upperBound;
	double totalTime;
	double maxTimeLimit;
	BBTree bbTree;
	BBSolverStatus bStatus;
	Incumbent incumbent;
	std::list<TourSolution> tourSolutions;
	TSPSolution tspSolution;
	std::vector<std::vector<double>> costTableau;
	std::vector<int> oldListOfCities;
	std::vector<int> newListOfCities;
	std::map<int, int> newCityToOldCityMap;
	std::map<int, int> assignmentSolution;
	std::list<BasicCell> transportationBasicSolution;
	std::list<std::vector<int>> routeLists;	
public:
	BranchAndBoundSolver();
	BranchAndBoundSolver(std::vector<int> initialTourOfCities, std::vector<std::vector<double>> wholeCostMatrix);
	BranchAndBoundSolver(const BranchAndBoundSolver & depthFirstBBSolver);
	void solveAssignmentProblem();
	void generateBasicTransportationSolution();
	void initBranchAndBoundTree();
	void solveNodeByCostOperator(Node node);
	TSPSolution getTSPSolution();
	void pruneNodeByIntegrality();
	void pruneNodesWithNegativeDelta();
	void checkForBBOptimality();
	Node selectNodeBasedOnBestWeakerLowerBound();
	void runBranchAndBoundSolver();
};

#endif

