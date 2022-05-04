#include "OperatorTheory.h"

//default constructor
OperatorTheory::OperatorTheory() {
	std::cout << "\nThe default constructor has been called!" << std::endl;
	nodeIndex = 0;
	parentNodeIndex = 0;
	isSolutionATour = false;
	weakerLowerBound = INFINITY;
	lowerBound = INFINITY;
	delta = 0.0;
}

OperatorTheory::OperatorTheory(Node node) {
	nodeIndex = node.nodeIndex;
	parentNodeIndex = node.parentNodeIndex;
	isSolutionATour = node.isSolutionATour;
	weakerLowerBound = node.weakerLowerBound;
	lowerBound = node.lowerBound;
	delta = node.delta;
	Ip = node.Ip;
	Iq = node.Iq;
	Jp = node.Jp;
	Jq = node.Jq;
	branchOnCell = node.branchOnCell;
	enteringCell = node.enteringCell;
	basicSolution = node.basicSolution;
	rowWiseDualSolution = node.rowDualSolution;
	columnWiseDualSolution = node.columnDualSolution;
	costTableau = node.costTableau;
}

//default operator to perform cost operator from tsp solution
OperatorTheory::OperatorTheory(std::list<BasicCell> basicSol, std::vector<std::vector<double>> cTableau) {
	//populate all other values using these two input values
	nodeIndex = 0;
	parentNodeIndex = 0;
	isSolutionATour = false;
	weakerLowerBound = INFINITY;
	lowerBound = INFINITY;
	delta = 0.0;
	basicSolution = basicSol;
	costTableau = cTableau;
	for (int i = 0; i < costTableau.size(); i++) {
		rowWiseDualSolution.push_back(0);
		columnWiseDualSolution.push_back(0);
	}
}

//constructor
OperatorTheory::OperatorTheory(std::list<BasicCell> basicSol, std::vector<double> rowWiseDualSol, std::vector<double> columnWiseDualSol, std::vector<std::vector<double>> cTableau) {
	std::cout << "\nThe operator theory constructor has been called!" << std::endl;
	nodeIndex = 0;
	parentNodeIndex = 0;
	isSolutionATour = false;
	weakerLowerBound = INFINITY;
	lowerBound = INFINITY;
	delta = 0.0;
	basicSolution = basicSol;
	rowWiseDualSolution = rowWiseDualSol;
	columnWiseDualSolution = columnWiseDualSol;
	costTableau = cTableau;
}

//copy constructor
OperatorTheory::OperatorTheory(const OperatorTheory& opThr) {
	std::cout << "\nThe copy constructor has been called!" << std::endl;
	nodeIndex = opThr.nodeIndex;
	parentNodeIndex = opThr.parentNodeIndex;
	isSolutionATour = opThr.isSolutionATour;
	weakerLowerBound = opThr.weakerLowerBound;
	lowerBound = opThr.lowerBound;
	delta = opThr.delta;
	Ip = opThr.Ip;
	Iq = opThr.Iq;
	Jp = opThr.Jp;
	Jq = opThr.Jq;
	branchOnCell = opThr.branchOnCell;
	basicSolution = opThr.basicSolution;
	rowWiseDualSolution = opThr.rowWiseDualSolution;
	columnWiseDualSolution = opThr.columnWiseDualSolution;
	costTableau = opThr.costTableau;
	childNodes = opThr.childNodes;
	cStatus = opThr.cStatus;
}

//generate initial dual solution
void OperatorTheory::generateDualSolution() {
	std::multimap<int, int> rowColMapForBasicSolution;
	std::multimap<int, int> colRowMapForBasicSolution;
	for (auto& it : basicSolution) {
		rowColMapForBasicSolution.insert(std::pair<int, int>(it.rowID, it.colID));
		colRowMapForBasicSolution.insert(std::pair<int, int>(it.colID, it.rowID));
	}
	//generate dual solution
	bool dualflag = false;
	std::map<int, int> mapForRowDualSolution;
	std::map<int, int> mapForColumnDualSolution;
	for (int i = 0; i < costTableau.size(); i++) {
		mapForRowDualSolution.insert(std::pair<int, int>(i, 0));
		mapForColumnDualSolution.insert(std::pair<int, int>(i, 0));
	}
	mapForRowDualSolution.erase(0);
	mapForRowDualSolution.insert(std::pair<int, int>(0, 1));
	rowWiseDualSolution[0] = 0;
	while (!dualflag) {
		//update column dual solution
		for (auto& it : mapForRowDualSolution) {
			if (it.second == 1) {
				for (auto itt = rowColMapForBasicSolution.lower_bound(it.first); itt != rowColMapForBasicSolution.upper_bound(it.first); itt++) {
					int col = (*itt).second;
					if (mapForColumnDualSolution[col] == 0) {
						mapForColumnDualSolution.erase(col);
						mapForColumnDualSolution.insert(std::pair<int, int>(col, 1));
						double rowDualVal = rowWiseDualSolution.at(it.first);
						double cellCost = costTableau[(*itt).first][col];
						double colDualVal = cellCost - rowDualVal;
						columnWiseDualSolution[col] = colDualVal;
					}
				}
			}
		}
		//update row dual solution
		for (auto& it : mapForColumnDualSolution) {
			if (it.second == 1) {
				for (auto itt = colRowMapForBasicSolution.lower_bound(it.first); itt != colRowMapForBasicSolution.upper_bound(it.first); itt++) {
					int row = (*itt).second;
					if (mapForRowDualSolution[row] == 0) {
						mapForRowDualSolution.erase(row);
						mapForRowDualSolution.insert(std::pair<int, int>(row, 1));
						double colDualVal = columnWiseDualSolution.at(it.first);
						double cellCost = costTableau[row][(*itt).first];
						double rowDualVal = cellCost - colDualVal;
						rowWiseDualSolution[row] = rowDualVal;
					}
				}
			}
		}
		//check whether dual solution generation is complete
		int counter = 0;
		for (auto& it : mapForColumnDualSolution) {
			if (it.second == 1) {
				counter += 1;
			}
		}
		for (auto& it : mapForRowDualSolution) {
			if (it.second == 1) {
				counter += 1;
			}
		}
		//std::cout << "\nThe number of dual solutions found : " << counter << std::endl;
		if (counter == (2 * costTableau.size())) {
			dualflag = true;
		}
	}
	/*
	std::cout << "\nShow the dual solutions." << std::endl;
	std::cout << "\nThe row dual solution are : " << std::endl;
	for (auto & it:rowWiseDualSolution) {
		std::cout << it << std::endl;
	}
	std::cout << "\nThe column dual solution are : " << std::endl;
	for (auto& it : columnWiseDualSolution) {
		std::cout << it << std::endl;
	}
	*/
}

//scanning routine to populate Ip, Iq, Jp, Jq sets
void OperatorTheory::scanningRoutine(int p, int q) {
	std::vector<int> rowLabel;
	std::vector<int> columnLabel;
	int cellRowID = p;
	int cellColumnID = q;
	int size = costTableau.size();
	bool stopFlag = false;
	int iter = 0;
	//Populate rowlabel and columnlabel with  default values;
	for (int i = 0; i < size; i++) {
		rowLabel.push_back(0);
		columnLabel.push_back(0);
	}
	//Run scanning routine
	bool newColumnLabel = false;
	bool newRowLabel = false;
	while (!stopFlag) {
		iter += 1;
		if (iter == 1) {
			//std::cout << "\nLabel with 1 the columns of basis cells in row p cexept for basis cell (p,q); label row p with 2" << std::endl;
			for (auto it = basicSolution.begin(); it != basicSolution.end(); it++) {
				if ((*it).rowID == cellRowID && (*it).colID != cellColumnID) {
					columnLabel[(*it).colID] = 1;
					newColumnLabel = true;
				}
			}
			rowLabel[cellRowID] = 2;
		}
		//std::cout << "\nIf no new columns were labelled 1 on the preceding step go to 5. otherwise go to 2." << std::endl;
		if (newColumnLabel != true) {
			break;
		}
		//std::cout << "\nFor each column labelled with 1, label with 1 each row containing a basis cell in that column unless the row is labelled with 2; then change the label of the column to 2." << std::endl;
		for (int i = 0; i < size; i++) {
			if (columnLabel[i] == 1) {
				for (auto it = basicSolution.begin(); it != basicSolution.end(); ++it) {
					if ((*it).colID == i && rowLabel[(*it).rowID] != 2) {
						rowLabel[(*it).rowID] = 1;
						newRowLabel = true;
					}
				}
				columnLabel[i] = 2;
			}
		}
		newColumnLabel = false;
		//std::cout << "\nIf no new rows were labelled with 1 on the preceeding step go to 5. Otherwise go to 4." << std::endl;
		if (newRowLabel != true) {
			break;
		}
		//std::cout << "\nFor each row labelled with 1, label with 1 each column containing a basis cell in that row unless the column is labelled with 2; then change the label of the row to 2. Go to 1." << std::endl;
		for (int i = 0; i < size; i++) {
			if (rowLabel[i] == 1) {
				for (auto it = basicSolution.begin(); it != basicSolution.end(); ++it) {
					if ((*it).rowID == i && columnLabel[(*it).colID] != 2) {
						columnLabel[(*it).colID] = 1;
						newColumnLabel = true;
					}
				}
				rowLabel[i] = 2;
			}
		}
		newRowLabel = false;
	}
	//std::cout << "\nStop. The set Ip(Jp) consists of the rows(columns) labelled with 2; the set Iq(Jq) consists of the unlabelled rows(columns)." << std::endl;
	/*
	std::cout << "\nShow the contents of the I, J sets." << std::endl;
	std::cout << "\nIp set elements : " << std::endl;
	for (auto& it : Ip) {
		std::cout << it << std::endl;
	}
	std::cout << "\nIq set elements : " << std::endl;
	for (auto& it : Iq) {
		std::cout << it << std::endl;
	}
	std::cout << "\nJp set elements : " << std::endl;
	for (auto& it : Jp) {
		std::cout << it << std::endl;
	}
	std::cout << "\nJq set elements : " << std::endl;
	for (auto& it : Jq) {
		std::cout << it << std::endl;
	}
	*/
	std::cout << "\nClear I, J sets if they are nonempty." << std::endl;
	if (!Ip.empty()) {
		Ip.clear();
	}
	if (!Iq.empty()) {
		Iq.clear();
	}
	if (!Jp.empty()) {
		Jp.clear();
	}
	if (!Jq.empty()) {
		Jq.clear();
	}
	for (int i = 0; i < size; i++) {
		rowLabel[i] == 2 ? Ip.insert(i) : Iq.insert(i);
		columnLabel[i] == 2 ? Jp.insert(i) : Jq.insert(i);
	}
	/*
	std::cout << "\nShow the updated contents of the I, J sets." << std::endl;
	std::cout << "\nIp set elements : " << std::endl;
	for (auto & it: Ip) {
		std::cout << it << std::endl;
	}
	std::cout << "\nIq set elements : " << std::endl;
	for (auto& it : Iq) {
		std::cout << it << std::endl;
	}
	std::cout << "\nJp set elements : " << std::endl;
	for (auto& it : Jp) {
		std::cout << it << std::endl;
	}
	std::cout << "\nJq set elements : " << std::endl;
	for (auto& it : Jq) {
		std::cout << it << std::endl;
	}
	*/
}

//update dual variables
void OperatorTheory::updateDualSolution(double val) {
	/*
	std::cout << "\nShow dual solutions." << std::endl;
	std::cout << "\nShow row dual solution : " << std::endl;
	for (auto it : rowWiseDualSolution) {
		std::cout << it << std::endl;
	}
	std::cout << "\nShow column dual solution : " << std::endl;
	for (auto it : columnWiseDualSolution) {
		std::cout << it << std::endl;
	}
	*/
	//Dual solutions for basis preserving cost operators \sigma Cpq+
	std::vector<double> transformedRowWiseDualSolution;
	std::vector<double> transformedColumnWiseDualSolution;
	for (int i = 0; i < costTableau.size(); i++) {
		transformedRowWiseDualSolution.push_back(0);
		transformedColumnWiseDualSolution.push_back(0);
	}
	int size = costTableau.size();
	double delta = val;
	//update row dual variables
	for (int i = 0; i < size; i++) {
		auto it = Ip.find(i);
		if (it != Ip.end()) {
			transformedRowWiseDualSolution[i] = rowWiseDualSolution.at(i) + delta;
		}
		else {
			transformedRowWiseDualSolution[i] = rowWiseDualSolution.at(i);
		}
	}
	//update column dual variables
	for (int i = 0; i < size; i++) {
		auto it = Jp.find(i);
		if (it != Jp.end()) {
			transformedColumnWiseDualSolution[i] = columnWiseDualSolution.at(i) - delta;
		}
		else {
			transformedColumnWiseDualSolution[i] = columnWiseDualSolution.at(i);
		}
	}
	rowWiseDualSolution = transformedRowWiseDualSolution;
	columnWiseDualSolution = transformedColumnWiseDualSolution;
	/*
	std::cout << "\nShow updated dual solutions." << std::endl;
	std::cout << "\nShow row dual solution : " << std::endl;
	for (auto it: rowWiseDualSolution) {
		std::cout << it << std::endl;
	}
	std::cout << "\nShow column dual solution : " << std::endl;
	for (auto it : columnWiseDualSolution) {
		std::cout << it << std::endl;
	}
	*/
}

//find max delta and find the potential entering cells
void OperatorTheory::findMaxDeltaAndEnteringCell(int p, int q) {
	double maxDelta = INFINITY;
	int cellRowID = p;
	int cellColumnID = q;
	int enteringCellRowID = 0;
	int enteringCellColumnID = 0;
	double val = 0;
	for (auto it : Ip) {
		for (auto itt : Jq) {
			if (it == cellRowID && itt == cellColumnID) {
				continue;
			}
			else {
				val = costTableau[it][itt] - rowWiseDualSolution[it] - columnWiseDualSolution[itt];
				if (val < maxDelta) {
					maxDelta = val;
					enteringCellRowID = it;
					enteringCellColumnID = itt;
				}
			}
		}
	}
	//std::cout << "\nThe entering cell row id : " << enteringCellRowID << " " << "column id : " << enteringCellColumnID << std::endl;
	delta = maxDelta;
	Cell cell = Cell();
	cell.rowID = enteringCellRowID;
	cell.colID = enteringCellColumnID;
	enteringCell = cell;
	//std::cout << "\nDelta value : " << delta << std::endl;
}



//find max delta and find the potential entering cells
void OperatorTheory::findMaxDeltaAndEnteringCellWithForbiddenCell(int p, int q, Cell forbiddenCell) {
	double maxDelta = INFINITY;
	int cellRowID = p;
	int cellColumnID = q;
	int enteringCellRowID = 0;
	int enteringCellColumnID = 0;
	Cell bannedCell = forbiddenCell;
	double val = 0;
	for (auto it : Ip) {
		for (auto itt : Jq) {
			if (it == cellRowID && itt == cellColumnID) {
				continue;
			}
			else if (it == bannedCell.rowID && itt == bannedCell.colID) {
				continue;
			}
			else {
				val = costTableau[it][itt] - rowWiseDualSolution[it] - columnWiseDualSolution[itt];
				if (val < maxDelta) {
					maxDelta = val;
					enteringCellRowID = it;
					enteringCellColumnID = itt;
				}
			}
		}
	}
	//std::cout << "\nThe entering cell row id : " << enteringCellRowID << " " << "column id : " << enteringCellColumnID << std::endl;
	delta = maxDelta;
	Cell cell = Cell();
	cell.rowID = enteringCellRowID;
	cell.colID = enteringCellColumnID;
	enteringCell = cell;
	//std::cout << "\nDelta value : " << delta << std::endl;
}

//generates cycle or loop containing entering and leaving cells
void OperatorTheory::generateCycleWithEnteringCell(int enCellRowId, int enCellColID) {
	//std::cout << "\nEntering cell row id : " << enCellRowId << ", col id : " << enCellColID << std::endl;
	std::multimap<int, int> rowColMapForBasicSolution;
	std::multimap<int, int> colRowMapForBasicSolution;
	std::map<int, std::map<int, double>> cellToValueMap;
	for (auto& it : basicSolution) {
		rowColMapForBasicSolution.insert(std::pair<int, int>(it.rowID, it.colID));
		colRowMapForBasicSolution.insert(std::pair<int, int>(it.colID, it.rowID));
		cellToValueMap[it.rowID][it.colID] = it.value;
	}
	std::list<AllocatedCell> cycleOfAllocatedCells;
	std::list<AllocatedCell> deletedAllocatedCells;
	int enteringCellRowID = enCellRowId;
	int enteringCellColumnID = enCellColID;
	cellToValueMap[enteringCellRowID][enteringCellColumnID] = 0.0;
	rowColMapForBasicSolution.insert(std::pair<int, int>(enteringCellRowID, enteringCellColumnID));
	colRowMapForBasicSolution.insert(std::pair<int, int>(enteringCellColumnID, enteringCellRowID));

	//Entering cell cofig as allocated cell
	AllocatedCell enteringCell = AllocatedCell();
	enteringCell.cellProperty.rowID = enteringCellRowID;
	enteringCell.cellProperty.colID = enteringCellColumnID;
	enteringCell.cellProperty.value = 0.0;
	enteringCell.cellType = Getter;
	enteringCell.prevCell.rowID = enteringCellRowID;
	enteringCell.prevCell.colID = enteringCellColumnID;
	enteringCell.postCell.rowID = NULL;
	enteringCell.postCell.colID = NULL;
	AllocatedCell currentCell = enteringCell;
	bool cycleComplete = false;
	bool enteringCellRowCovered = false;
	bool enteringCellColumnCovered = false;
	int counter = 0;
	/*
	std::cout << "\nShow current basic solution : " << std::endl;
	for (auto & it: basicSolution) {
		std::cout << "\nRow id : " << it.rowID << ", Col id : " << it.colID << ", value : " << it.value << std::endl;
	}
	*/
	while (!cycleComplete) {
		if (counter > 2) {
			abnormalNode = true;
			break;
		}
		if (abnormalNode == true) {
			break;
		}
		//std::cout << "\nCurrent Cell row ID : " << currentCell.cellProperty.rowID << ", current Cell col ID : " << currentCell.cellProperty.colID << std::endl;
		if ((currentCell.prevCell.rowID == currentCell.cellProperty.rowID) && (currentCell.prevCell.colID == currentCell.cellProperty.colID)) {
			//searach row 
			std::list<int> listOfColumns;
			std::list<int> listOfRows;
			for (auto it = rowColMapForBasicSolution.lower_bound(currentCell.cellProperty.rowID); it != rowColMapForBasicSolution.upper_bound(currentCell.cellProperty.rowID); it++) {
				if ((*it).second != currentCell.cellProperty.colID) {
					listOfColumns.push_back((*it).second);
				}
			}
			// search column
			for (auto it = colRowMapForBasicSolution.lower_bound(currentCell.cellProperty.colID); it != colRowMapForBasicSolution.upper_bound(currentCell.cellProperty.colID); it++) {
				if ((*it).second != currentCell.cellProperty.rowID) {
					listOfRows.push_back((*it).second);
				}
			}
			//choose post cell and update current cell
			if (listOfRows.size() <= listOfColumns.size()) {
				double val = NULL;
				for (auto it : listOfRows) {
					val = cellToValueMap[it][currentCell.cellProperty.colID];
					if (val == 1.0) {
						currentCell.postCell.rowID = it;
						currentCell.postCell.colID = currentCell.cellProperty.colID;
						enteringCellColumnCovered = true;
						break;
					}
				}
			}
			else {
				double val = NULL;
				for (auto it : listOfColumns) {
					val = cellToValueMap[currentCell.cellProperty.rowID][it];
					if (val == 1.0) {
						currentCell.postCell.rowID = currentCell.cellProperty.rowID;
						currentCell.postCell.colID = it;
						enteringCellRowCovered = true;
						break;
					}
				}
			}
			//populate post cell as an allocated cell
			AllocatedCell newAllocatedCell = AllocatedCell();
			newAllocatedCell.cellProperty.rowID = currentCell.postCell.rowID;
			newAllocatedCell.cellProperty.colID = currentCell.postCell.colID;
			newAllocatedCell.cellProperty.value = cellToValueMap[newAllocatedCell.cellProperty.rowID][newAllocatedCell.cellProperty.colID];
			newAllocatedCell.cellType = Giver;
			newAllocatedCell.prevCell.rowID = currentCell.cellProperty.rowID;
			newAllocatedCell.prevCell.colID = currentCell.cellProperty.colID;
			newAllocatedCell.postCell.rowID = NULL;
			newAllocatedCell.postCell.colID = NULL;
			cycleOfAllocatedCells.push_back(currentCell);
			currentCell = newAllocatedCell;
		}
		else if ((currentCell.prevCell.rowID == currentCell.cellProperty.rowID) && (currentCell.prevCell.colID != currentCell.cellProperty.colID)) {
			int rowId = NULL;
			std::vector<int> rowList;
			for (auto it = colRowMapForBasicSolution.lower_bound(currentCell.cellProperty.colID); it != colRowMapForBasicSolution.upper_bound(currentCell.cellProperty.colID); it++) {
				if (((*it).second != currentCell.cellProperty.rowID) && ((*it).second == enteringCellRowID)) {
					rowId = enteringCellRowID;
					enteringCellRowCovered = true;
				}
			}
			if (rowId == NULL) {
				for (auto it = colRowMapForBasicSolution.lower_bound(currentCell.cellProperty.colID); it != colRowMapForBasicSolution.upper_bound(currentCell.cellProperty.colID); it++) {
					if ((*it).second != currentCell.cellProperty.rowID) {
						rowList.push_back((*it).second);
					}
				}
				if (rowList.size() == 0) {
					counter++;
					std::cout << "\nThe zero row size case has appeared!" << std::endl;
					walkBackThroughTheCycleAndChooseNewAllocatedCell(currentCell, cycleOfAllocatedCells, deletedAllocatedCells, rowColMapForBasicSolution, colRowMapForBasicSolution, cellToValueMap);
					continue;
				}
				else if (rowList.size() == 1) {
					rowId = rowList.at(0);
				}
				else {
					if (currentCell.cellType == Giver) {
						double val = NULL;
						for (auto it : rowList) {
							val = cellToValueMap[it][currentCell.cellProperty.colID];
							if (val == 0.0) {
								rowId = it;
								break;
							}
						}

					}
					else if (currentCell.cellType == Getter) {
						double val = NULL;
						for (auto it : rowList) {
							val = cellToValueMap[it][currentCell.cellProperty.colID];
							if (val == 1.0) {
								rowId = it;
								break;
							}
						}
					}
				}
			}
			currentCell.postCell.colID = currentCell.cellProperty.colID;
			currentCell.postCell.rowID = rowId;
			if ((currentCell.postCell.rowID == enteringCellRowID) && (currentCell.postCell.colID == enteringCellColumnID)) {
				cycleComplete = true;
				cycleOfAllocatedCells.push_back(currentCell);
			}
			else {
				//populate new allocated cell
				AllocatedCell newAllocatedCell = AllocatedCell();
				newAllocatedCell.cellProperty.rowID = currentCell.postCell.rowID;
				newAllocatedCell.cellProperty.colID = currentCell.postCell.colID;
				newAllocatedCell.cellProperty.value = cellToValueMap[newAllocatedCell.cellProperty.rowID][newAllocatedCell.cellProperty.colID];
				currentCell.cellType == Getter ? newAllocatedCell.cellType = Giver : newAllocatedCell.cellType = Getter;
				newAllocatedCell.prevCell.rowID = currentCell.cellProperty.rowID;
				newAllocatedCell.prevCell.colID = currentCell.cellProperty.colID;
				newAllocatedCell.postCell.rowID = NULL;
				newAllocatedCell.postCell.colID = NULL;
				cycleOfAllocatedCells.push_back(currentCell);
				currentCell = newAllocatedCell;
			}
			if (enteringCellRowCovered == true && enteringCellColumnCovered == true) {
				currentCell.postCell.rowID = enteringCellRowID;
				currentCell.postCell.colID = enteringCellColumnID;
				cycleOfAllocatedCells.push_back(currentCell);
				break;
			}
		}
		else if ((currentCell.prevCell.colID == currentCell.cellProperty.colID) && (currentCell.prevCell.rowID != currentCell.cellProperty.rowID)) {
			int colId = NULL;
			std::vector<int> colList;
			for (auto it = rowColMapForBasicSolution.lower_bound(currentCell.cellProperty.rowID); it != rowColMapForBasicSolution.upper_bound(currentCell.cellProperty.rowID); it++) {
				if (((*it).second != currentCell.cellProperty.colID) && ((*it).second == enteringCellColumnID)) {
					colId = enteringCellColumnID;
					enteringCellColumnCovered = true;
				}
			}
			if (colId == NULL) {
				for (auto it = rowColMapForBasicSolution.lower_bound(currentCell.cellProperty.rowID); it != rowColMapForBasicSolution.upper_bound(currentCell.cellProperty.rowID); it++) {
					if ((*it).second != currentCell.cellProperty.colID) {
						colList.push_back((*it).second);
					}
				}
				if (colList.size() == 0) {
					counter++;
					std::cout << "\nThe zero column size case has appeared!" << std::endl;
					walkBackThroughTheCycleAndChooseNewAllocatedCell(currentCell, cycleOfAllocatedCells, deletedAllocatedCells, rowColMapForBasicSolution, colRowMapForBasicSolution, cellToValueMap);
					continue;
				}
				else if (colList.size() == 1) {
					colId = colList.at(0);
				}
				else {
					if (currentCell.cellType == Giver) {
						double val = NULL;
						for (auto it : colList) {
							val = cellToValueMap[currentCell.cellProperty.rowID][it];
							if (val == 0.0) {
								colId = it;
								break;
							}
						}

					}
					else if (currentCell.cellType == Getter) {
						double val = NULL;
						for (auto it : colList) {
							val = cellToValueMap[currentCell.cellProperty.rowID][it];
							if (val == 1.0) {
								colId = it;
								break;
							}
						}
					}
				}
			}
			currentCell.postCell.colID = colId;
			currentCell.postCell.rowID = currentCell.cellProperty.rowID;
			if ((currentCell.postCell.rowID == enteringCellRowID) && (currentCell.postCell.colID == enteringCellColumnID)) {
				cycleComplete = true;
				cycleOfAllocatedCells.push_back(currentCell);
			}
			else {
				//populate new allocated cell
				AllocatedCell newAllocatedCell = AllocatedCell();
				newAllocatedCell.cellProperty.rowID = currentCell.postCell.rowID;
				newAllocatedCell.cellProperty.colID = currentCell.postCell.colID;
				newAllocatedCell.cellProperty.value = cellToValueMap[newAllocatedCell.cellProperty.rowID][newAllocatedCell.cellProperty.colID];
				currentCell.cellType == Getter ? newAllocatedCell.cellType = Giver : newAllocatedCell.cellType = Getter;
				newAllocatedCell.prevCell.rowID = currentCell.cellProperty.rowID;
				newAllocatedCell.prevCell.colID = currentCell.cellProperty.colID;
				newAllocatedCell.postCell.rowID = NULL;
				newAllocatedCell.postCell.colID = NULL;
				cycleOfAllocatedCells.push_back(currentCell);
				currentCell = newAllocatedCell;
			}
			if (enteringCellRowCovered == true && enteringCellColumnCovered == true) {
				currentCell.postCell.rowID = enteringCellRowID;
				currentCell.postCell.colID = enteringCellColumnID;
				cycleOfAllocatedCells.push_back(currentCell);
				break;
			}
		}
	}
	/*
	std::cout << "\nShow the cycle : " << std::endl;
	for (auto & it: cycleOfAllocatedCells) {
		std::cout << "\nrow id : " << it.cellProperty.rowID << " column id : " << it.cellProperty.colID << " cell type : " << it.cellType << " allocated value : " << it.cellProperty.value << std::endl;
	}
	*/
	if (!abnormalNode) {
		cycleOfCells = cycleOfAllocatedCells;
	}
}

//fix the current cycle if it ends up being a tree
void OperatorTheory::walkBackThroughTheCycleAndChooseNewAllocatedCell(AllocatedCell& currentCell, std::list<AllocatedCell>& cycleOfAllocatedCells, std::list<AllocatedCell>& deletedAllocatedCells, std::multimap<int, int>& rowColMapForBasicSolution, std::multimap<int, int>& colRowMapForBasicSolution, std::map<int, std::map<int, double>>& cellToValueMap) {
	bool newCellSelected = false;
	int counter = 0;
	while (!newCellSelected) {
		counter++;
		if (counter > 10) {
			abnormalNode = true;
			break;
		}
		//delete all the deleted allocated cells whose precedence cell is the current cell from the deletedAllocatedCells list
		std::list<std::list<AllocatedCell>::iterator> iterList;
		std::list<AllocatedCell>::iterator iter;
		if (!deletedAllocatedCells.empty()) {
			//std::cout << "\nInside first if block." << std::endl;
			for (auto it = deletedAllocatedCells.begin(); it != deletedAllocatedCells.end(); it++) {
				if ((*it).prevCell.rowID == currentCell.cellProperty.rowID && (*it).prevCell.colID == currentCell.cellProperty.colID) {
					iterList.push_back(it);
				}
			}
		}
		while (!iterList.empty()) {
			//std::cout << "\nInside while block." << std::endl;
			auto it = iterList.begin();
			iter = (*it);
			deletedAllocatedCells.erase(iter);
			iterList.erase(it);
		}
		deletedAllocatedCells.push_back(currentCell);
		//look for a new allocated cell 
		if (currentCell.cellProperty.rowID == currentCell.prevCell.rowID && currentCell.cellProperty.colID != currentCell.prevCell.colID) {
			std::list<int> colList;
			for (auto it = rowColMapForBasicSolution.lower_bound(currentCell.prevCell.rowID); it != rowColMapForBasicSolution.upper_bound(currentCell.prevCell.rowID); it++) {
				if ((*it).second != currentCell.cellProperty.colID && (*it).second != currentCell.prevCell.colID) {
					bool deletedBefore = false;
					for (auto& itt : deletedAllocatedCells) {
						if (itt.cellProperty.rowID == currentCell.cellProperty.rowID && itt.cellProperty.colID == (*it).second) {
							deletedBefore = true;
						}
					}
					if (deletedBefore == false) {
						colList.push_back((*it).second);
					}
				}
			}
			if (!colList.empty()) {
				auto colIter = colList.begin();
				int colId = (*colIter);
				AllocatedCell newAllCell = AllocatedCell();
				newAllCell.cellProperty.rowID = currentCell.cellProperty.rowID;
				newAllCell.cellProperty.colID = colId;
				newAllCell.cellProperty.value = cellToValueMap[newAllCell.cellProperty.rowID][newAllCell.cellProperty.colID];
				newAllCell.cellType = currentCell.cellType;
				newAllCell.prevCell = currentCell.prevCell;
				newAllCell.postCell.rowID = NULL;
				newAllCell.postCell.colID = NULL;
				currentCell = newAllCell;
				newCellSelected = true;
			}
			else {
				newCellSelected = false;
				AllocatedCell prevAllocatedCell = AllocatedCell();
				std::list<AllocatedCell>::iterator cycleIterator;
				for (auto it = cycleOfAllocatedCells.begin(); it != cycleOfAllocatedCells.end(); it++) {
					if ((*it).cellProperty.rowID == currentCell.prevCell.rowID && (*it).cellProperty.colID == currentCell.prevCell.colID) {
						cycleIterator = it;
					}
				}
				prevAllocatedCell = (*cycleIterator);
				cycleOfAllocatedCells.erase(cycleIterator);
				currentCell = prevAllocatedCell;
				currentCell.postCell.rowID = NULL;
				currentCell.postCell.colID = NULL;
			}
		}
		else if (currentCell.cellProperty.rowID != currentCell.prevCell.rowID && currentCell.cellProperty.colID == currentCell.prevCell.colID) {
			std::list<int> rowList;
			for (auto it = colRowMapForBasicSolution.lower_bound(currentCell.prevCell.colID); it != colRowMapForBasicSolution.upper_bound(currentCell.prevCell.colID); it++) {
				if ((*it).second != currentCell.cellProperty.rowID && (*it).second != currentCell.prevCell.rowID) {
					bool deletedBefore = false;
					for (auto& itt : deletedAllocatedCells) {
						if (itt.cellProperty.rowID == (*it).second && itt.cellProperty.colID == currentCell.cellProperty.colID) {
							deletedBefore = true;
						}
					}
					if (deletedBefore == false) {
						rowList.push_back((*it).second);
					}
				}
			}
			if (!rowList.empty()) {
				auto rowIter = rowList.begin();
				int rowId = (*rowIter);
				AllocatedCell newAllCell = AllocatedCell();
				newAllCell.cellProperty.rowID = rowId;
				newAllCell.cellProperty.colID = currentCell.cellProperty.colID;
				newAllCell.cellProperty.value = cellToValueMap[newAllCell.cellProperty.rowID][newAllCell.cellProperty.colID];
				newAllCell.cellType = currentCell.cellType;
				newAllCell.prevCell = currentCell.prevCell;
				newAllCell.postCell.rowID = NULL;
				newAllCell.postCell.colID = NULL;
				currentCell = newAllCell;
				newCellSelected = true;
			}
			else {
				newCellSelected = false;
				AllocatedCell prevAllocatedCell = AllocatedCell();
				std::list<AllocatedCell>::iterator cycleIterator;
				for (auto it = cycleOfAllocatedCells.begin(); it != cycleOfAllocatedCells.end(); it++) {
					if ((*it).cellProperty.rowID == currentCell.prevCell.rowID && (*it).cellProperty.colID == currentCell.prevCell.colID) {
						cycleIterator = it;
					}
				}
				prevAllocatedCell = (*cycleIterator);
				cycleOfAllocatedCells.erase(cycleIterator);
				currentCell = prevAllocatedCell;
				currentCell.postCell.rowID = NULL;
				currentCell.postCell.colID = NULL;
			}
		}
	}
}


//check wheter the generated cycle is feasible and updates basaic solution and cost tableau
void OperatorTheory::checkCycleFeasibililtyAndUpdateBasicSolutionCostTableau() {
	double minVal = INFINITY;
	isCycleFeasible = false;
	std::cout << "\nFind the minimum value of the giver cells." << std::endl;
	for (auto& it : cycleOfCells) {
		if (it.cellType == Giver) {
			if (it.cellProperty.value < minVal) {
				minVal = it.cellProperty.value;
			}
		}
	}
	minVal >= 1.0 ? isCycleFeasible = true : isCycleFeasible = false;
	if (isCycleFeasible == true) {
		std::cout << "\nUpdate cell values as the cycle is feasible." << std::endl;
		for (auto& it : cycleOfCells) {
			//std::cout << " row id : " << it.cellProperty.rowID << " col id : " << it.cellProperty.colID << " value : " << it.cellProperty.value << std::endl;
			if (it.cellType == Giver) {
				it.cellProperty.value -= minVal;
			}
			else if (it.cellType == Getter) {
				it.cellProperty.value += minVal;
			}
			//std::cout << " row id : " << it.cellProperty.rowID << " col id : " << it.cellProperty.colID << " value : " << it.cellProperty.value << std::endl;
		}
		std::cout << "\nUpdate basic solution." << std::endl;
		std::list<BasicCell> solution;
		std::map<int, std::map<int, double>> cellMap;
		for (auto& it : cycleOfCells) {
			if (it.cellProperty.rowID == branchOnCell.rowID && it.cellProperty.colID == branchOnCell.colID) {
				continue;
			}
			else {
				solution.push_back(it.cellProperty);
				cellMap[it.cellProperty.rowID][it.cellProperty.colID] = 1.0;
			}
		}
		for (auto& it : basicSolution) {
			if (cellMap[it.rowID][it.colID] != 1) {
				if (it.rowID == branchOnCell.rowID && it.colID == branchOnCell.colID) {
					continue;
				}
				else {
					solution.push_back(it);
				}
			}
		}
		basicSolution = solution;
	}
	else {
		std::cout << "\nNo need to change basic solution." << std::endl;
	}
	/*
	std::cout << "\nPrint cost tableau." << std::endl;
	for (int i = 0; i < costTableau.size(); i++) {
		for (int j = 0; j < costTableau.size(); j++) {
			std::cout << costTableau[i][j] << "  ";
		}
		std::cout << " " << std::endl;
	}
	*/
	isCycleFeasible == true ? costTableau[branchOnCell.rowID][branchOnCell.colID] = INFINITY : costTableau[branchOnCell.rowID][branchOnCell.colID] += delta;
	/*
	std::cout << "\nPrint updated cost tableau." << std::endl;
	for (int i = 0; i < costTableau.size(); i++) {
		for (int j = 0; j < costTableau.size(); j++) {
			std::cout << costTableau[i][j] << "  ";
		}
		std::cout << " " << std::endl;
	}
	*/
}

//generate list of routes
void OperatorTheory::generateListOfRoutes(std::map<int, int> boxPoints) {
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
		if (counter > 2 * costTableau.size()) {
			abnormalNode = true;
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
				clistOfRoutes.push_back(route);
				route.clear();
				i = 0;
			}
			else {
				i += 1;
			}
		}
	}
}

//generates child nodes
void OperatorTheory::generateChildNodes() {
	//generate child nodes 
	std::vector<int> route;
	if (clistOfRoutes.size() > 1) {
		double val = INFINITY;
		for (auto& it : clistOfRoutes) {
			if (it.size() < val) {
				val = it.size();
				route.clear();
				route = it;
			}
		}
		isSolutionATour = false;
		std::map<int, int> cellMaps;
		for (int i = 0; i < route.size() - 1; i++) {
			cellMaps.insert(std::pair<int, int>(route.at(i), route.at(i + 1)));
		}
		cellMaps.insert(std::pair<int, int>(route.at(route.size() - 1), route.at(0)));
		/*
		std::cout << "\nBranch on cells : " << std::endl;
		for (auto &it: cellMaps) {
			std::cout << "row id : " << it.first << " col id : " << it.second << std::endl;
		}
		*/
		//create nodes
		double nodeNum = parentNodeIndex;
		for (auto& it : cellMaps) {
			nodeNum += 1;
			Cell cell = Cell();
			cell.rowID = it.first;
			cell.colID = it.second;
			branchOnCell = cell;
			std::cout << "\nBranch on cell row id : " << branchOnCell.rowID << ", column id : " << branchOnCell.colID << std::endl;
			scanningRoutine(branchOnCell.rowID, branchOnCell.colID);
			findMaxDeltaAndEnteringCell(branchOnCell.rowID, branchOnCell.colID);
			double weakerLB = 0;
			double lb = 0;
			weakerLB = lowerBound + delta;
			lb = lowerBound;
			Node node = Node();
			node.nodeIndex = nodeNum;
			node.parentNodeIndex = parentNodeIndex;
			node.isSolutionATour = false;
			node.weakerLowerBound = weakerLB;
			node.lowerBound = lb;
			node.delta = delta;
			node.Ip = Ip;
			node.Iq = Iq;
			node.Jp = Jp;
			node.Jq = Jq;
			node.branchOnCell = branchOnCell;
			node.enteringCell = enteringCell;
			node.basicSolution = basicSolution;
			node.rowDualSolution = rowWiseDualSolution;
			node.columnDualSolution = columnWiseDualSolution;
			node.costTableau = costTableau;
			childNodes.push_back(node);
		}
	}
	else if (clistOfRoutes.size() == 1) {
		Node node = Node();
		node.nodeIndex = nodeIndex;
		node.parentNodeIndex = parentNodeIndex;
		node.isSolutionATour = true;
		node.weakerLowerBound = weakerLowerBound;
		node.lowerBound = lowerBound;
		node.delta = 0;
		node.basicSolution = basicSolution;
		node.rowDualSolution = rowWiseDualSolution;
		node.columnDualSolution = columnWiseDualSolution;
		node.costTableau = costTableau;
		childNodes.push_back(node);
	}
	std::cout << "\nNumber of child nodes generated : " << childNodes.size() << std::endl;
}

//update weaker lower bound
void OperatorTheory::runCostOperatorForGeneratingRootNodes() {
	generateDualSolution();
	std::map<int, int> boxPoints;
	for (auto& it : basicSolution) {
		if (it.value == 1.0) {
			boxPoints.insert(std::pair<int, int>(it.rowID, it.colID));
		}
	}
	generateListOfRoutes(boxPoints);
	updateBounds(boxPoints);
	generateChildNodes();
}

//update bounds
void OperatorTheory::updateBounds(std::map<int, int> bPoints) {
	double cost = 0;
	std::map<int, int> boxPoints = bPoints;
	for (auto& it : boxPoints) {
		cost += costTableau[it.first][it.second];
	}
	weakerLowerBound = cost;
	lowerBound = cost;
}

//run cost operator for solving a node
void OperatorTheory::runCostOperatorForSolvingANode() {
	Cell prevEnteringCell = Cell();
	Cell forbiddenCell = Cell();
	int counter = 0;
	int iterator = 0;
	bool breakByIterator = false;
	prevEnteringCell = enteringCell;
	while (isCycleFeasible == false) {
		iterator++;
		if (iterator > 2) {
			breakByIterator = true;
			break;
		}
		std::cout << "\nGenerate cycle with entering cell." << std::endl;
		generateCycleWithEnteringCell(enteringCell.rowID, enteringCell.colID);
		if (abnormalNode == true) {
			breakByIterator = true;
			break;
		}
		checkCycleFeasibililtyAndUpdateBasicSolutionCostTableau();
		if (isCycleFeasible == false) {
			counter++;
			updateDualSolution(delta);
			scanningRoutine(branchOnCell.rowID, branchOnCell.colID);
			if (counter < 2) {
				findMaxDeltaAndEnteringCell(branchOnCell.rowID, branchOnCell.colID);
			}
			else {
				forbiddenCell = enteringCell;
				findMaxDeltaAndEnteringCellWithForbiddenCell(branchOnCell.rowID, branchOnCell.colID, forbiddenCell);
				counter = 0;
			}
		}
		else {
			generateDualSolution();
		}
	}
	if (!breakByIterator) {
		std::cout << "\nGenerate routes from the basic solution." << std::endl;
		std::map<int, int> boxPoints;
		for (auto& it : basicSolution) {
			if (it.value == 1.0) {
				boxPoints.insert(std::pair<int, int>(it.rowID, it.colID));
			}
		}
		generateListOfRoutes(boxPoints);
		if (!abnormalNode) {
			updateBounds(boxPoints);
			std::cout << "\nShow the current subtours : " << std::endl;
			for (auto& it : clistOfRoutes) {
				for (auto itt : it) {
					std::cout << itt << " ";
				}
				std::cout << " " << std::endl;
			}
			generateChildNodes();
		}
	}
}

//returns child nodes
std::list<Node> OperatorTheory::getChildNodes() {
	return childNodes;
}

//prints the contents of the child nodes
void OperatorTheory::showChildNodes() {
	std::cout << "\nShow the child nodes : " << std::endl;
	for (auto& it : childNodes) {
		std::cout << "Node index : " << it.nodeIndex << std::endl;
		std::cout << "Parent node index : " << it.parentNodeIndex << std::endl;
		std::cout << "Is solution a tour : " << it.isSolutionATour << std::endl;
		std::cout << "Weaker lower bound : " << it.weakerLowerBound << std::endl;
		std::cout << "Lower bound : " << it.lowerBound << std::endl;
		std::cout << "Delta : " << it.delta << std::endl;
		std::cout << "Ip : " << std::endl;
		for (auto ip : it.Ip) {
			std::cout << ip << " ";
		}
		std::cout << "\nIq : " << std::endl;
		for (auto iq : it.Iq) {
			std::cout << iq << " ";
		}
		std::cout << "\nJp : " << std::endl;
		for (auto jp : it.Jp) {
			std::cout << jp << " ";
		}
		std::cout << "\nJq : " << std::endl;
		for (auto jq : it.Jq) {
			std::cout << jq << " ";
		}
		std::cout << "\nBranch on cell : " << "Row id : " << it.branchOnCell.rowID << ", Column id : " << it.branchOnCell.colID << std::endl;
		std::cout << "Entering cell : " << "Row id : " << it.enteringCell.rowID << ", Column id : " << it.enteringCell.colID << std::endl;
		std::cout << "Show basic solution : " << std::endl;
		for (auto& bSol : it.basicSolution) {
			std::cout << "Row id : " << bSol.rowID << " Column id : " << bSol.colID << " Value : " << bSol.value << std::endl;
		}
		std::cout << "Show rowwise dual solution : " << std::endl;
		for (auto& rd : it.rowDualSolution) {
			std::cout << rd << " ";
		}
		std::cout << "\nShow column wise dual solution : " << std::endl;
		for (auto& cd : it.columnDualSolution) {
			std::cout << cd << " ";
		}
		std::cout << "\nShow the cost tableau : " << std::endl;
		for (int i = 0; i < it.costTableau.size(); i++) {
			for (int j = 0; j < it.costTableau.size(); j++) {
				std::cout << it.costTableau[i][j] << " ";
			}
			std::cout << " " << std::endl;
		}
	}
}



