#include "RoutingTableHierarchy.h"
#include "Filter.h"

void showMenu() {
	std::cout << "======Menu======" << '\n';
	std::cout << "1. Filter by lifetime" << '\n';
	std::cout << "2. Filter by nexthop address" << '\n';
	std::cout << "3. Filter by matching with prefix address" << '\n';
	std::cout << "4. Print filtered data structure" << '\n';
	std::cout << "5. Exit the part" << '\n';
	std::cout << "Enter your choice: ";
}

void showParts() {
	std::cout << "======Parts======" << "\n";
	std::cout << "1. Part1 (simple vector)" << '\n';
	std::cout << "2. Part2 (explicit hierarchy)" << '\n';
	std::cout << "3. Exit the program" << '\n';
	std::cout << "Enter your choice: ";
}

void showMovementOptions() {
	std::cout << "Chose where to move" << "\n";
	std::cout << "1. To parent" << "\n";
	std::cout << "2. To son" << "\n";
	std::cout << "3. Start filtering" << "\n";
	std::cout << "Enter your choice: ";
}

void printFiltered(std::vector<RoutingRecord>& filteredVector) {
	for (RoutingRecord& record : filteredVector) {
		std::cout << record.getInfo() << std::endl;
	}
}

int main() {
	initHeapMonitor();
	std::string userNextHop;
	std::string userAddress;
	int userMask;
	std::string userFromTime;
	std::string userToTime;
	std::vector<RoutingRecord> filtered;
	RoutingTableHierarchy hierarchy = RoutingTableHierarchy();
	hierarchy.addFromVector();
	auto currentNode = RoutingTableHierarchyIterator(hierarchy.getHierarchy()->accessRoot(), hierarchy.getHierarchy());
	auto end = RoutingTableHierarchyIterator(nullptr, hierarchy.getHierarchy());
	auto matchNextHopPred = [&](RoutingRecord& record) { return record.matchNextHop(userNextHop); };
	auto matchWithAddressPred = [&](RoutingRecord& record) {return record.matchWithAddress(userAddress, userMask); };
	auto matchLifetimePred = [&](RoutingRecord& record) {return record.matchLifeTime(userFromTime, userToTime); };
	auto isLeaf = [&](RoutingRecordNode& record) { return record.getRecords().size() > 0;};

	auto pushToFiltered = [&](RoutingRecord& record) {
		filtered.push_back(record);
	};

	auto processLifetimePred = [&](RoutingRecordNode& node) {
		Filter::filter(node.getRecords().begin(), node.getRecords().end(), matchLifetimePred, pushToFiltered);
		};
	auto processNextHopPred = [&](RoutingRecordNode& node) {
		Filter::filter(node.getRecords().begin(), node.getRecords().end(), matchNextHopPred, pushToFiltered);
		};
	auto processAddressPred = [&](RoutingRecordNode& node) {
		Filter::filter(node.getRecords().begin(), node.getRecords().end(), matchWithAddressPred, pushToFiltered);
		};


	
}