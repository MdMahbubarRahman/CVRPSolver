#include "HungarianAlg.h"

//default constructor
HungarianAlg::HungarianAlg() {
	reducedCost = 0.0;
	baseValue = 0.0;
	initSolCost = 0.0;
	finalSolCost = 0.0;
	tspOptimal = false;
	assignmentOptimal = false;
	reducedCostMatrixSize = 0;
}

//copy constructor
HungarianAlg::HungarianAlg(const HungarianAlg& hunAlg) {
	reducedCost = hunAlg.reducedCost;
	baseValue = hunAlg.baseValue;
	initSolCost = hunAlg.initSolCost;
	finalSolCost = hunAlg.finalSolCost;
	tspOptimal = hunAlg.tspOptimal;
	reducedCostMatrixSize = hunAlg.reducedCostMatrixSize;
	boxPoints = hunAlg.boxPoints;
	assignmentOptimal = hunAlg.assignmentOptimal;
	listOfRoutes = hunAlg.listOfRoutes;
	initialTsp = hunAlg.initialTsp;
	costMatrix = hunAlg.costMatrix;
	reducedCostMatrix = hunAlg.reducedCostMatrix;
}

//constructor
HungarianAlg::HungarianAlg(std::vector<int> initTsp, std::vector<std::vector<double>> cMatrix) {
	reducedCost = 0.0;
	baseValue = 0.0;
	initSolCost = 0.0;
	finalSolCost = 0.0;
	tspOptimal = false;
	assignmentOptimal = false;
	initialTsp = initTsp;
	costMatrix = cMatrix;
	reducedCostMatrixSize = initialTsp.size();
	reducedCostMatrix = costMatrix;
	for (int i = 0; i < reducedCostMatrixSize - 1; i++) {
		initSolCost += costMatrix[initialTsp.at(i)][initialTsp.at(i + 1)];
	}
	initSolCost += costMatrix[initialTsp.at(reducedCostMatrixSize - 1)][initialTsp.at(0)];
}

//performs row reduction
void HungarianAlg::performRowColumnReduction() {
	std::cout << "\nPerform row reduction" << std::endl;
	for (int i = 0; i < reducedCostMatrixSize; i++) {
		double minVal = INFINITY;
		for (int j = 0; j < reducedCostMatrixSize; j++) {
			if (minVal > reducedCostMatrix[i][j]) {
				minVal = reducedCostMatrix[i][j];
			}
		}
		for (int j = 0; j < reducedCostMatrixSize; j++) {
			reducedCostMatrix[i][j] = reducedCostMatrix[i][j] - minVal;
		}
		reducedCost += minVal;
	}

	std::cout << "\nPerform column reduction." << std::endl;
	for (int i = 0; i < reducedCostMatrixSize; i++) {
		double minVal = INFINITY;
		for (int j = 0; j < reducedCostMatrixSize; j++) {
			if (minVal > reducedCostMatrix[j][i]) {
				minVal = reducedCostMatrix[j][i];
			}
		}
		for (int j = 0; j < reducedCostMatrixSize; j++) {
			reducedCostMatrix[j][i] = reducedCostMatrix[j][i] - minVal;
		}
		reducedCost += minVal;
	}
	reducedCost == initSolCost ? tspOptimal = true : tspOptimal = false;
}

//perform row scanning
void HungarianAlg::rowScanning(std::set<int>& coveredRows, std::set<int>& coveredColumns, std::map<int, int>& boxPoints) {
	std::cout << "\nRow scanning in progress!" << std::endl;
	for (int i = 0; i < reducedCostMatrixSize; i++) {
		auto it = coveredRows.find(i);
		if (it == coveredRows.end()) {
			int numOfZeros = 0;
			int colPos = 0;
			for (int j = 0; j < reducedCostMatrixSize; j++) {
				auto iter = coveredColumns.find(j);
				if (iter == coveredColumns.end() && reducedCostMatrix[i][j] == 0) {
					numOfZeros += 1;
					colPos = j;
				}
			}
			if (numOfZeros == 1) {
				coveredColumns.insert(colPos);
				boxPoints.insert(std::pair<int, int>(i, colPos));
				//std::cout << "The new box point is : " << i << " , " << colPos << std::endl;
			}
		}
	}
}

//perform column scanning
void HungarianAlg::columnScanning(std::set<int>& coveredRows, std::set<int>& coveredColumns, std::map<int, int>& boxPoints) {
	std::cout << "\nColumn scanning in progress!" << std::endl;
	for (int i = 0; i < reducedCostMatrixSize; i++) {
		auto it = coveredColumns.find(i);
		if (it == coveredColumns.end()) {
			int numOfZeros = 0;
			int rowPos = 0;
			for (int j = 0; j < reducedCostMatrixSize; j++) {
				auto iter = coveredRows.find(j);
				if (iter == coveredRows.end() && reducedCostMatrix[j][i] == 0) {
					numOfZeros += 1;
					rowPos = j;
				}
			}
			if (numOfZeros == 1) {
				coveredRows.insert(rowPos);
				boxPoints.insert(std::pair<int, int>(rowPos, i));
				//std::cout << "The new box point is : " << rowPos << " , " << i << std::endl;
			}
		}
	}
}

//scan for the number of uncovered zeros
int HungarianAlg::uncoveredZeroScanning(std::set<int>& coveredRows, std::set<int>& coveredColumns) {
	std::cout << "\nNumber of uncovered zero scanning in progress!" << std::endl;
	std::map<int, int> numZeroInRow;
	std::map<int, int> numZeroInCol;
	std::cout << "\nUpdate number of uncovered zeros in each row." << std::endl;
	for (int i = 0; i < reducedCostMatrixSize; i++) {
		auto itr = coveredRows.find(i);
		if (itr == coveredRows.end()) {
			int numZero = 0;
			for (int j = 0; j < reducedCostMatrixSize; j++) {
				auto it = coveredColumns.find(j);
				if (it == coveredColumns.end() && reducedCostMatrix[i][j] == 0) {
					numZero += 1;
				}
			}
			numZeroInRow.insert(std::pair<int, int>(i, numZero));
		}
	}
	std::cout << "\nUpdate number of uncovered zeros in each column." << std::endl;
	for (int i = 0; i < reducedCostMatrixSize; i++) {
		auto itr = coveredColumns.find(i);
		if (itr == coveredColumns.end()) {
			int numZero = 0;
			for (int j = 0; j < reducedCostMatrixSize; j++) {
				auto it = coveredRows.find(j);
				if (it == coveredRows.end() && reducedCostMatrix[j][i] == 0) {
					numZero += 1;
				}
			}
			numZeroInCol.insert(std::pair<int, int>(i, numZero));
		}
	}
	int numOfZeros = 0;
	for (auto& it : numZeroInCol) {
		numOfZeros += it.second;
	}
	//std::cout << "\nNumber of uncovered zeros : " << numOfZeros << std::endl;

	return numOfZeros;
}

//perform diagonal selection in case more than one zero in a row
void HungarianAlg::diagonalSelectionAdv(std::set<int>& coveredRows, std::set<int>& coveredColumns, std::map<int, int>& boxPoints) {
	std::map<int, int> uncoveredRowMapWithZeros;
	std::map<int, int> uncoveredColumnMapWithZeros;
	int iter1 = 0;
	int iter2 = 0;
	for (int i = 0; i < reducedCostMatrixSize; i++) {
		auto it = coveredRows.find(i);
		if (it == coveredRows.end()) {
			int numZero = 0;
			for (int j = 0; j < reducedCostMatrixSize; j++) {
				auto itt = coveredColumns.find(j);
				if (itt == coveredColumns.end() && reducedCostMatrix[i][j] == 0) {
					numZero += 1;
				}
			}
			if (numZero > 1) {
				uncoveredRowMapWithZeros.insert(std::pair<int, int>(iter1, i));
				iter1 += 1;
			}
		}
	}
	for (int i = 0; i < reducedCostMatrixSize; i++) {
		auto it = coveredColumns.find(i);
		if (it == coveredColumns.end()) {
			int numZero = 0;
			for (int j = 0; j < reducedCostMatrixSize; j++) {
				auto itt = coveredRows.find(j);
				if (itt == coveredRows.end() && reducedCostMatrix[j][i] == 0) {
					numZero += 1;
				}
			}
			if (numZero > 1) {
				uncoveredColumnMapWithZeros.insert(std::pair<int, int>(iter2, i));
				iter2 += 1;
			}
		}
	}
	std::cout << "\nDiagonal selection in progress!" << std::endl;
	while (!uncoveredRowMapWithZeros.empty()) {
		bool flag1 = true;
		bool flag2 = true;
		int rowID = 10000;
		int colID = 10000;
		int difVal = 0;
		//perform first zero selection
		for (auto& it : uncoveredRowMapWithZeros) {
			for (auto& itt : uncoveredColumnMapWithZeros) {
				if (reducedCostMatrix[it.second][itt.second] == 0) {
					boxPoints.insert(std::pair<int, int>(it.second, itt.second));
					coveredColumns.insert(itt.second);
					//std::cout << "The new box point is : " << it.second << " , " << itt.second << std::endl;
					rowID = it.first;
					colID = itt.first;
					difVal = itt.first - it.first;
					flag1 = false;
					break;
				}
			}
			if (flag1 == false) {
				break;
			}
		}
		rowID != 10000 ? uncoveredRowMapWithZeros.erase(rowID) : rowID = 10000;
		//perform diagonal opposite zero selection
		if (!uncoveredRowMapWithZeros.empty()) {
			for (auto& it : uncoveredRowMapWithZeros) {
				if (it.first > rowID) {
					for (auto& itt : uncoveredColumnMapWithZeros) {
						int val = itt.first - it.first;
						if (itt.first > colID && val == difVal && reducedCostMatrix[it.second][itt.second] == 0) {
							boxPoints.insert(std::pair<int, int>(it.second, itt.second));
							coveredColumns.insert(itt.second);
							rowID = it.first;
							//std::cout << "The new box point is : " << it.second << " , " << itt.second << std::endl;
							flag2 = false;
							break;
						}
					}
				}
				if (flag2 == false) {
					break;
				}
			}
		}
		rowID != 10000 ? uncoveredRowMapWithZeros.erase(rowID) : rowID = 10000;
	}
}

//cover zero values and check for optimal assignment problem solution
void HungarianAlg::coverMinimumValueAssignmentsAndCheckOptimality(bool& assignmentOptimal, std::set<int>& coveredRows, std::set<int>& coveredColumns, std::map<int, int>& boxPoints, std::list<IntersectionPoint>& twiceCoveredPoints) {
	int numZero1 = 1000;
	int numZero2 = 0;
	int iter = 0;
	while (numZero1 > 0) {
		rowScanning(coveredRows, coveredColumns, boxPoints);
		columnScanning(coveredRows, coveredColumns, boxPoints);
		numZero2 = uncoveredZeroScanning(coveredRows, coveredColumns);
		if (numZero1 == numZero2) {
			iter += 1;
		}
		else {
			numZero1 = numZero2;
			iter = 0;
		}
		if (numZero1 > 1 && iter == 1) {
			diagonalSelectionAdv(coveredRows, coveredColumns, boxPoints);
			iter = 0;
		}
	}
	std::cout << "\nShow the intersection points" << std::endl;
	for (auto i : coveredRows) {
		for (auto j : coveredColumns) {
			IntersectionPoint point = IntersectionPoint(i, j);
			//std::cout << "Row : " << i << " Column : " << j << std::endl;
			twiceCoveredPoints.push_back(point);
		}
	}
	//check assignment optimality
	std::cout << "\nNumber of rows covered : " << coveredRows.size() << ", Number of columns covered : " << coveredColumns.size() << std::endl;
	if ((coveredColumns.size() + coveredRows.size()) == reducedCostMatrixSize) {
		assignmentOptimal = true;
		std::cout << "\nAssignment optimal : " << assignmentOptimal << std::endl;
	}
}

//performs row column reduction for uncovered nonzero cells
void HungarianAlg::performRowColumnReductionForUncoveredCells(bool& assignmentOptimal, std::set<int>& coveredRows, std::set<int>& coveredColumns, std::map<int, int>& boxPoints, std::list<IntersectionPoint>& twiceCoveredPoints) {
	std::set<int> uncoveredRows, uncoveredColumns;
	for (int i = 0; i < reducedCostMatrixSize; i++) {
		auto it = coveredRows.find(i);
		if (it == coveredRows.end()) {
			uncoveredRows.insert(i);
		}
	}
	for (int i = 0; i < reducedCostMatrixSize; i++) {
		auto it = coveredColumns.find(i);
		if (it == coveredColumns.end()) {
			uncoveredColumns.insert(i);
		}
	}
	double minVal = INFINITY;
	for (auto it : uncoveredRows) {
		for (auto itt : uncoveredColumns) {
			if (minVal > reducedCostMatrix[it][itt]) {
				minVal = reducedCostMatrix[it][itt];
			}
		}
	}
	for (auto it : uncoveredRows) {
		for (auto itt : uncoveredColumns) {
			reducedCostMatrix[it][itt] = reducedCostMatrix[it][itt] - minVal;
		}
		reducedCost += minVal;
	}
	for (auto& point : twiceCoveredPoints) {
		reducedCostMatrix[point.row][point.column] = reducedCostMatrix[point.row][point.column] + minVal;
	}
}

//generates routes from assignment solution
void HungarianAlg::populateListOfRoutes(std::map<int, int> boxPoints) {
	std::vector<int> route;
	int sourceNode = 0;
	int destinationNode = 0;
	int routeStartNode = 0;
	std::map<int, int> newAssignments;
	newAssignments = boxPoints;
	int i = 0;
	int counter = 0;
	while (!newAssignments.empty()) {
		counter++;
		if (counter > 2 * boxPoints.size()) {
			hStatus = HungarianAlgTerminatedWithErrors;
			break;
		}
		if (i == 0) {
			auto it = newAssignments.begin();
			routeStartNode = (*it).first;
			destinationNode = (*it).second;
			route.push_back(destinationNode);
			newAssignments.erase(routeStartNode);
			i += 1;
		}
		else {
			sourceNode = destinationNode;
			destinationNode = newAssignments[sourceNode];
			route.push_back(destinationNode);
			newAssignments.erase(sourceNode);
			if (destinationNode == routeStartNode) {
				listOfRoutes.push_back(route);
				route.clear();
				i = 0;
			}
			else {
				i += 1;
			}
		}
	}
	if (hStatus != HungarianAlgTerminatedWithErrors) {
		hStatus = HungarianAlgTerminatedSuccessfully;
	}
}

//solves relaxed tsp problem which is an assignment problem and generates lower bound of TSP
void HungarianAlg::runHungarianAlg() {
	std::set<int> coveredRows;
	std::set<int> coveredColumns;
	std::list<IntersectionPoint> twiceCoveredPoints;
	performRowColumnReduction();
	if (tspOptimal == true) {
		std::cout << "\nThe initial tsp solution is optimal! No need to look for optimal tsp solution." << std::endl;
	}
	else {
		while (assignmentOptimal != true) {
			coverMinimumValueAssignmentsAndCheckOptimality(assignmentOptimal, coveredRows, coveredColumns, boxPoints, twiceCoveredPoints);
			if (assignmentOptimal != true) {
				performRowColumnReductionForUncoveredCells(assignmentOptimal, coveredRows, coveredColumns, boxPoints, twiceCoveredPoints);
				coveredColumns.clear();
				coveredRows.clear();
				boxPoints.clear();
				twiceCoveredPoints.clear();
			}
			else {
				std::cout << "\nOptimum assignment is found!" << std::endl;
				populateListOfRoutes(boxPoints);
				//showAssignmentSolution();
			}
		}
	}
}

//returns assignment solution
std::map<int, int> HungarianAlg::getAssignmentSolution() {
	return boxPoints;
}

//returns list of routes
std::list<std::vector<int>> HungarianAlg::getListOfRoutes() {
	return listOfRoutes;
}

//shows results
void HungarianAlg::showAssignmentSolution() {
	std::cout << "\nShow assignments : " << std::endl;
	for (auto& it : boxPoints) {
		std::cout << it.first << " --> " << it.second << std::endl;
	}
	std::cout << "\nShow the routes : " << std::endl;
	for (auto& it : listOfRoutes) {
		for (auto itt : it) {
			std::cout << itt << " ";
		}
		std::cout << ";" << std::endl;
	}
}

HungarianAlgStatus HungarianAlg::getHungarianAlgStatus() {
	return hStatus;
}